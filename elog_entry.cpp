#include <cstdio>
#include "elog_entry.hpp"

namespace phosphor
{
namespace logging
{

Entry::Entry(sdbusplus::bus::bus& bus,
                 const char* obj) :
    details::ServerObject<details::EntryIface>(bus, obj)
{
}

} // namespace logging
} // namepsace phosphor
