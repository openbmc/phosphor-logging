#include <sdbusplus/server.hpp>
#include <xyz/openbmc_project/Logging/Manager/server.hpp>
    
namespace sdbusplus
{
namespace server
{
namespace xyz
{
namespace openbmc_project
{
namespace logging
{

Manager::Manager(bus::bus& bus, const char* path)
        : _xyz_openbmc_project_Loggin_Manager_interface(
                bus, path, _interface, _vtable, this)
{
}


int Manager::_callback_Commit(
        sd_bus_message* msg, void* context, sd_bus_error* error)
{
    auto m = message::message(sd_bus_message_ref(msg));

    i msgid{};
    s emsg{};

    m.read(msgid, emsg);

    auto o = static_cast<Manager*>(context);
    o->commit(msgid, emsg);

    auto reply = m.new_method_return();
    // No data to append on reply.

    reply.method_return();

    return 0;
}

namespace details
{
namespace Manager
{
static const auto _param_Commit =
        utility::tuple_to_array(message::types::type_id<
                i, s>());
static const auto _return_Commit =
        utility::tuple_to_array(message::types::type_id<
                >());
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

} // namespace logging
} // namespace openbmc_project
} // namespace xyz
} // namespace server
} // namespace sdbusplus

