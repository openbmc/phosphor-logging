// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "data_interface.hpp"

#include "util.hpp"

#include <phosphor-logging/lg2.hpp>
#include <xyz/openbmc_project/State/BMC/server.hpp>
#include <xyz/openbmc_project/State/Boot/Progress/server.hpp>

#include <filesystem>

#ifdef PEL_ENABLE_PHAL
#include <libekb.H>
#include <libpdbg.h>
#include <libphal.H>
#endif

// Use a timeout of 10s for D-Bus calls so if there are
// timeouts the callers of the PEL creation method won't
// also timeout.
constexpr auto dbusTimeout = 10000000;

namespace openpower
{
namespace pels
{

namespace service_name
{
constexpr auto objectMapper = "xyz.openbmc_project.ObjectMapper";
constexpr auto vpdManager = "com.ibm.VPD.Manager";
constexpr auto ledGroupManager = "xyz.openbmc_project.LED.GroupManager";
constexpr auto biosConfigMgr = "xyz.openbmc_project.BIOSConfigManager";
constexpr auto bootRawProgress = "xyz.openbmc_project.State.Boot.Raw";
constexpr auto pldm = "xyz.openbmc_project.PLDM";
constexpr auto inventoryManager = "xyz.openbmc_project.Inventory.Manager";
constexpr auto entityManager = "xyz.openbmc_project.EntityManager";
constexpr auto systemd = "org.freedesktop.systemd1";
} // namespace service_name

namespace object_path
{
constexpr auto objectMapper = "/xyz/openbmc_project/object_mapper";
constexpr auto systemInv = "/xyz/openbmc_project/inventory/system";
constexpr auto motherBoardInv =
    "/xyz/openbmc_project/inventory/system/chassis/motherboard";
constexpr auto baseInv = "/xyz/openbmc_project/inventory";
constexpr auto bmcState = "/xyz/openbmc_project/state/bmc0";
constexpr auto chassisState = "/xyz/openbmc_project/state/chassis0";
constexpr auto hostState = "/xyz/openbmc_project/state/host0";
constexpr auto enableHostPELs =
    "/xyz/openbmc_project/logging/send_event_logs_to_host";
constexpr auto vpdManager = "/com/ibm/VPD/Manager";
constexpr auto logSetting = "/xyz/openbmc_project/logging/settings";
constexpr auto biosConfigMgr = "/xyz/openbmc_project/bios_config/manager";
constexpr auto bootRawProgress = "/xyz/openbmc_project/state/boot/raw0";
constexpr auto systemd = "/org/freedesktop/systemd1";
} // namespace object_path

namespace interface
{
constexpr auto dbusProperty = "org.freedesktop.DBus.Properties";
constexpr auto objectMapper = "xyz.openbmc_project.ObjectMapper";
constexpr auto invAsset = "xyz.openbmc_project.Inventory.Decorator.Asset";
constexpr auto bootProgress = "xyz.openbmc_project.State.Boot.Progress";
constexpr auto enable = "xyz.openbmc_project.Object.Enable";
constexpr auto bmcState = "xyz.openbmc_project.State.BMC";
constexpr auto chassisState = "xyz.openbmc_project.State.Chassis";
constexpr auto hostState = "xyz.openbmc_project.State.Host";
constexpr auto viniRecordVPD = "com.ibm.ipzvpd.VINI";
constexpr auto vsbpRecordVPD = "com.ibm.ipzvpd.VSBP";
constexpr auto locCode = "xyz.openbmc_project.Inventory.Decorator.LocationCode";
constexpr auto compatible =
    "xyz.openbmc_project.Inventory.Decorator.Compatible";
constexpr auto vpdManager = "com.ibm.VPD.Manager";
constexpr auto ledGroup = "xyz.openbmc_project.Led.Group";
constexpr auto operationalStatus =
    "xyz.openbmc_project.State.Decorator.OperationalStatus";
constexpr auto logSetting = "xyz.openbmc_project.Logging.Settings";
constexpr auto associationDef = "xyz.openbmc_project.Association.Definitions";
constexpr auto hwIsolationEntry = "xyz.openbmc_project.HardwareIsolation.Entry";
constexpr auto association = "xyz.openbmc_project.Association";
constexpr auto biosConfigMgr = "xyz.openbmc_project.BIOSConfig.Manager";
constexpr auto bootRawProgress = "xyz.openbmc_project.State.Boot.Raw";
constexpr auto invItem = "xyz.openbmc_project.Inventory.Item";
constexpr auto invFan = "xyz.openbmc_project.Inventory.Item.Fan";
constexpr auto invPowerSupply =
    "xyz.openbmc_project.Inventory.Item.PowerSupply";
constexpr auto inventoryManager = "xyz.openbmc_project.Inventory.Manager";
constexpr auto systemdMgr = "org.freedesktop.systemd1.Manager";
} // namespace interface

using namespace sdbusplus::server::xyz::openbmc_project::state::boot;
using namespace sdbusplus::server::xyz::openbmc_project::state;
namespace match_rules = sdbusplus::bus::match::rules;

const DBusInterfaceList hotplugInterfaces{interface::invFan,
                                          interface::invPowerSupply};
static constexpr auto PDBG_DTB_PATH =
    "/var/lib/phosphor-software-manager/hostfw/running/DEVTREE";

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

DataInterface::DataInterface(sdbusplus::bus_t& bus) :
    _bus(bus), _systemdSlot(nullptr)
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
            if (std::get<bool>(value) != this->_sendPELsToHost)
            {
                lg2::info("The send PELs to host setting changed to {VAL}",
                          "VAL", std::get<bool>(value));
            }
            this->_sendPELsToHost = std::get<bool>(value);
        }));

    // Watch the BMCState property
    _properties.emplace_back(std::make_unique<PropertyWatcher<DataInterface>>(
        bus, object_path::bmcState, interface::bmcState, "CurrentBMCState",
        *this, [this](const auto& value) {
            const auto& state = std::get<std::string>(value);
            this->_bmcState = state;

            // Wait for BMC ready to start watching for
            // plugs so things calm down first.
            if (BMC::convertBMCStateFromString(state) == BMC::BMCState::Ready)
            {
                startFruPlugWatch();
            }
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

    // Watch the BaseBIOSTable property for the hmc managed attribute
    _properties.emplace_back(std::make_unique<PropertyWatcher<DataInterface>>(
        bus, object_path::biosConfigMgr, interface::biosConfigMgr,
        "BaseBIOSTable", service_name::biosConfigMgr, *this,
        [this](const auto& value) {
            const auto& attributes = std::get<BiosAttributes>(value);

            auto it = attributes.find("pvm_hmc_managed");
            if (it != attributes.end())
            {
                const auto& currentValVariant = std::get<5>(it->second);
                auto currentVal = std::get_if<std::string>(&currentValVariant);
                if (currentVal)
                {
                    this->_hmcManaged =
                        (*currentVal == "Enabled") ? true : false;
                }
            }
        }));

    if (isPHALDevTreeExist())
    {
#ifdef PEL_ENABLE_PHAL
        initPHAL();
#endif
    }
    else
    {
        // Watch the "openpower-update-bios-attr-table" service to init
        // PHAL libraries
        subscribeToSystemdSignals();
    }
}

DBusPropertyMap DataInterface::getAllProperties(
    const std::string& service, const std::string& objectPath,
    const std::string& interface) const
{
    DBusPropertyMap properties;

    auto method = _bus.new_method_call(service.c_str(), objectPath.c_str(),
                                       interface::dbusProperty, "GetAll");
    method.append(interface);
    auto reply = _bus.call(method, dbusTimeout);

    reply.read(properties);

    return properties;
}

void DataInterface::getProperty(
    const std::string& service, const std::string& objectPath,
    const std::string& interface, const std::string& property,
    DBusValue& value) const
{
    auto method = _bus.new_method_call(service.c_str(), objectPath.c_str(),
                                       interface::dbusProperty, "Get");
    method.append(interface, property);
    auto reply = _bus.call(method, dbusTimeout);

    reply.read(value);
}

DBusPathList DataInterface::getPaths(const DBusInterfaceList& interfaces) const
{
    auto method = _bus.new_method_call(
        service_name::objectMapper, object_path::objectMapper,
        interface::objectMapper, "GetSubTreePaths");

    method.append(std::string{"/"}, 0, interfaces);

    auto reply = _bus.call(method, dbusTimeout);

    auto paths = reply.unpack<DBusPathList>();

    return paths;
}

DBusSubTree DataInterface::getSubTree(const DBusInterfaceList& interfaces) const
{
    auto method = _bus.new_method_call(service_name::objectMapper,
                                       object_path::objectMapper,
                                       interface::objectMapper, "GetSubTree");
    method.append(std::string{"/"}, 0, interfaces);
    auto reply = _bus.call(method, dbusTimeout);

    return reply.unpack<DBusSubTree>();
}

DBusService DataInterface::getService(const std::string& objectPath,
                                      const std::string& interface) const
{
    auto method = _bus.new_method_call(service_name::objectMapper,
                                       object_path::objectMapper,
                                       interface::objectMapper, "GetObject");

    method.append(objectPath, std::vector<std::string>({interface}));

    auto reply = _bus.call(method, dbusTimeout);

    auto response = reply.unpack<std::map<DBusService, DBusInterfaceList>>();

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
        lg2::warning("Failed reading Model property from "
                     "interface: {IFACE} exception: {ERROR}",
                     "IFACE", interface::invAsset, "ERROR", e);
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
        lg2::warning("Failed reading SerialNumber property from "
                     "interface: {IFACE} exception: {ERROR}",
                     "IFACE", interface::invAsset, "ERROR", e);
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
        lg2::warning("Failed reading Motherboard CCIN property from "
                     "interface: {IFACE} exception: {ERROR}",
                     "IFACE", interface::viniRecordVPD, "ERROR", e);
    }

    return ccin;
}

std::vector<uint8_t> DataInterface::getSystemIMKeyword() const
{
    static std::vector<uint8_t> systemIM;

    if (!systemIM.empty())
    {
        return systemIM;
    }

    try
    {
        auto subtree = getSubTree({interface::vsbpRecordVPD});

        if (subtree.empty())
        {
            lg2::warning("No VSBP VPD interface found");
            return systemIM;
        }

        DBusValue imValue;
        const auto& path = subtree.begin()->first;
        const auto& interfaceMap = subtree.begin()->second;
        const auto& service = interfaceMap.begin()->first;
        getProperty(service, path, interface::vsbpRecordVPD, "IM", imValue);

        systemIM = std::get<std::vector<uint8_t>>(imValue);
    }
    catch (const std::exception& e)
    {
        lg2::warning("Failed reading System IM property from "
                     "interface: {IFACE} exception: {ERROR}",
                     "IFACE", interface::vsbpRecordVPD, "ERROR", e);
    }

    return systemIM;
}

void DataInterface::getHWCalloutFields(
    const std::string& inventoryPath, std::string& fruPartNumber,
    std::string& ccin, std::string& serialNumber) const
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

std::string DataInterface::getLocationCode(
    const std::string& inventoryPath) const
{
    auto service = getService(inventoryPath, interface::locCode);

    DBusValue locCode;
    getProperty(service, inventoryPath, interface::locCode, "LocationCode",
                locCode);

    return std::get<std::string>(locCode);
}

std::string DataInterface::addLocationCodePrefix(
    const std::string& locationCode)
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

    auto reply = _bus.call(method, dbusTimeout);

    auto expandedLocationCode = reply.unpack<std::string>();

    if (!connectorLoc.empty())
    {
        expandedLocationCode += connectorLoc;
    }

    return expandedLocationCode;
}

