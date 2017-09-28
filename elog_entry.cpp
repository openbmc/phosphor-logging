#include "elog_entry.hpp"
#include "log_manager.hpp"
#include "elog_serialize.hpp"

namespace phosphor
{
namespace logging
{

// Initial class version 0
int Entry::version = 0;

// TODO Add interfaces to handle the error log id numbering

void Entry::delete_()
{
    parent.erase(id());
}

bool Entry::resolved(bool value)
{
    auto current = sdbusplus::xyz::openbmc_project::
                       Logging::server::Entry::resolved();
    if (value != current)
    {
        value ?
            associations({}) :
            associations(assocs);
        current = sdbusplus::xyz::openbmc_project::
                      Logging::server::Entry::resolved(value);
        serialize(*this);
    }

    return current;
}

} // namespace logging
} // namepsace phosphor
