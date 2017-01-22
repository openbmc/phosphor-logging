#include <algorithm>
#include <sdbusplus/server.hpp>
#include <sdbusplus/exception.hpp>
#include <xyz/openbmc_project/Logging/Entry/server.hpp>

namespace sdbusplus
{
namespace xyz
{
namespace openbmc_project
{
namespace Logging
{
namespace server
{

Entry::Entry(bus::bus& bus, const char* path)
        : _xyz_openbmc_project_Logging_Entry_interface(
                bus, path, _interface, _vtable, this)
{
}



auto Entry::id() const ->
        uint32_t
{
    return _id;
}

int Entry::_callback_get_Id(
        sd_bus* bus, const char* path, const char* interface,
        const char* property, sd_bus_message* reply, void* context,
        sd_bus_error* error)
{
    using sdbusplus::server::binding::details::convertForMessage;

    try
    {
        auto m = message::message(reply);

        auto o = static_cast<Entry*>(context);
        m.append(convertForMessage(o->id()));
    }
    catch(sdbusplus::internal_exception_t& e)
    {
        sd_bus_error_set_const(error, e.name(), e.description());
        return -EINVAL;
    }

    return true;
}

auto Entry::id(uint32_t value) ->
        uint32_t
{
    if (_id != value)
    {
        _id = value;
        _xyz_openbmc_project_Logging_Entry_interface.property_changed("Id");
    }

    return _id;
}

int Entry::_callback_set_Id(
        sd_bus* bus, const char* path, const char* interface,
        const char* property, sd_bus_message* value, void* context,
        sd_bus_error* error)
{
    try
    {
        auto m = message::message(value);

        auto o = static_cast<Entry*>(context);

        uint32_t v{};
        m.read(v);
        o->id(v);
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
namespace Entry
{
static const auto _property_Id =
    utility::tuple_to_array(message::types::type_id<
            uint32_t>());
}
}
auto Entry::timestamp() const ->
        int32_t
{
    return _timestamp;
}

int Entry::_callback_get_Timestamp(
        sd_bus* bus, const char* path, const char* interface,
        const char* property, sd_bus_message* reply, void* context,
        sd_bus_error* error)
{
    using sdbusplus::server::binding::details::convertForMessage;

    try
    {
        auto m = message::message(reply);

        auto o = static_cast<Entry*>(context);
        m.append(convertForMessage(o->timestamp()));
    }
    catch(sdbusplus::internal_exception_t& e)
    {
        sd_bus_error_set_const(error, e.name(), e.description());
        return -EINVAL;
    }

    return true;
}

auto Entry::timestamp(int32_t value) ->
        int32_t
{
    if (_timestamp != value)
    {
        _timestamp = value;
        _xyz_openbmc_project_Logging_Entry_interface.property_changed("Timestamp");
    }

    return _timestamp;
}

int Entry::_callback_set_Timestamp(
        sd_bus* bus, const char* path, const char* interface,
        const char* property, sd_bus_message* value, void* context,
        sd_bus_error* error)
{
    try
    {
        auto m = message::message(value);

        auto o = static_cast<Entry*>(context);

        int32_t v{};
        m.read(v);
        o->timestamp(v);
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
namespace Entry
{
static const auto _property_Timestamp =
    utility::tuple_to_array(message::types::type_id<
            int32_t>());
}
}
auto Entry::severity() const ->
        Level
{
    return _severity;
}

int Entry::_callback_get_Severity(
        sd_bus* bus, const char* path, const char* interface,
        const char* property, sd_bus_message* reply, void* context,
        sd_bus_error* error)
{
    using sdbusplus::server::binding::details::convertForMessage;

    try
    {
        auto m = message::message(reply);

        auto o = static_cast<Entry*>(context);
        m.append(convertForMessage(o->severity()));
    }
    catch(sdbusplus::internal_exception_t& e)
    {
        sd_bus_error_set_const(error, e.name(), e.description());
        return -EINVAL;
    }

    return true;
}

auto Entry::severity(Level value) ->
        Level
{
    if (_severity != value)
    {
        _severity = value;
        _xyz_openbmc_project_Logging_Entry_interface.property_changed("Severity");
    }

    return _severity;
}

int Entry::_callback_set_Severity(
        sd_bus* bus, const char* path, const char* interface,
        const char* property, sd_bus_message* value, void* context,
        sd_bus_error* error)
{
    try
    {
        auto m = message::message(value);

        auto o = static_cast<Entry*>(context);

        std::string v{};
        m.read(v);
        o->severity(convertLevelFromString(v));
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
namespace Entry
{
static const auto _property_Severity =
    utility::tuple_to_array(message::types::type_id<
            std::string>());
}
}
auto Entry::message() const ->
        std::string
{
    return _message;
}

int Entry::_callback_get_Message(
        sd_bus* bus, const char* path, const char* interface,
        const char* property, sd_bus_message* reply, void* context,
        sd_bus_error* error)
{
    using sdbusplus::server::binding::details::convertForMessage;

    try
    {
        auto m = message::message(reply);

        auto o = static_cast<Entry*>(context);
        m.append(convertForMessage(o->message()));
    }
    catch(sdbusplus::internal_exception_t& e)
    {
        sd_bus_error_set_const(error, e.name(), e.description());
        return -EINVAL;
    }

    return true;
}

auto Entry::message(std::string value) ->
        std::string
{
    if (_message != value)
    {
        _message = value;
        _xyz_openbmc_project_Logging_Entry_interface.property_changed("Message");
    }

    return _message;
}

int Entry::_callback_set_Message(
        sd_bus* bus, const char* path, const char* interface,
        const char* property, sd_bus_message* value, void* context,
        sd_bus_error* error)
{
    try
    {
        auto m = message::message(value);

        auto o = static_cast<Entry*>(context);

        std::string v{};
        m.read(v);
        o->message(v);
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
namespace Entry
{
static const auto _property_Message =
    utility::tuple_to_array(message::types::type_id<
            std::string>());
}
}
auto Entry::additionalData() const ->
        std::vector<std::string>
{
    return _additionalData;
}

int Entry::_callback_get_AdditionalData(
        sd_bus* bus, const char* path, const char* interface,
        const char* property, sd_bus_message* reply, void* context,
        sd_bus_error* error)
{
    using sdbusplus::server::binding::details::convertForMessage;

    try
    {
        auto m = message::message(reply);

        auto o = static_cast<Entry*>(context);
        m.append(convertForMessage(o->additionalData()));
    }
    catch(sdbusplus::internal_exception_t& e)
    {
        sd_bus_error_set_const(error, e.name(), e.description());
        return -EINVAL;
    }

    return true;
}

auto Entry::additionalData(std::vector<std::string> value) ->
        std::vector<std::string>
{
    if (_additionalData != value)
    {
        _additionalData = value;
        _xyz_openbmc_project_Logging_Entry_interface.property_changed("AdditionalData");
    }

    return _additionalData;
}

int Entry::_callback_set_AdditionalData(
        sd_bus* bus, const char* path, const char* interface,
        const char* property, sd_bus_message* value, void* context,
        sd_bus_error* error)
{
    try
    {
        auto m = message::message(value);

        auto o = static_cast<Entry*>(context);

        std::vector<std::string> v{};
        m.read(v);
        o->additionalData(v);
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
namespace Entry
{
static const auto _property_AdditionalData =
    utility::tuple_to_array(message::types::type_id<
            std::vector<std::string>>());
}
}


namespace
{
/** String to enum mapping for Entry::Level */
static const std::tuple<const char*, Entry::Level> mappingEntryLevel[] =
        {
            std::make_tuple( "xyz.openbmc_project.Logging.Entry.Level.Emergency",                 Entry::Level::Emergency ),
            std::make_tuple( "xyz.openbmc_project.Logging.Entry.Level.Alert",                 Entry::Level::Alert ),
            std::make_tuple( "xyz.openbmc_project.Logging.Entry.Level.Critical",                 Entry::Level::Critical ),
            std::make_tuple( "xyz.openbmc_project.Logging.Entry.Level.Error",                 Entry::Level::Error ),
            std::make_tuple( "xyz.openbmc_project.Logging.Entry.Level.Warning",                 Entry::Level::Warning ),
            std::make_tuple( "xyz.openbmc_project.Logging.Entry.Level.Notice",                 Entry::Level::Notice ),
            std::make_tuple( "xyz.openbmc_project.Logging.Entry.Level.Informational",                 Entry::Level::Informational ),
            std::make_tuple( "xyz.openbmc_project.Logging.Entry.Level.Debug",                 Entry::Level::Debug ),
        };

} // anonymous namespace

auto Entry::convertLevelFromString(std::string& s) ->
        Level
{
    auto i = std::find_if(
            std::begin(mappingEntryLevel),
            std::end(mappingEntryLevel),
            [&s](auto& e){ return 0 == strcmp(s.c_str(), std::get<0>(e)); } );
    if (std::end(mappingEntryLevel) == i)
    {
        throw sdbusplus::exception::InvalidEnumString();
    }
    else
    {
        return std::get<1>(*i);
    }
}

std::string convertForMessage(Entry::Level v)
{
    auto i = std::find_if(
            std::begin(mappingEntryLevel),
            std::end(mappingEntryLevel),
            [v](auto& e){ return v == std::get<1>(e); });
    return std::get<0>(*i);
}

const vtable::vtable_t Entry::_vtable[] = {
    vtable::start(),
    vtable::property("Id",
                     details::Entry::_property_Id
                        .data(),
                     _callback_get_Id,
                     _callback_set_Id,
                     vtable::property_::emits_change),
    vtable::property("Timestamp",
                     details::Entry::_property_Timestamp
                        .data(),
                     _callback_get_Timestamp,
                     _callback_set_Timestamp,
                     vtable::property_::emits_change),
    vtable::property("Severity",
                     details::Entry::_property_Severity
                        .data(),
                     _callback_get_Severity,
                     _callback_set_Severity,
                     vtable::property_::emits_change),
    vtable::property("Message",
                     details::Entry::_property_Message
                        .data(),
                     _callback_get_Message,
                     _callback_set_Message,
                     vtable::property_::emits_change),
    vtable::property("AdditionalData",
                     details::Entry::_property_AdditionalData
                        .data(),
                     _callback_get_AdditionalData,
                     _callback_set_AdditionalData,
                     vtable::property_::emits_change),
    vtable::end()
};

} // namespace server
} // namespace Logging
} // namespace openbmc_project
} // namespace xyz
} // namespace sdbusplus

