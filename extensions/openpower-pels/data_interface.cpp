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
#include <iterator>
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
constexpr auto logSetting = "xyz.openbmc_project.Settings";
constexpr auto hwIsolation = "org.open_power.HardwareIsolation";
constexpr auto bootRawProgress = "xyz.openbmc_project.State.Boot.Raw";
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
constexpr auto logSetting = "/xyz/openbmc_project/logging/settings";
constexpr auto hwIsolation = "/xyz/openbmc_project/hardware_isolation";
constexpr auto bootRawSetting = "/xyz/openbmc_project/state/boot/raw0";
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
constexpr auto vsbpRecordVPD = "com.ibm.ipzvpd.VSBP";
constexpr auto locCode = "xyz.openbmc_project.Inventory.Decorator.LocationCode";
constexpr auto compatible =
    "xyz.openbmc_project.Configuration.IBMCompatibleSystem";
constexpr auto vpdManager = "com.ibm.VPD.Manager";
constexpr auto ledGroup = "xyz.openbmc_project.Led.Group";
constexpr auto operationalStatus =
    "xyz.openbmc_project.State.Decorator.OperationalStatus";
constexpr auto logSetting = "xyz.openbmc_project.Logging.Settings";
constexpr auto association = "xyz.openbmc_project.Association.Definitions";
constexpr auto dumpEntry = "xyz.openbmc_project.Dump.Entry";
constexpr auto dumpProgress = "xyz.openbmc_project.Common.Progress";
constexpr auto hwIsolationCreate = "org.open_power.HardwareIsolation.Create";
constexpr auto bootRawProgress = "xyz.openbmc_project.State.Boot.Raw";
} // namespace interface

using namespace sdbusplus::xyz::openbmc_project::State::Boot::server;
using namespace phosphor::logging;

std::pair<std::string, std::string>
    DataInterfaceBase::extractConnectorFromLocCode(
        const std::string& locationCode)
{
    auto base = locationCode;
    std::string connector{};

    auto pos = base.find("-T");
    if (pos != std::string::npos)
    {
        connector = base.substr(pos);
        base = base.substr(0, pos);
    }

    return {base, connector};
}

