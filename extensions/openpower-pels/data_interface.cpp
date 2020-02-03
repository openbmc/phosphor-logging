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
constexpr auto bmcState = "/xyz/openbmc_project/state/bmc0";
constexpr auto chassisState = "/xyz/openbmc_project/state/chassis0";
constexpr auto hostState = "/xyz/openbmc_project/state/host0";
constexpr auto pldm = "/xyz/openbmc_project/pldm";
constexpr auto enableHostPELs =
    "/xyz/openbmc_project/logging/send_event_logs_to_host";
} // namespace object_path

namespace interface
{
constexpr auto dbusProperty = "org.freedesktop.DBus.Properties";
constexpr auto objectMapper = "xyz.openbmc_project.ObjectMapper";
constexpr auto invAsset = "xyz.openbmc_project.Inventory.Decorator.Asset";
constexpr auto osStatus = "xyz.openbmc_project.State.OperatingSystem.Status";
constexpr auto pldmRequester = "xyz.openbmc_project.PLDM.Requester";
constexpr auto enable = "xyz.openbmc_project.Object.Enable";
constexpr auto bmcState = "xyz.openbmc_project.State.BMC";
constexpr auto chassisState = "xyz.openbmc_project.State.Chassis";
constexpr auto hostState = "xyz.openbmc_project.State.Host";
} // namespace interface

using namespace sdbusplus::xyz::openbmc_project::State::OperatingSystem::server;

DataInterface::DataInterface(sdbusplus::bus::bus& bus) : _bus(bus)
{
    readBMCFWVersion();
    readServerFWVersion();
    readBMCFWVersionID();

    // Watch both the Model and SN properties on the system's Asset iface
    _properties.emplace_back(std::make_unique<InterfaceWatcher<DataInterface>>(
        bus, object_path::systemInv, interface::invAsset, *this,
        [this](const auto& properties) {
            auto model = properties.find("Model");
            if (model != properties.end())
            {
                this->_machineTypeModel = std::get<std::string>(model->second);
            }

            auto sn = properties.find("SerialNumber");
            if (sn != properties.end())
            {
                this->_machineSerialNumber = std::get<std::string>(sn->second);
            }
        }));

    // Watch the OperatingSystemState property
    _properties.emplace_back(std::make_unique<PropertyWatcher<DataInterface>>(
        bus, object_path::hostState, interface::osStatus,
        "OperatingSystemState", *this, [this](const auto& value) {
            auto status =
                Status::convertOSStatusFromString(std::get<std::string>(value));

            if ((status == Status::OSStatus::BootComplete) ||
                (status == Status::OSStatus::Standby))
            {
                setHostState(true);
            }
            else
            {
                setHostState(false);
            }
        }));

    // Watch the host PEL enable property
    _properties.emplace_back(std::make_unique<PropertyWatcher<DataInterface>>(
        bus, object_path::enableHostPELs, interface::enable, "Enabled", *this,
        [this](const auto& value) {
            this->_sendPELsToHost = std::get<bool>(value);
        }));

    // Watch the BMCState property
    _properties.emplace_back(std::make_unique<PropertyWatcher<DataInterface>>(
        bus, object_path::bmcState, interface::bmcState, "CurrentBMCState",
        *this, [this](const auto& value) {
            this->_bmcState = std::get<std::string>(value);
        }));

    // Watch the chassis current and requested power state properties
    _properties.emplace_back(std::make_unique<InterfaceWatcher<DataInterface>>(
        bus, object_path::chassisState, interface::chassisState, *this,
        [this](const auto& properties) {
            auto state = properties.find("CurrentPowerState");
            if (state != properties.end())
            {
                this->_chassisState = std::get<std::string>(state->second);
            }

            auto trans = properties.find("RequestedPowerTransition");
            if (trans != properties.end())
            {
                this->_chassisTransition = std::get<std::string>(trans->second);
            }
        }));

    // Watch the CurrentHostState property
    _properties.emplace_back(std::make_unique<PropertyWatcher<DataInterface>>(
        bus, object_path::hostState, interface::hostState, "CurrentHostState",
        *this, [this](const auto& value) {
            this->_hostState = std::get<std::string>(value);
        }));
}

DBusPropertyMap
    DataInterface::getAllProperties(const std::string& service,
                                    const std::string& objectPath,
                                    const std::string& interface) const
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
                                const std::string& property,
                                DBusValue& value) const
{

    auto method = _bus.new_method_call(service.c_str(), objectPath.c_str(),
                                       interface::dbusProperty, "Get");
    method.append(interface, property);
    auto reply = _bus.call(method);

    reply.read(value);
}

DBusService DataInterface::getService(const std::string& objectPath,
                                      const std::string& interface) const
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

uint8_t DataInterface::getPLDMInstanceID(uint8_t eid) const
{
    return 0;
// Don't use until PLDM switches to async D-Bus.
#if 0
    auto service = getService(object_path::pldm, interface::pldmRequester);

    auto method =
        _bus.new_method_call(service.c_str(), object_path::pldm,
                             interface::pldmRequester, "GetInstanceId");
    method.append(eid);
    auto reply = _bus.call(method);

    uint8_t instanceID = 0;
    reply.read(instanceID);

    return instanceID;
#endif
}

/**
 * @brief Return a value found in the /etc/os-release file
 *
 * @param[in] key - The key name, like "VERSION"
 *
 * @return std::optional<std::string> - The value
 */
std::optional<std::string> getOSReleaseValue(const std::string& key)
{
    std::ifstream versionFile{BMC_VERSION_FILE};
    std::string line;
    std::string keyPattern{key + '='};

    while (std::getline(versionFile, line))
    {
        if (line.find(keyPattern) != std::string::npos)
        {
            auto pos = line.find_first_of('"') + 1;
            auto value = line.substr(pos, line.find_last_of('"') - pos);
            return value;
        }
    }

    return std::nullopt;
}

void DataInterface::readBMCFWVersion()
{
    _bmcFWVersion = getOSReleaseValue("VERSION").value_or("");
}

void DataInterface::readServerFWVersion()
{
    // Not available yet
}

void DataInterface::readBMCFWVersionID()
{
    _bmcFWVersionID = getOSReleaseValue("VERSION_ID").value_or("");
}

} // namespace pels
} // namespace openpower
