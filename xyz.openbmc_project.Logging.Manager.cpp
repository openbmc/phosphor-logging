#include <algorithm>
#include <sdbusplus/server.hpp>
#include <sdbusplus/exception.hpp>
#include <xyz/openbmc_project/Logging/Manager/server.hpp>


namespace sdbusplus
{
namespace server
{
namespace xyz
{
namespace openbmc_project
{
namespace Logging
{

Manager::Manager(bus::bus& bus, const char* path)
        : _xyz_openbmc_project_Logging_Manager_interface(
                bus, path, _interface, _vtable, this)
{
}


int Manager::_callback_Commit(
        sd_bus_message* msg, void* context, sd_bus_error* error)
{
    using sdbusplus::server::binding::details::convertForMessage;

    try
    {
        auto m = message::message(sd_bus_message_ref(msg));

        uint64_t transactionId{};
    std::string errMsg{};

        m.read(transactionId, errMsg);

        auto o = static_cast<Manager*>(context);
        o->commit(transactionId, errMsg);

        auto reply = m.new_method_return();
        // No data to append on reply.

        reply.method_return();
    }
    catch(sdbusplus::internal_exception_t& e)
    {
        sd_bus_error_set_const(error, e.name(), e.description());
        return -EINVAL;
    }

    return true;
}

namespace details
{
namespace Manager
{
static const auto _param_Commit =
        utility::tuple_to_array(message::types::type_id<
                uint64_t, std::string>());
static const auto _return_Commit =
        utility::tuple_to_array(std::make_tuple('\0'));
}
}




const vtable::vtable_t Manager::_vtable[] = {
    vtable::start(),

    vtable::method("Commit",
                   details::Manager::_param_Commit
                        .data(),
                   details::Manager::_return_Commit
                        .data(),
                   _callback_Commit),
    vtable::end()
};

} // namespace Logging
} // namespace openbmc_project
} // namespace xyz
} // namespace server
} // namespace sdbusplus

