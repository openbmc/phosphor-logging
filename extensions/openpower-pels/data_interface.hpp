#pragma once

#include "dbus_types.hpp"
#include "dbus_watcher.hpp"

#include <filesystem>
#include <phosphor-logging/log.hpp>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/bus/match.hpp>

namespace openpower
{
namespace pels
{

/**
 * @class DataInterface
 *
 * A base class for gathering data about the system for use
 * in PELs. Implemented this way to facilitate mocking.
 */
class DataInterfaceBase
{
  public:
    DataInterfaceBase() = default;
    virtual ~DataInterfaceBase() = default;
    DataInterfaceBase(const DataInterfaceBase&) = default;
    DataInterfaceBase& operator=(const DataInterfaceBase&) = default;
    DataInterfaceBase(DataInterfaceBase&&) = default;
    DataInterfaceBase& operator=(DataInterfaceBase&&) = default;

    /**
     * @brief Returns the machine Type/Model
     *
     * @return string - The machine Type/Model string
     */
    virtual std::string getMachineTypeModel() const = 0;

    /**
     * @brief Returns the machine serial number
     *
     * @return string - The machine serial number
     */
    virtual std::string getMachineSerialNumber() const = 0;

    /**
     * @brief Says if the system is managed by a hardware
     *        management console.
     * @return bool - If the system is HMC managed
     */
    virtual bool isHMCManaged() const
    {
        return _hmcManaged;
    }

    /**
     * @brief Says if the host is up and running
     *
     * @return bool - If the host is running
     */
    virtual bool isHostUp() const
    {
        return _hostUp;
    }

    using HostStateChangeFunc = std::function<void(bool)>;

    /**
     * @brief Register a callback function that will get
     *        called on all host on/off transitions.
     *
     * The void(bool) function will get passed the new
     * value of the host state.
     *
     * @param[in] name - The subscription name
     * @param[in] func - The function to run
     */
    void subscribeToHostStateChange(const std::string& name,
                                    HostStateChangeFunc func)
    {
        _hostChangeCallbacks[name] = func;
    }

    /**
     * @brief Unsubscribe from host state changes.
     *
     * @param[in] name - The subscription name
     */
    void unsubscribeFromHostStateChange(const std::string& name)
    {
        _hostChangeCallbacks.erase(name);
    }

    /**
     * @brief Returns the BMC firmware version
     *
     * @return std::string - The BMC version
     */
    virtual std::string getBMCFWVersion() const
    {
        return _bmcFWVersion;
    }

    /**
     * @brief Returns the server firmware version
     *
     * @return std::string - The server firmware version
     */
    virtual std::string getServerFWVersion() const
    {
        return _serverFWVersion;
    }

    /**
     * @brief Returns the BMC FW version ID
     *
     * @return std::string - The BMC FW version ID
     */
    virtual std::string getBMCFWVersionID() const
    {
        return _bmcFWVersionID;
    }

    /**
     * @brief Returns the process name given its PID.
     *
     * @param[in] pid - The PID value as a string
     *
     * @return std::optional<std::string> - The name, or std::nullopt
     */
    std::optional<std::string> getProcessName(const std::string& pid) const
    {
        namespace fs = std::filesystem;

        fs::path path{"/proc"};
        path /= fs::path{pid} / "exe";

        if (fs::exists(path))
        {
            return fs::read_symlink(path);
        }

        return std::nullopt;
    }

    /**
     * @brief Returns the 'send event logs to host' setting.
     *
     * @return bool - If sending PELs to the host is enabled.
     */
    virtual bool getHostPELEnablement() const
    {
        return _sendPELsToHost;
    }

    /**
     * @brief Returns the BMC state
     *
     * @return std::string - The BMC state property value
     */
    virtual std::string getBMCState() const
    {
        return _bmcState;
    }

    /**
     * @brief Returns the Chassis state
     *
     * @return std::string - The chassis state property value
     */
    virtual std::string getChassisState() const
    {
        return _chassisState;
    }

    /**
     * @brief Returns the chassis requested power
     *        transition value.
     *
     * @return std::string - The chassis transition property
     */
    virtual std::string getChassisTransition() const
    {
        return _chassisTransition;
    }

    /**
     * @brief Returns the Host state
     *
     * @return std::string - The Host state property value
     */
    virtual std::string getHostState() const
    {
        return _hostState;
    }

    /**
     * @brief Returns the motherboard CCIN
     *
     * @return std::string The motherboard CCIN
     */
    virtual std::string getMotherboardCCIN() const = 0;

