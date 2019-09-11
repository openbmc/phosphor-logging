#include "data_interface.hpp"

#include <phosphor-logging/log.hpp>

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
} // namespace object_path

namespace interface
{
constexpr auto dbusProperty = "org.freedesktop.DBus.Properties";
constexpr auto objectMapper = "xyz.openbmc_project.ObjectMapper";
constexpr auto invAsset = "xyz.openbmc_project.Inventory.Decorator.Asset";
} // namespace interface

using namespace phosphor::logging;

DataInterface::DataInterface(sdbusplus::bus::bus& bus) : _bus(bus)
{
    readMTMS();
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
} // namespace pels
} // namespace openpower