std::vector<std::string> DataInterface::getInventoryFromLocCode(
    const std::string& locationCode, uint16_t node, bool expanded) const
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

    auto reply = _bus.call(method, dbusTimeout);

    auto entries = reply.unpack<std::vector<sdbusplus::message::object_path>>();

    std::vector<std::string> paths;

    // Note: The D-Bus method will fail if nothing found.
    std::for_each(entries.begin(), entries.end(),
                  [&paths](const auto& path) { paths.push_back(path); });

    return paths;
}

void DataInterface::assertLEDGroup(const std::string& ledGroup,
                                   bool value) const
{
    DBusValue variant = value;
    auto method =
        _bus.new_method_call(service_name::ledGroupManager, ledGroup.c_str(),
                             interface::dbusProperty, "Set");
    method.append(interface::ledGroup, "Asserted", variant);
    _bus.call(method, dbusTimeout);
}

void DataInterface::setFunctional(const std::string& objectPath,
                                  bool value) const
{
    DBusPropertyMap prop{{"Functional", value}};
    DBusInterfaceMap iface{{interface::operationalStatus, prop}};

    // PIM takes a relative path like /system/chassis so remove
    // /xyz/openbmc_project/inventory if present.
    std::string path{objectPath};
    if (path.starts_with(object_path::baseInv))
    {
        path = objectPath.substr(strlen(object_path::baseInv));
    }
    DBusObjectMap object{{path, iface}};

    auto method = _bus.new_method_call(service_name::inventoryManager,
                                       object_path::baseInv,
                                       interface::inventoryManager, "Notify");
    method.append(std::move(object));
    _bus.call(method, dbusTimeout);
}

