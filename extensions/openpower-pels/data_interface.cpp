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

#include "util.hpp"

#include <fmt/format.h>

#include <fstream>
#include <phosphor-logging/log.hpp>
#include <xyz/openbmc_project/State/Boot/Progress/server.hpp>

namespace openpower
{
namespace pels
{

namespace service_name
{
constexpr auto objectMapper = "xyz.openbmc_project.ObjectMapper";
constexpr auto vpdManager = "com.ibm.VPD.Manager";
constexpr auto ledGroupManager = "xyz.openbmc_project.LED.GroupManager";
} // namespace service_name

namespace object_path
{
constexpr auto objectMapper = "/xyz/openbmc_project/object_mapper";
constexpr auto systemInv = "/xyz/openbmc_project/inventory/system";
constexpr auto chassisInv = "/xyz/openbmc_project/inventory/system/chassis";
constexpr auto motherBoardInv =
    "/xyz/openbmc_project/inventory/system/chassis/motherboard";
constexpr auto baseInv = "/xyz/openbmc_project/inventory";
constexpr auto bmcState = "/xyz/openbmc_project/state/bmc0";
constexpr auto chassisState = "/xyz/openbmc_project/state/chassis0";
constexpr auto hostState = "/xyz/openbmc_project/state/host0";
constexpr auto pldm = "/xyz/openbmc_project/pldm";
constexpr auto enableHostPELs =
    "/xyz/openbmc_project/logging/send_event_logs_to_host";
constexpr auto vpdManager = "/com/ibm/VPD/Manager";
} // namespace object_path

namespace interface
{
constexpr auto dbusProperty = "org.freedesktop.DBus.Properties";
constexpr auto objectMapper = "xyz.openbmc_project.ObjectMapper";
constexpr auto invAsset = "xyz.openbmc_project.Inventory.Decorator.Asset";
constexpr auto bootProgress = "xyz.openbmc_project.State.Boot.Progress";
constexpr auto pldmRequester = "xyz.openbmc_project.PLDM.Requester";
constexpr auto enable = "xyz.openbmc_project.Object.Enable";
constexpr auto bmcState = "xyz.openbmc_project.State.BMC";
constexpr auto chassisState = "xyz.openbmc_project.State.Chassis";
constexpr auto hostState = "xyz.openbmc_project.State.Host";
constexpr auto invMotherboard =
    "xyz.openbmc_project.Inventory.Item.Board.Motherboard";
constexpr auto viniRecordVPD = "com.ibm.ipzvpd.VINI";
constexpr auto locCode = "com.ibm.ipzvpd.Location";
constexpr auto compatible =
    "xyz.openbmc_project.Configuration.IBMCompatibleSystem";
constexpr auto vpdManager = "com.ibm.VPD.Manager";
constexpr auto ledGroup = "xyz.openbmc_project.Led.Group";
constexpr auto operationalStatus =
    "xyz.openbmc_project.State.Decorator.OperationalStatus";
} // namespace interface

using namespace sdbusplus::xyz::openbmc_project::State::Boot::server;
using sdbusplus::exception::SdBusError;
using namespace phosphor::logging;

DataInterface::DataInterface(sdbusplus::bus::bus& bus) : _bus(bus)
{
    readBMCFWVersion();
    readServerFWVersion();
    readBMCFWVersionID();

    // Watch the BootProgress property
    _properties.emplace_back(std::make_unique<PropertyWatcher<DataInterface>>(
        bus, object_path::hostState, interface::bootProgress, "BootProgress",
        *this, [this](const auto& value) {
            auto status = Progress::convertProgressStagesFromString(
                std::get<std::string>(value));

            if ((status == Progress::ProgressStages::SystemInitComplete) ||
                (status == Progress::ProgressStages::OSStart) ||
                (status == Progress::ProgressStages::OSRunning))
            {
                setHostUp(true);
            }
            else
            {
                setHostUp(false);
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

DBusPathList DataInterface::getPaths(const DBusInterfaceList& interfaces) const
{

    auto method = _bus.new_method_call(
        service_name::objectMapper, object_path::objectMapper,
        interface::objectMapper, "GetSubTreePaths");

    method.append(std::string{"/"}, 0, interfaces);

    auto reply = _bus.call(method);

    DBusPathList paths;
    reply.read(paths);

    return paths;
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

void DataInterface::readBMCFWVersion()
{
    _bmcFWVersion =
        phosphor::logging::util::getOSReleaseValue("VERSION").value_or("");
}

void DataInterface::readServerFWVersion()
{
    // Not available yet
}

void DataInterface::readBMCFWVersionID()
{
    _bmcFWVersionID =
        phosphor::logging::util::getOSReleaseValue("VERSION_ID").value_or("");
}

std::string DataInterface::getMachineTypeModel() const
{
    std::string model;
    try
    {

        auto service = getService(object_path::systemInv, interface::invAsset);
        if (!service.empty())
        {
            DBusValue value;
            getProperty(service, object_path::systemInv, interface::invAsset,
                        "Model", value);

            model = std::get<std::string>(value);
        }
    }
    catch (const std::exception& e)
    {
        log<level::WARNING>(fmt::format("Failed reading Model property from "
                                        "Interface: {} exception: {}",
                                        interface::viniRecordVPD, e.what())
                                .c_str());
    }

    return model;
}

std::string DataInterface::getMachineSerialNumber() const
{
    std::string sn;
    try
    {

        auto service = getService(object_path::systemInv, interface::invAsset);
        if (!service.empty())
        {
            DBusValue value;
            getProperty(service, object_path::systemInv, interface::invAsset,
                        "SerialNumber", value);

            sn = std::get<std::string>(value);
        }
    }
    catch (const std::exception& e)
    {
        log<level::WARNING>(fmt::format("Failed reading Model property from "
                                        "Interface: {} exception: {}",
                                        interface::viniRecordVPD, e.what())
                                .c_str());
    }

    return sn;
}

std::string DataInterface::getMotherboardCCIN() const
{
    std::string ccin;

    try
    {
        auto service =
            getService(object_path::motherBoardInv, interface::viniRecordVPD);
        if (!service.empty())
        {
            DBusValue value;
            getProperty(service, object_path::motherBoardInv,
                        interface::viniRecordVPD, "CC", value);

            auto cc = std::get<std::vector<uint8_t>>(value);
            ccin = std::string{cc.begin(), cc.end()};
        }
    }
    catch (const std::exception& e)
    {
        log<level::WARNING>(fmt::format("Failed reading Model property from "
                                        "Interface: {} exception: {}",
                                        interface::viniRecordVPD, e.what())
                                .c_str());
    }

    return ccin;
}

void DataInterface::getHWCalloutFields(const std::string& inventoryPath,
                                       std::string& fruPartNumber,
                                       std::string& ccin,
                                       std::string& serialNumber) const
{
    // For now, attempt to get all of the properties directly on the path
    // passed in.  In the future, may need to make use of an algorithm
    // to figure out which inventory objects actually hold these
    // interfaces in the case of non FRUs, or possibly another service
    // will provide this info.  Any missing interfaces will result
    // in exceptions being thrown.

    auto service = getService(inventoryPath, interface::viniRecordVPD);

    auto properties =
        getAllProperties(service, inventoryPath, interface::viniRecordVPD);

    auto value = std::get<std::vector<uint8_t>>(properties["FN"]);
    fruPartNumber = std::string{value.begin(), value.end()};

    value = std::get<std::vector<uint8_t>>(properties["CC"]);
    ccin = std::string{value.begin(), value.end()};

    value = std::get<std::vector<uint8_t>>(properties["SN"]);
    serialNumber = std::string{value.begin(), value.end()};
}

std::string
    DataInterface::getLocationCode(const std::string& inventoryPath) const
{
    auto service = getService(inventoryPath, interface::locCode);

    DBusValue locCode;
    getProperty(service, inventoryPath, interface::locCode, "LocationCode",
                locCode);

    return std::get<std::string>(locCode);
}

std::string
    DataInterface::addLocationCodePrefix(const std::string& locationCode)
{
    static const std::string locationCodePrefix{"Ufcs-"};

    // Technically there are 2 location code prefixes, Ufcs and Umts, so
    // if it already starts with a U then don't need to do anything.
    if (locationCode.front() != 'U')
    {
        return locationCodePrefix + locationCode;
    }

    return locationCode;
}

std::string DataInterface::expandLocationCode(const std::string& locationCode,
                                              uint16_t /*node*/) const
{
    auto method =
        _bus.new_method_call(service_name::vpdManager, object_path::vpdManager,
                             interface::vpdManager, "GetExpandedLocationCode");

    method.append(addLocationCodePrefix(locationCode),
                  static_cast<uint16_t>(0));

    auto reply = _bus.call(method);

    std::string expandedLocationCode;
    reply.read(expandedLocationCode);

    return expandedLocationCode;
}

std::string
    DataInterface::getInventoryFromLocCode(const std::string& locationCode,
                                           uint16_t node, bool expanded) const
{
    std::string methodName = expanded ? "GetFRUsByExpandedLocationCode"
                                      : "GetFRUsByUnexpandedLocationCode";

    auto method =
        _bus.new_method_call(service_name::vpdManager, object_path::vpdManager,
                             interface::vpdManager, methodName.c_str());

    if (expanded)
    {
        method.append(locationCode);
    }
    else
    {
        method.append(addLocationCodePrefix(locationCode), node);
    }

    auto reply = _bus.call(method);

    std::vector<sdbusplus::message::object_path> entries;
    reply.read(entries);

    // Get the shortest entry from the paths received, as this
    // would be the path furthest up the inventory hierarchy so
    // would be the parent FRU.  There is guaranteed to at least
    // be one entry if the call didn't fail.
    std::string shortest{entries[0]};

    std::for_each(entries.begin(), entries.end(),
                  [&shortest](const auto& path) {
                      if (path.str.size() < shortest.size())
                      {
                          shortest = path;
                      }
                  });

    return shortest;
}

void DataInterface::assertLEDGroup(const std::string& ledGroup,
                                   bool value) const
{
    DBusValue variant = value;
    auto method =
        _bus.new_method_call(service_name::ledGroupManager, ledGroup.c_str(),
                             interface::dbusProperty, "Set");
    method.append(interface::ledGroup, "Asserted", variant);
    _bus.call(method);
}

void DataInterface::setFunctional(const std::string& objectPath,
                                  bool value) const
{
    DBusValue variant = value;
    auto service = getService(objectPath, interface::operationalStatus);

    auto method = _bus.new_method_call(service.c_str(), objectPath.c_str(),
                                       interface::dbusProperty, "Set");

    method.append(interface::operationalStatus, "Functional", variant);
    _bus.call(method);
}

std::vector<std::string> DataInterface::getSystemNames() const
{
    DBusSubTree subtree;
    DBusValue names;

    auto method = _bus.new_method_call(service_name::objectMapper,
                                       object_path::objectMapper,
                                       interface::objectMapper, "GetSubTree");
    method.append(std::string{"/"}, 0,
                  std::vector<std::string>{interface::compatible});
    auto reply = _bus.call(method);

    reply.read(subtree);
    if (subtree.empty())
    {
        throw std::runtime_error("Compatible interface not on D-Bus");
    }

    const auto& object = *(subtree.begin());
    const auto& path = object.first;
    const auto& service = object.second.begin()->first;

    getProperty(service, path, interface::compatible, "Names", names);

    return std::get<std::vector<std::string>>(names);
}

} // namespace pels
} // namespace openpower