    /**
     * @brief Returns the system IM
     *
     * @return std::string The system IM
     */
    virtual std::vector<uint8_t> getSystemIMKeyword() const = 0;

    /**
     * @brief Get the fields from the inventory necessary for doing
     *        a callout on an inventory path.
     *
     * @param[in] inventoryPath - The item to get the data for
     * @param[out] fruPartNumber - Filled in with the VINI/FN keyword
     * @param[out] ccin - Filled in with the VINI/CC keyword
     * @param[out] serialNumber - Filled in with the VINI/SN keyword
     */
    virtual void getHWCalloutFields(const std::string& inventoryPath,
                                    std::string& fruPartNumber,
                                    std::string& ccin,
                                    std::string& serialNumber) const = 0;

    /**
     * @brief Get the location code for an inventory item.
     *
     * @param[in] inventoryPath - The item to get the data for
     *
     * @return std::string - The location code
     */
    virtual std::string
        getLocationCode(const std::string& inventoryPath) const = 0;

    /**
     * @brief Get the list of system type names the system is called.
     *
     * @return std::vector<std::string> - The list of names
     */
    virtual std::vector<std::string> getSystemNames() const = 0;

    /**
     * @brief Fills in the placeholder 'Ufcs' in the passed in location
     *        code with the machine feature code and serial number, which
     *        is needed to create a valid location code.
     *
     * @param[in] locationCode - Location code value starting with Ufcs-, and
     *                           if that isn't present it will be added first.
     *
     * @param[in] node - The node number the location is on.
     *
     * @return std::string - The expanded location code
     */
    virtual std::string expandLocationCode(const std::string& locationCode,
                                           uint16_t node) const = 0;

    /**
     * @brief Returns the inventory path for the FRU that the location
     *        code represents.
     *
     * @param[in] locationCode - If an expanded location code, then the
     *                           full location code.
     *                           If not expanded, a location code value
     *                           starting with Ufcs-, and if that isn't
     *                           present it will be added first.
     *
     * @param[in] node - The node number the location is on.  Ignored if the
     *                   expanded location code is passed in.
     *
     * @param[in] expanded - If the location code already has the relevent
     *                       VPD fields embedded in it.
     *
     * @return std::string - The inventory D-Bus object
     */
    virtual std::string getInventoryFromLocCode(const std::string& LocationCode,
                                                uint16_t node,
                                                bool expanded) const = 0;

    /**
     * @brief Sets the Asserted property on the LED group passed in.
     *
     * @param[in] ledGroup - The LED group D-Bus path
     * @param[in] value - The value to set it to
     */
    virtual void assertLEDGroup(const std::string& ledGroup,
                                bool value) const = 0;

    /**
     * @brief Sets the Functional property on the OperationalStatus
     *        interface on a D-Bus object.
     *
     * @param[in] objectPath - The D-Bus object path
     * @param[in] functional - The value
     */
    virtual void setFunctional(const std::string& objectPath,
                               bool functional) const = 0;

    /**
     * @brief Sets the critical association on the D-Bus object.
     *
     * @param[in] objectPath - The D-Bus object path
     */
    virtual void
        setCriticalAssociation(const std::string& objectPath) const = 0;

    /**
     * @brief Returns the manufacturing QuiesceOnError property
     *
     * @return bool - Manufacturing QuiesceOnError property
     */
    virtual bool getQuiesceOnError() const = 0;

    /**
     * @brief Split location code into base and connector segments
     *
     * A location code that ends in '-Tx', where 'x' is a number,
     * represents a connector, such as a USB cable connector.
     *
     * This function splits the passed in location code into a
     * base and connector segment.  e.g.:
     *   P0-T1 -> ['P0', '-T1']
     *   P0 -> ['P0', '']
     *
     * @param[in] locationCode - location code to split
     * @return pair<string, string> - The base and connector segments
     */
    static std::pair<std::string, std::string>
        extractConnectorFromLocCode(const std::string& locationCode);

  protected:
    /**
     * @brief Sets the host on/off state and runs any
     *        callback functions (if there was a change).
     */
    void setHostUp(bool hostUp)
    {
        if (_hostUp != hostUp)
        {
            _hostUp = hostUp;

            for (auto& [name, func] : _hostChangeCallbacks)
            {
                try
                {
                    func(_hostUp);
                }
                catch (std::exception& e)
                {
                    using namespace phosphor::logging;
                    log<level::ERR>("A host state change callback threw "
                                    "an exception");
                }
            }
        }
    }

