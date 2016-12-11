#include <cstdio>
#include "elog_entry.hpp"

namespace phosphor
{
namespace logging
{

Entry::Entry(sdbusplus::bus::bus& bus, const char* obj) :
    details::ServerObject<details::EntryIface>(bus, obj)
{
    // TODO Add logic to populate error log dbus objects that exist on flash
}

// TODO Add interfaces to handle the error log id numbering

} // namespace logging
} // namepsace phosphor