using AssociationTuple = std::tuple<std::string, std::string, std::string>;
using AssociationsProperty = std::vector<AssociationTuple>;

void DataInterface::setCriticalAssociation(const std::string& objectPath) const
{
    DBusValue getAssociationValue;

    auto service = getService(objectPath, interface::associationDef);

    getProperty(service, objectPath, interface::associationDef, "Associations",
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

        method.append(interface::associationDef, "Associations",
                      setAssociationValue);
        _bus.call(method, dbusTimeout);
    }
}

std::vector<std::string> DataInterface::getSystemNames() const
{
    auto subtree = getSubTree({interface::compatible});

    if (subtree.empty())
    {
        throw std::runtime_error("Compatible interface not on D-Bus");
    }

    for (const auto& [path, interfaceMap] : subtree)
    {
        auto iface = interfaceMap.find(service_name::entityManager);
        if (iface == interfaceMap.end())
        {
            continue;
        }

        DBusValue names;

        getProperty(iface->first, path, interface::compatible, "Names", names);

        return std::get<std::vector<std::string>>(names);
    }

    throw std::runtime_error("EM Compatible interface not on D-Bus");
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
        lg2::warning("Failed reading QuiesceOnHwError property from "
                     "interface: {IFACE} exception: {ERROR}",
                     "IFACE", interface::logSetting, "ERROR", e);
    }

    return ret;
}