    /**
     * @brief The hardware management console status.  Always kept
     *        up to date.
     */
    bool _hmcManaged = false;

    /**
     * @brief The host up status.  Always kept up to date.
     */
    bool _hostUp = false;

    /**
     * @brief The map of host state change subscriber
     *        names to callback functions.
     */
    std::map<std::string, HostStateChangeFunc> _hostChangeCallbacks;

    /**
     * @brief The BMC firmware version string
     */
    std::string _bmcFWVersion;

    /**
     * @brief The server firmware version string
     */
    std::string _serverFWVersion;

    /**
     * @brief The BMC firmware version ID string
     */
    std::string _bmcFWVersionID;

    /**
     * @brief If sending PELs is enabled.
     *
     * This is usually set to false in manufacturing test.
     */
    bool _sendPELsToHost = true;

    /**
     * @brief The BMC state property
     */
    std::string _bmcState;

    /**
     * @brief The Chassis current power state property
     */
    std::string _chassisState;

    /**
     * @brief The Chassis requested power transition property
     */
    std::string _chassisTransition;

    /**
     * @brief The host state property
     */
    std::string _hostState;
};

/**
 * @class DataInterface
 *
 * Concrete implementation of DataInterfaceBase.
 */
class DataInterface : public DataInterfaceBase
{
  public:
    DataInterface() = delete;
    ~DataInterface() = default;
    DataInterface(const DataInterface&) = default;
    DataInterface& operator=(const DataInterface&) = default;
    DataInterface(DataInterface&&) = default;
    DataInterface& operator=(DataInterface&&) = default;

    /**
     * @brief Constructor
     *
     * @param[in] bus - The sdbusplus bus object
     */
    explicit DataInterface(sdbusplus::bus::bus& bus);

    /**
     * @brief Finds the D-Bus service name that hosts the
     *        passed in path and interface.
     *
     * @param[in] objectPath - The D-Bus object path
     * @param[in] interface - The D-Bus interface
     */
    DBusService getService(const std::string& objectPath,
                           const std::string& interface) const;

    /**
     * @brief Wrapper for the 'GetAll' properties method call
     *
     * @param[in] service - The D-Bus service to call it on
     * @param[in] objectPath - The D-Bus object path
     * @param[in] interface - The interface to get the props on
     *
     * @return DBusPropertyMap - The property results
     */
    DBusPropertyMap getAllProperties(const std::string& service,
                                     const std::string& objectPath,
                                     const std::string& interface) const;
    /**
     * @brief Wrapper for the 'Get' properties method call
     *
     * @param[in] service - The D-Bus service to call it on
     * @param[in] objectPath - The D-Bus object path
     * @param[in] interface - The interface to get the property on
     * @param[in] property - The property name
     * @param[out] value - Filled in with the property value.
     */
    void getProperty(const std::string& service, const std::string& objectPath,
                     const std::string& interface, const std::string& property,
                     DBusValue& value) const;
    /**
     * @brief Returns the machine Type/Model
     *
     * @return string - The machine Type/Model string
     */
    std::string getMachineTypeModel() const override;

    /**
     * @brief Returns the machine serial number
     *
     * @return string - The machine serial number
     */
    std::string getMachineSerialNumber() const override;

    /**
     * @brief Returns the motherboard CCIN
     *
     * @return std::string The motherboard CCIN
     */
    std::string getMotherboardCCIN() const override;

    /**
     * @brief Returns the system IM
     *
     * @return std::vector The system IM keyword in 4 byte vector
     */
    std::vector<uint8_t> getSystemIMKeyword() const override;

    /**
     * @brief Get the fields from the inventory necessary for doing
     *        a callout on an inventory path.
     *
     * @param[in] inventoryPath - The item to get the data for
     * @param[out] fruPartNumber - Filled in with the VINI/FN keyword
     * @param[out] ccin - Filled in with the VINI/CC keyword
     * @param[out] serialNumber - Filled in with the VINI/SN keyword
     */
    void getHWCalloutFields(const std::string& inventoryPath,
                            std::string& fruPartNumber, std::string& ccin,
                            std::string& serialNumber) const override;

    /**
     * @brief Get the location code for an inventory item.
     *
     * Throws an exception if the inventory item doesn't have the
     * location code interface.
     *
     * @param[in] inventoryPath - The item to get the data for
     *
     * @return std::string - The location code
     */
    std::string
        getLocationCode(const std::string& inventoryPath) const override;

