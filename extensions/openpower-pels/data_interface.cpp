/**
 * Copyright © 2019 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "config.h"

#include "data_interface.hpp"

#include <fstream>
#include <xyz/openbmc_project/State/OperatingSystem/Status/server.hpp>

namespace openpower
{
namespace pels
{

namespace service_name
{
constexpr auto objectMapper = "xyz.openbmc_project.ObjectMapper";
} // namespace service_name

namespace object_path
{
constexpr auto objectMapper = "/xyz/openbmc_project/object_mapper";
constexpr auto systemInv = "/xyz/openbmc_project/inventory/system";
constexpr auto hostState = "/xyz/openbmc_project/state/host0";
} // namespace object_path

namespace interface
{
constexpr auto dbusProperty = "org.freedesktop.DBus.Properties";
constexpr auto objectMapper = "xyz.openbmc_project.ObjectMapper";
constexpr auto invAsset = "xyz.openbmc_project.Inventory.Decorator.Asset";
constexpr auto osStatus = "xyz.openbmc_project.State.OperatingSystem.Status";
} // namespace interface

using namespace sdbusplus::xyz::openbmc_project::State::OperatingSystem::server;

DataInterface::DataInterface(sdbusplus::bus::bus& bus) : _bus(bus)
{
    readMTMS();
    readHostState();
    readBMCFWVersion();
    readServerFWVersion();
}

void DataInterface::readMTMS()
{
    // If this runs when the inventory service isn't running, it will get the
    // value whenever it starts via the propertiesChanged callback.
    try
    {
        auto inventoryService =
            getService(object_path::systemInv, interface::invAsset);

        if (!inventoryService.empty())
        {
            auto properties = getAllProperties(
                inventoryService, object_path::systemInv, interface::invAsset);

            _machineTypeModel = std::get<std::string>(properties["Model"]);

            _machineSerialNumber =
                std::get<std::string>(properties["SerialNumber"]);
        }
    }
    catch (std::exception& e)
    {
        // Inventory must not be running at this moment.
    }

    // Keep up to date by watching for the propertiesChanged signal.
    _sysInventoryPropMatch = std::make_unique<sdbusplus::bus::match_t>(
        _bus,
        sdbusplus::bus::match::rules::propertiesChanged(object_path::systemInv,
                                                        interface::invAsset),
        std::bind(std::mem_fn(&DataInterface::sysAssetPropChanged), this,
                  std::placeholders::_1));
}

void DataInterface::sysAssetPropChanged(sdbusplus::message::message& msg)
{
    DBusInterface interface;
    DBusPropertyMap properties;

    msg.read(interface, properties);

    auto model = properties.find("Model");
    if (model != properties.end())
    {
        _machineTypeModel = std::get<std::string>(model->second);
    }

    auto sn = properties.find("SerialNumber");
    if (sn != properties.end())
    {
        _machineSerialNumber = std::get<std::string>(sn->second);
    }
}

DBusPropertyMap DataInterface::getAllProperties(const std::string& service,
                                                const std::string& objectPath,
                                                const std::string& interface)
{
    DBusPropertyMap properties;

    auto method = _bus.new_method_call(service.c_str(), objectPath.c_str(),
                                       interface::dbusProperty, "GetAll");
    method.append(interface);
    auto reply = _bus.call(method);

    reply.read(properties);

    return properties;
}

void DataInterface::getProperty(const std::string& service,
                                const std::string& objectPath,
                                const std::string& interface,
                                const std::string& property, DBusValue& value)
{

    auto method = _bus.new_method_call(service.c_str(), objectPath.c_str(),
                                       interface::dbusProperty, "Get");
    method.append(interface, property);
    auto reply = _bus.call(method);

    reply.read(value);
}

DBusService DataInterface::getService(const std::string& objectPath,
                                      const std::string& interface)
{
    auto method = _bus.new_method_call(service_name::objectMapper,
                                       object_path::objectMapper,
                                       interface::objectMapper, "GetObject");

    method.append(objectPath, std::vector<std::string>({interface}));

    auto reply = _bus.call(method);

    std::map<DBusService, DBusInterfaceList> response;
    reply.read(response);

    if (!response.empty())
    {
        return response.begin()->first;
    }

    return std::string{};
}

void DataInterface::readHostState()
{
    _hostUp = false;

    try
    {
        auto service = getService(object_path::hostState, interface::osStatus);
        if (!service.empty())
        {
            DBusValue value;
            getProperty(service, object_path::hostState, interface::osStatus,
                        "OperatingSystemState", value);

            auto status =
                Status::convertOSStatusFromString(std::get<std::string>(value));

            if ((status == Status::OSStatus::BootComplete) ||
                (status == Status::OSStatus::Standby))
            {
                _hostUp = true;
            }
        }
    }
    catch (std::exception& e)
    {
        // Not available yet.
    }

    // Keep up to date by watching for the propertiesChanged signal.
    _osStateMatch = std::make_unique<sdbusplus::bus::match_t>(
        _bus,
        sdbusplus::bus::match::rules::propertiesChanged(object_path::hostState,
                                                        interface::osStatus),
        std::bind(std::mem_fn(&DataInterface::osStatePropChanged), this,
                  std::placeholders::_1));
}

void DataInterface::osStatePropChanged(sdbusplus::message::message& msg)
{
    DBusInterface interface;
    DBusPropertyMap properties;

    msg.read(interface, properties);

    auto state = properties.find("OperatingSystemState");
    if (state != properties.end())
    {
        auto status = Status::convertOSStatusFromString(
            std::get<std::string>(state->second));

        bool newHostState = false;
        if ((status == Status::OSStatus::BootComplete) ||
            (status == Status::OSStatus::Standby))
        {
            newHostState = true;
        }

        setHostState(newHostState);
    }
}

void DataInterface::readBMCFWVersion()
{
    std::ifstream versionFile{BMC_VERSION_FILE};
    std::string line;
    static const auto versionID = "VERSION=";

    while (std::getline(versionFile, line))
    {
        if (line.find(versionID) != std::string::npos)
        {
            auto pos = line.find_first_of('"') + 1;
            _bmcFWVersion = line.substr(pos, line.find_last_of('"') - pos);
            break;
        }
    }
}

void DataInterface::readServerFWVersion()
{
    // Not available yet
}

} // namespace pels
} // namespace openpower
