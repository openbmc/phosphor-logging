#pragma once

#include <filesystem>
#include <phosphor-logging/log.hpp>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/bus/match.hpp>

namespace openpower
{
namespace pels
{

using DBusValue = sdbusplus::message::variant<std::string, bool>;
using DBusProperty = std::string;
using DBusInterface = std::string;
using DBusService = std::string;
using DBusPath = std::string;
using DBusInterfaceList = std::vector<DBusInterface>;
using DBusPropertyMap = std::map<DBusProperty, DBusValue>;
using DBusInterfaceMap = std::map<DBusInterface, DBusPropertyMap>;

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
    virtual std::string getMachineTypeModel() const
    {
        return _machineTypeModel;
    }

    /**
     * @brief Returns the machine serial number
     *
     * @return string - The machine serial number
     */
    virtual std::string getMachineSerialNumber() const
    {
        return _machineSerialNumber;
    }

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

    /**
     * @brief Returns the PLDM instance ID to use for PLDM commands
     *
     * The base class implementation just returns zero.
     *
     * @param[in] eid - The PLDM EID
     *
     * @return uint8_t - The instance ID
     */
    virtual uint8_t getPLDMInstanceID(uint8_t eid) const
    {
        return 0;
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
<<<<<<< HEAD
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
     * @return std::string - If sending PELs to the host if enabled.
     */
    virtual bool getHostPELEnablement() const
    {
        return _sendPELsToHost;
    }

  protected:
    /**
     * @brief Sets the host on/off state and runs any
     *        callback functions (if there was a change).
     */
    void setHostState(bool newState)
    {
        if (_hostUp != newState)
        {
            _hostUp = newState;

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
     * @brief The machine type-model.  Always kept up to date
     */
    std::string _machineTypeModel;

    /**
     * @brief The machine serial number.  Always kept up to date
     */
    std::string _machineSerialNumber;

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
     * @brief Returns the PLDM instance ID to use for PLDM commands
     *
     * @param[in] eid - The PLDM EID
     *
     * @return uint8_t - The instance ID
     */
    uint8_t getPLDMInstanceID(uint8_t eid) const override;

  private:
    /**
     * @brief Reads the machine type/model and SN from D-Bus.
     *
     * Looks for them on the 'system' inventory object, and also
     * places a properties changed watch on them to obtain any changes
     * (or read them for the first time if the inventory isn't ready
     * when this function runs.)
     */
    void readMTMS();

    /**
     * @brief Reads the host state from D-Bus.
     *
     * For host on, looks for the values of 'BootComplete' or 'Standby'
     * in the OperatingSystemState property on the
     * 'xyz.openbmc_project.State.OperatingSystem.Status' interface
     * on the '/xyz/openbmc_project/state/host0' path.
     *
     * Also adds a properties changed watch on it so the code can be
     * kept up to date on changes.
     */
    void readHostState();

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
     * @brief Reads the setting that says if sending PELs to the host is
     *        enabled.
     */
    void readHostPELEnablement();

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
                                     const std::string& interface);

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
                     DBusValue& value);

    /**
     * @brief The properties changed callback for the Asset iface
     *        on the system inventory object.
     *
     * @param[in] msg - The sdbusplus message of the signal
     */
    void sysAssetPropChanged(sdbusplus::message::message& msg);

    /**
     * @brief The interfaces added callback for the Asset iface
     *        on the system inventory object.
     *
     * @param[in] msg - The sdbusplus message of the signal
     */
    void sysAssetIfaceAdded(sdbusplus::message::message& msg);

    /**
     * @brief The properties changed callback for the OperatingSystemStatus
     *        interface on the host state object.
     *
     * @param[in] msg - The sdbusplus message of the signal
     */
    void osStatePropChanged(sdbusplus::message::message& msg);

    /**
     * @brief The interfaces added callback for the OperatingSystemStatus
     *        interface on the host state object.
     *
     * @param[in] msg - The sdbusplus message of the signal
     */
    void osStatusIfaceAdded(sdbusplus::message::message& msg);

    /**
     * @brief The properties changed callback for the Object.Enable
     *        interface on the send_event_logs_to_host object.
     *
     * @param[in] msg - The sdbusplus message of the signal
     */
    void hostPELPropChanged(sdbusplus::message::message& msg);

    /**
     * @brief The interfaces added callback for the Object.Enable
     *        interface on the send_event_logs_to_host object.
     *
     * @param[in] msg - The sdbusplus message of the signal
     */
    void hostPELIfaceAdded(sdbusplus::message::message& msg);

    /**
     * @brief The matches for the propertiesChanged and interfacesAdded
     *        signals.
     */
    std::vector<sdbusplus::bus::match_t> _matches;

    /**
     * @brief The sdbusplus bus object for making D-Bus calls.
     */
    sdbusplus::bus::bus& _bus;
};

} // namespace pels
} // namespace openpower