DataInterface::DataInterface(sdbusplus::bus::bus& bus) : _bus(bus)
{
    readBMCFWVersion();
    readServerFWVersion();
    readBMCFWVersionID();

    // Watch the BootProgress property
    _properties.emplace_back(std::make_unique<PropertyWatcher<DataInterface>>(
        bus, object_path::hostState, interface::bootProgress, "BootProgress",
        *this, [this](const auto& value) {
            this->_bootState = std::get<std::string>(value);
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
    auto value =
        phosphor::logging::util::getOSReleaseValue("VERSION_ID").value_or("");
    if ((value != "") && (value.find_last_of(')') != std::string::npos))
    {
        std::size_t pos = value.find_first_of('(') + 1;
        _serverFWVersion = value.substr(pos, value.find_last_of(')') - pos);
    }
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
                                        interface::invAsset, e.what())
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
        log<level::WARNING>(
            fmt::format("Failed reading SerialNumber property from "
                        "Interface: {} exception: {}",
                        interface::invAsset, e.what())
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
        log<level::WARNING>(
            fmt::format("Failed reading Motherboard CCIN property from "
                        "Interface: {} exception: {}",
                        interface::viniRecordVPD, e.what())
                .c_str());
    }

    return ccin;
}

std::vector<uint8_t> DataInterface::getSystemIMKeyword() const
{
    std::vector<uint8_t> systemIM;

    try
    {
        auto service =
            getService(object_path::motherBoardInv, interface::vsbpRecordVPD);
        if (!service.empty())
        {
            DBusValue value;
            getProperty(service, object_path::motherBoardInv,
                        interface::vsbpRecordVPD, "IM", value);

            systemIM = std::get<std::vector<uint8_t>>(value);
        }
    }
    catch (const std::exception& e)
    {
        log<level::WARNING>(
            fmt::format("Failed reading System IM property from "
                        "Interface: {} exception: {}",
                        interface::vsbpRecordVPD, e.what())
                .c_str());
    }

    return systemIM;
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
    // Location codes for connectors are the location code of the FRU they are
    // on, plus a '-Tx' segment.  Remove this last segment before expanding it
    // and then add it back in afterwards.  This way, the connector doesn't have
    // to be in the model just so that it can be expanded.
    auto [baseLoc, connectorLoc] = extractConnectorFromLocCode(locationCode);

    auto method =
        _bus.new_method_call(service_name::vpdManager, object_path::vpdManager,
                             interface::vpdManager, "GetExpandedLocationCode");

    method.append(addLocationCodePrefix(baseLoc), static_cast<uint16_t>(0));

    auto reply = _bus.call(method);

    std::string expandedLocationCode;
    reply.read(expandedLocationCode);

    if (!connectorLoc.empty())
    {
        expandedLocationCode += connectorLoc;
    }

    return expandedLocationCode;
}

std::string
    DataInterface::getInventoryFromLocCode(const std::string& locationCode,
                                           uint16_t node, bool expanded) const
{
    std::string methodName = expanded ? "GetFRUsByExpandedLocationCode"
                                      : "GetFRUsByUnexpandedLocationCode";

    // Remove the connector segment, if present, so that this method call
    // returns an inventory path that getHWCalloutFields() can be used with.
    // (The serial number, etc, aren't stored on the connector in the
    // inventory, and may not even be modeled.)
    auto [baseLoc, connectorLoc] = extractConnectorFromLocCode(locationCode);

    auto method =
        _bus.new_method_call(service_name::vpdManager, object_path::vpdManager,
                             interface::vpdManager, methodName.c_str());

    if (expanded)
    {
        method.append(baseLoc);
    }
    else
    {
        method.append(addLocationCodePrefix(baseLoc), node);
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

using AssociationTuple = std::tuple<std::string, std::string, std::string>;
using AssociationsProperty = std::vector<AssociationTuple>;

void DataInterface::setCriticalAssociation(const std::string& objectPath) const
{
    DBusValue getAssociationValue;

    auto service = getService(objectPath, interface::association);

    getProperty(service, objectPath, interface::association, "Associations",
                getAssociationValue);

    auto association = std::get<AssociationsProperty>(getAssociationValue);

    AssociationTuple critAssociation{
        "health_rollup", "critical",
        "/xyz/openbmc_project/inventory/system/chassis"};

    if (std::find(association.begin(), association.end(), critAssociation) ==
        association.end())
    {
        association.push_back(critAssociation);
        DBusValue setAssociationValue = association;

        auto method = _bus.new_method_call(service.c_str(), objectPath.c_str(),
                                           interface::dbusProperty, "Set");

        method.append(interface::association, "Associations",
                      setAssociationValue);
        _bus.call(method);
    }
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

bool DataInterface::getQuiesceOnError() const
{
    bool ret = false;

    try
    {
        auto service =
            getService(object_path::logSetting, interface::logSetting);
        if (!service.empty())
        {
            DBusValue value;
            getProperty(service, object_path::logSetting, interface::logSetting,
                        "QuiesceOnHwError", value);

            ret = std::get<bool>(value);
        }
    }
    catch (const std::exception& e)
    {
        log<level::WARNING>(
            fmt::format("Failed reading QuiesceOnHwError property from "
                        "Interface: {} exception: {}",
                        interface::logSetting, e.what())
                .c_str());
    }

    return ret;
}

std::vector<bool>
    DataInterface::checkDumpStatus(const std::vector<std::string>& type) const
{
    DBusSubTree subtree;
    std::vector<bool> result(type.size(), false);

    // Query GetSubTree for the availability of dump interface
    auto method = _bus.new_method_call(service_name::objectMapper,
                                       object_path::objectMapper,
                                       interface::objectMapper, "GetSubTree");
    method.append(std::string{"/"}, 0,
                  std::vector<std::string>{interface::dumpEntry});
    auto reply = _bus.call(method);

    reply.read(subtree);

    if (subtree.empty())
    {
        return result;
    }

    std::vector<bool>::iterator itDumpStatus = result.begin();
    uint8_t count = 0;
    for (const auto& [path, serviceInfo] : subtree)
    {
        const auto& service = serviceInfo.begin()->first;
        // Check for dump type on the object path
        for (const auto& it : type)
        {
            if (path.find(it) != std::string::npos)
            {
                DBusValue value, progress;

                // If dump type status is already available go for next path
                if (*itDumpStatus)
                {
                    break;
                }

                // Check for valid dump to be available if following
                // conditions are met for the dump entry path -
                // Offloaded == false and Status == Completed
                getProperty(service, path, interface::dumpEntry, "Offloaded",
                            value);
                getProperty(service, path, interface::dumpProgress, "Status",
                            progress);
                auto offload = std::get<bool>(value);
                auto status = std::get<std::string>(progress);
                if (!offload && (status.find("Completed") != std::string::npos))
                {
                    *itDumpStatus = true;
                    count++;
                    if (count >= type.size())
                    {
                        return result;
                    }
                    break;
                }
            }
            itDumpStatus++;
        }
        itDumpStatus = result.begin();
    }

    return result;
}

void DataInterface::createGuardRecord(const std::vector<uint8_t>& binPath,
                                      const std::string& type,
                                      const std::string& logPath) const
{
    try
    {
        auto method = _bus.new_method_call(
            service_name::hwIsolation, object_path::hwIsolation,
            interface::hwIsolationCreate, "CreateWithEntityPath");
        method.append(binPath, type, sdbusplus::message::object_path(logPath));
        // Note: hw isolation "CreateWithEntityPath" got dependency on logging
        // api's. Making d-bus call no reply type to avoid cyclic dependency.
        // Added minimal timeout to catch initial failures.
        // Need to revisit this design later to avoid cyclic dependency.
        constexpr auto dbusTimeout = 100000; // in micro seconds
        _bus.call_noreply(method, dbusTimeout);
    }

    catch (const sdbusplus::exception::exception& e)
    {
        std::string errName = e.name();
        // SD_BUS_ERROR_TIMEOUT error is expected, due to PEL api dependency
        // mentioned above. Ignoring the error.
        if (errName != SD_BUS_ERROR_TIMEOUT)
        {
            log<level::ERR>(
                fmt::format("GUARD D-Bus call exception"
                            "OBJPATH={}, INTERFACE={}, EXCEPTION={}",
                            object_path::hwIsolation,
                            interface::hwIsolationCreate, e.what())
                    .c_str());
        }
    }
}

void DataInterface::createProgressSRC(
    const uint64_t& priSRC, const std::vector<uint8_t>& srcStruct) const
{
    DBusValue variant = std::make_tuple(priSRC, srcStruct);

    auto method = _bus.new_method_call(service_name::bootRawProgress,
                                       object_path::bootRawSetting,
                                       interface::dbusProperty, "Set");

    method.append(interface::bootRawProgress, "Value", variant);

    _bus.call(method);
}
} // namespace pels
} // namespace openpower