#ifdef PEL_ENABLE_PHAL
void DataInterface::createGuardRecord(const std::vector<uint8_t>& binPath,
                                      GardType eGardType, uint32_t plid) const
{
    try
    {
        libguard::libguard_init(false);
        libguard::create(binPath, plid, eGardType);
    }
    catch (libguard::exception::GuardException& e)
    {
        lg2::error("Exception in libguard {ERROR}", "ERROR", e);
    }
}
#endif

void DataInterface::createProgressSRC(
    const std::vector<uint8_t>& priSRC,
    const std::vector<uint8_t>& srcStruct) const
{
    DBusValue variant = std::make_tuple(priSRC, srcStruct);

    auto method = _bus.new_method_call(service_name::bootRawProgress,
                                       object_path::bootRawProgress,
                                       interface::dbusProperty, "Set");

    method.append(interface::bootRawProgress, "Value", variant);

    _bus.call(method, dbusTimeout);
}

std::vector<uint32_t> DataInterface::getLogIDWithHwIsolation() const
{
    std::vector<std::string> association = {"xyz.openbmc_project.Association"};
    std::string hwErrorLog = "/isolated_hw_errorlog";
    std::string errorLog = "/error_log";
    DBusPathList paths;
    std::vector<uint32_t> ids;

    // Get all latest mapper associations
    paths = getPaths(association);
    for (auto& path : paths)
    {
        // Look for object path with hardware isolation entry if any
        size_t pos = path.find(hwErrorLog);
        if (pos != std::string::npos)
        {
            // Get the object path
            std::string ph = path;
            ph.erase(pos, hwErrorLog.length());
            auto service = getService(ph, interface::hwIsolationEntry);
            if (!service.empty())
            {
                bool status;
                DBusValue value;

                // Read the Resolved property from object path
                getProperty(service, ph, interface::hwIsolationEntry,
                            "Resolved", value);

                status = std::get<bool>(value);

                // If the entry isn't resolved
                if (!status)
                {
                    auto assocService =
                        getService(path, interface::association);
                    if (!assocService.empty())
                    {
                        DBusValue endpoints;

                        // Read Endpoints property
                        getProperty(assocService, path, interface::association,
                                    "endpoints", endpoints);

                        auto logPath =
                            std::get<std::vector<std::string>>(endpoints);
                        if (!logPath.empty())
                        {
                            // Get OpenBMC event log Id
                            uint32_t id = stoi(logPath[0].substr(
                                logPath[0].find_last_of('/') + 1));
                            ids.push_back(id);
                        }
                    }
                }
            }
        }

        // Look for object path with error_log entry if any
        pos = path.find(errorLog);
        if (pos != std::string::npos)
        {
            auto service = getService(path, interface::association);
            if (!service.empty())
            {
                DBusValue value;

                // Read Endpoints property
                getProperty(service, path, interface::association, "endpoints",
                            value);

                auto logPath = std::get<std::vector<std::string>>(value);
                if (!logPath.empty())
                {
                    // Get OpenBMC event log Id
                    uint32_t id = stoi(
                        logPath[0].substr(logPath[0].find_last_of('/') + 1));
                    ids.push_back(id);
                }
            }
        }
    }

    if (ids.size() > 1)
    {
        // remove duplicates to have only unique ids
        std::sort(ids.begin(), ids.end());
        ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
    }
    return ids;
}