    /**
     * @brief Get the list of system type names the system is called.
     *
     * @return std::vector<std::string> - The list of names
     */
    std::vector<std::string> getSystemNames() const override;

    /**
     * @brief Fills in the placeholder 'Ufcs' in the passed in location
     *        code with the machine feature code and serial number, which
     *        is needed to create a valid location code.
     *
     * @param[in] locationCode - Location code value starting with Ufcs-, and
     *                           if that isn't present it will be added first.
     *
     * @param[in] node - The node number the location is one.
     *
     * @return std::string - The expanded location code
     */
    std::string expandLocationCode(const std::string& locationCode,
                                   uint16_t node) const override;

    /**
     * @brief Returns the inventory path for the FRU that the location
     *        code represents.
     *
     * @param[in] locationCode - If an expanded location code, then the
     *                           full location code.
     *                           If not expanded, a location code value
     *                           starting with Ufcs-, and if that isn't
     *                           present it will be added first.
     *
     * @param[in] node - The node number the location is on.  Ignored if the
     *                   expanded location code is passed in.
     *
     * @param[in] expanded - If the location code already has the relevent
     *                       VPD fields embedded in it.
     *
     * @return std::string - The inventory D-Bus object
     */
    std::string getInventoryFromLocCode(const std::string& locationCode,
                                        uint16_t node,
                                        bool expanded) const override;

    /**
     * @brief Sets the Asserted property on the LED group passed in.
     *
     * @param[in] ledGroup - The LED group D-Bus path
     * @param[in] value - The value to set it to
     */
    void assertLEDGroup(const std::string& ledGroup, bool value) const override;

    /**
     * @brief Sets the Functional property on the OperationalStatus
     *        interface on a D-Bus object.
     *
     * @param[in] objectPath - The D-Bus object path
     * @param[in] functional - The value
     */
    void setFunctional(const std::string& objectPath,
                       bool functional) const override;

    /**
     * @brief Sets the critical association on the D-Bus object.
     *
     * @param[in] objectPath - The D-Bus object path
     */
    void setCriticalAssociation(const std::string& objectPath) const override;

    /**
     * @brief Returns the manufacturing QuiesceOnError property
     *
     * @return bool - Manufacturing QuiesceOnError property
     */
    bool getQuiesceOnError() const override;

  private:
    /**
     * @brief Reads the BMC firmware version string and puts it into
     *        _bmcFWVersion.
     */
    void readBMCFWVersion();

    /**
     * @brief Reads the server firmware version string and puts it into
     *        _serverFWVersion.
     */
    void readServerFWVersion();

    /**
     * @brief Reads the BMC firmware version ID and puts it into
     *        _bmcFWVersionID.
     */
    void readBMCFWVersionID();

    /**
     * @brief Finds all D-Bus paths that contain any of the interfaces
     *        passed in, by using GetSubTreePaths.
     *
     * @param[in] interfaces - The desired interfaces
     *
     * @return The D-Bus paths.
     */
    DBusPathList getPaths(const DBusInterfaceList& interfaces) const;

    /**
     * @brief The interfacesAdded callback used on the inventory to
     *        find the D-Bus object that has the motherboard interface.
     *        When the motherboard is found, it then adds a PropertyWatcher
     *        for the motherboard CCIN.
     */
    void motherboardIfaceAdded(sdbusplus::message::message& msg);

    /**
     * @brief Adds the Ufcs- prefix to the location code passed in
     *        if necessary.
     *
     * Needed because the location codes that come back from the
     * message registry and device callout JSON don't have it.
     *
     * @param[in] - The location code without a prefix, like P1-C1
     *
     * @return std::string - The location code with the prefix
     */
    static std::string addLocationCodePrefix(const std::string& locationCode);

    /**
     * @brief The D-Bus property or interface watchers that have callbacks
     *        registered that will set members in this class when
     *        they change.
     */
    std::vector<std::unique_ptr<DBusWatcher>> _properties;

    /**
     * @brief The sdbusplus bus object for making D-Bus calls.
     */
    sdbusplus::bus::bus& _bus;

    /**
     * @brief The interfacesAdded match object used to wait for inventory
     *        interfaces to show up, so that the object with the motherboard
     *        interface can be found.  After it is found, this object is
     *        deleted.
     */
    std::unique_ptr<sdbusplus::bus::match_t> _inventoryIfacesAddedMatch;
};

} // namespace pels
} // namespace openpower
