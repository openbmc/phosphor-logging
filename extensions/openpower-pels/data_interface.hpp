#pragma once

#include <sdbusplus/bus.hpp>
#include <sdbusplus/bus/match.hpp>

namespace openpower
{
namespace pels
{

using DBusValue = sdbusplus::message::variant<std::string>;
using DBusProperty = std::string;
using DBusInterface = std::string;
using DBusService = std::string;
using DBusPath = std::string;
using DBusInterfaceList = std::vector<DBusInterface>;
using DBusPropertyMap = std::map<DBusProperty, DBusValue>;

/**
 * @class DataInterface
 *
 * An abstract interface class for gathering data about the system
 * for use in PELs. Implemented this way to facilitate mocking.
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

  protected:
    /**
     * @brief The machine type-model.  Always kept up to date
     */
    std::string _machineTypeModel;

    /**
     * @brief The machine serial number.  Always kept up to date
     */
    std::string _machineSerialNumber;
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
     * @brief Finds the D-Bus service name that hosts the
     *        passed in path and interface.
     *
     * @param[in] objectPath - The D-Bus object path
     * @param[in] interface - The D-Bus interface
     */
    DBusService getService(const std::string& objectPath,
                           const std::string& interface);
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
     * @brief The properties changed callback for the Asset iface
     *        on the system inventory object.
     *
     * @param[in] msg - The sdbusplus message of the signal
     */
    void sysAssetPropChanged(sdbusplus::message::message& msg);

    /**
     * @brief The match object for the system path's properties
     */
    std::unique_ptr<sdbusplus::bus::match_t> _sysInventoryPropMatch;

    /**
     * @brief The sdbusplus bus object for making D-Bus calls.
     */
    sdbusplus::bus::bus& _bus;
};

} // namespace pels
} // namespace openpower