std::vector<uint8_t> DataInterface::getRawProgressSRC(void) const
{
    using RawProgressProperty =
        std::tuple<std::vector<uint8_t>, std::vector<uint8_t>>;

    DBusValue value;
    getProperty(service_name::bootRawProgress, object_path::bootRawProgress,
                interface::bootRawProgress, "Value", value);

    const auto& rawProgress = std::get<RawProgressProperty>(value);
    return std::get<1>(rawProgress);
}

std::optional<std::vector<uint8_t>> DataInterface::getDIProperty(
    const std::string& locationCode) const
{
    std::vector<uint8_t> viniDI;

    try
    {
        // Note : The hardcoded value 0 should be changed when comes to
        // multinode system.
        auto objectPath = getInventoryFromLocCode(locationCode, 0, true);

        DBusValue value;
        getProperty(service_name::inventoryManager, objectPath[0],
                    interface::viniRecordVPD, "DI", value);

        viniDI = std::get<std::vector<uint8_t>>(value);
    }
    catch (const std::exception& e)
    {
        lg2::warning(
            "Failed reading DI property for the location code : {LOC_CODE} from "
            "interface: {IFACE} exception: {ERROR}",
            "LOC_CODE", locationCode, "IFACE", interface::viniRecordVPD,
            "ERROR", e);
        return std::nullopt;
    }

    return viniDI;
}

std::optional<bool> DataInterfaceBase::isDIMMLocCode(
    const std::string& locCode) const
{
    if (_locationCache.contains(locCode))
    {
        return _locationCache.at(locCode);
    }
    else
    {
        return std::nullopt;
    }
}

void DataInterfaceBase::addDIMMLocCode(const std::string& locCode,
                                       bool isFRUDIMM)
{
    _locationCache.insert({locCode, isFRUDIMM});
}

bool DataInterfaceBase::isDIMM(const std::string& locCode)
{
    auto isDIMMType = isDIMMLocCode(locCode);
    if (isDIMMType.has_value())
    {
        return isDIMMType.value();
    }
#ifndef PEL_ENABLE_PHAL
    return false;
#else
    else
    {
        // Invoke pHAL API inorder to fetch the FRU Type
        auto fruType = openpower::phal::pdbg::getFRUType(locCode);
        bool isDIMMFRU{false};
        if (fruType.has_value())
        {
            if (fruType.value() == ENUM_ATTR_TYPE_DIMM)
            {
                isDIMMFRU = true;
            }
            addDIMMLocCode(locCode, isDIMMFRU);
        }
        return isDIMMFRU;
    }
#endif
}

DBusPathList DataInterface::getAssociatedPaths(
    const DBusPath& associatedPath, const DBusPath& subtree, int32_t depth,
    const DBusInterfaceList& interfaces) const
{
    DBusPathList paths;
    try
    {
        auto method = _bus.new_method_call(
            service_name::objectMapper, object_path::objectMapper,
            interface::objectMapper, "GetAssociatedSubTreePaths");
        method.append(sdbusplus::message::object_path(associatedPath),
                      sdbusplus::message::object_path(subtree), depth,
                      interfaces);

        auto reply = _bus.call(method, dbusTimeout);
        reply.read(paths);
    }
    catch (const std::exception& e)
    {
        std::string ifaces(
            std::ranges::fold_left_first(
                interfaces,
                [](std::string ifaces, const std::string& iface) {
                    return ifaces + ", " + iface;
                })
                .value_or(""));

        lg2::error("Failed getting associated paths: {ERROR}. "
                   "AssociatedPath: {ASSOIC_PATH} Subtree: {SUBTREE} "
                   "Interfaces: {IFACES}",
                   "ERROR", e, "ASSOIC_PATH", associatedPath, "SUBTREE",
                   subtree, "IFACES", ifaces);
    }
    return paths;
}

void DataInterface::startFruPlugWatch()
{
    // Add a watch on inventory InterfacesAdded and then find all
    // existing hotpluggable interfaces and add propertiesChanged
    // watches on them.

    _invIaMatch = std::make_unique<sdbusplus::bus::match_t>(
        _bus, match_rules::interfacesAdded(object_path::baseInv),
        std::bind(&DataInterface::inventoryIfaceAdded, this,
                  std::placeholders::_1));
    try
    {
        auto paths = getPaths(hotplugInterfaces);

        _invPresentMatches.clear();

        std::for_each(paths.begin(), paths.end(),
                      [this](const auto& path) { addHotplugWatch(path); });
    }
    catch (const sdbusplus::exception_t& e)
    {
        lg2::warning("Failed getting FRU paths to watch: {ERROR}", "ERROR", e);
    }
}

void DataInterface::addHotplugWatch(const std::string& path)
{
    if (!_invPresentMatches.contains(path))
    {
        _invPresentMatches.emplace(
            path,
            std::make_unique<sdbusplus::bus::match_t>(
                _bus, match_rules::propertiesChanged(path, interface::invItem),
                std::bind(&DataInterface::presenceChanged, this,
                          std::placeholders::_1)));
    }
}

void DataInterface::inventoryIfaceAdded(sdbusplus::message_t& msg)
{
    sdbusplus::message::object_path path;
    DBusInterfaceMap interfaces;

    msg.read(path, interfaces);

    // Check if any of the new interfaces are for hot pluggable FRUs.
    if (std::find_if(interfaces.begin(), interfaces.end(),
                     [](const auto& interfacePair) {
                         return std::find(hotplugInterfaces.begin(),
                                          hotplugInterfaces.end(),
                                          interfacePair.first) !=
                                hotplugInterfaces.end();
                     }) == interfaces.end())
    {
        return;
    }

    addHotplugWatch(path.str);

    // If an Inventory.Item interface was also added, check presence now.

    // Notes:
    // * This assumes the Inv.Item and Inv.Fan/PS are added together which
    //   is currently the case.
    // * If the code ever switches to something without a Present
    //   property, then the IA signal itself would probably indicate presence.

    auto itemIt = interfaces.find(interface::invItem);
    if (itemIt != interfaces.end())
    {
        notifyPresenceSubscribers(path.str, itemIt->second);
    }
}

void DataInterface::presenceChanged(sdbusplus::message_t& msg)
{
    DBusInterface interface;
    DBusPropertyMap properties;

    msg.read(interface, properties);
    if (interface != interface::invItem)
    {
        return;
    }

    std::string path = msg.get_path();
    notifyPresenceSubscribers(path, properties);
}

void DataInterface::notifyPresenceSubscribers(const std::string& path,
                                              const DBusPropertyMap& properties)
{
    auto prop = properties.find("Present");
    if ((prop == properties.end()) || (!std::get<bool>(prop->second)))
    {
        return;
    }

    std::string locCode;

    try
    {
        auto service = getService(path, interface::locCode);

        // If the hotplugged FRU is hosted by PLDM, then it is
        // in an IO expansion drawer and we don't care about it.
        if (service == service_name::pldm)
        {
            return;
        }

        locCode = getLocationCode(path);
    }
    catch (const sdbusplus::exception_t& e)
    {
        lg2::debug("Could not get location code for {PATH}: {ERROR}", "PATH",
                   path, "ERROR", e);
        return;
    }

    lg2::debug("Detected FRU {PATH} ({LOC}) present ", "PATH", path, "LOC",
               locCode);

    // Tell the subscribers.
    setFruPresent(locCode);
}

bool DataInterface::isPHALDevTreeExist() const
{
    try
    {
        if (std::filesystem::exists(PDBG_DTB_PATH))
        {
            return true;
        }
    }
    catch (const std::exception& e)
    {
        lg2::error("Failed to check device tree {PHAL_DEVTREE_PATH} existence, "
                   "{ERROR}",
                   "PHAL_DEVTREE_PATH", PDBG_DTB_PATH, "ERROR", e);
    }
    return false;
}

#ifdef PEL_ENABLE_PHAL
void DataInterface::initPHAL()
{
    if (setenv("PDBG_DTB", PDBG_DTB_PATH, 1))
    {
        // Log message and continue,
        // This is to help continue creating PEL in raw format.
        lg2::error("Failed to set PDBG_DTB: ({ERRNO})", "ERRNO",
                   strerror(errno));
    }

    if (!pdbg_targets_init(NULL))
    {
        lg2::error("pdbg_targets_init failed");
        return;
    }

    if (libekb_init())
    {
        lg2::error("libekb_init failed, skipping ffdc processing");
        return;
    }
}
#endif

void DataInterface::subscribeToSystemdSignals()
{
    try
    {
        auto method =
            _bus.new_method_call(service_name::systemd, object_path::systemd,
                                 interface::systemdMgr, "Subscribe");
        _systemdSlot = method.call_async([this](sdbusplus::message_t&& msg) {
            // Initializing with nullptr to indicate that it is not subscribed
            // to any signal.
            this->_systemdSlot = sdbusplus::slot_t(nullptr);
            if (msg.is_method_error())
            {
                auto* error = msg.get_error();
                lg2::error("Failed to subscribe JobRemoved systemd signal, "
                           "errorName: {ERR_NAME}, errorMsg: {ERR_MSG} ",
                           "ERR_NAME", error->name, "ERR_MSG", error->message);
                return;
            }

            namespace sdbusRule = sdbusplus::bus::match::rules;
            this->_systemdMatch =
                std::make_unique<decltype(this->_systemdMatch)::element_type>(
                    this->_bus,
                    sdbusRule::type::signal() +
                        sdbusRule::member("JobRemoved") +
                        sdbusRule::path(object_path::systemd) +
                        sdbusRule::interface(interface::systemdMgr),
                    [this](sdbusplus::message_t& msg) {
                        uint32_t jobID;
                        sdbusplus::message::object_path jobObjPath;
                        std::string jobUnitName, jobUnitResult;

                        msg.read(jobID, jobObjPath, jobUnitName, jobUnitResult);
                        if ((jobUnitName == "obmc-recover-pnor.service") &&
                            (jobUnitResult == "done"))
                        {
#ifdef PEL_ENABLE_PHAL
                            this->initPHAL();
#endif
                            // Invoke unsubscribe method to stop monitoring for
                            // JobRemoved signals.
                            this->unsubscribeFromSystemdSignals();
                        }
                    });
        });
    }
    catch (const sdbusplus::exception_t& e)
    {
        lg2::error(
            "Exception occured while handling JobRemoved systemd signal, "
            "exception: {ERROR}",
            "ERROR", e);
    }
}

void DataInterface::unsubscribeFromSystemdSignals()
{
    try
    {
        auto method =
            _bus.new_method_call(service_name::systemd, object_path::systemd,
                                 interface::systemdMgr, "Unsubscribe");
        _systemdSlot = method.call_async([this](sdbusplus::message_t&& msg) {
            // Unsubscribing the _systemdSlot from the subscribed signal
            this->_systemdSlot = sdbusplus::slot_t(nullptr);
            if (msg.is_method_error())
            {
                auto* error = msg.get_error();
                lg2::error(
                    "Failed to unsubscribe from JobRemoved systemd signal, "
                    "errorName: {ERR_NAME}, errorMsg: {ERR_MSG} ",
                    "ERR_NAME", error->name, "ERR_MSG", error->message);
                return;
            }
            // Reset _systemdMatch to avoid reception of further JobRemoved
            // signals
            this->_systemdMatch.reset();
        });
    }
    catch (const sdbusplus::exception_t& e)
    {
        lg2::error(
            "Exception occured while unsubscribing from JobRemoved systemd signal, "
            "exception: {ERROR}",
            "ERROR", e);
    }
}

} // namespace pels
} // namespace openpower
