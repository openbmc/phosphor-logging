#include "elog_entry.hpp"
#include "log_manager.hpp"
#include "elog_serialize.hpp"

namespace phosphor
{
namespace logging
{

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

AssociationList Entry::associations(AssociationList value)
{

    auto _associations =
        sdbusplus::org::openbmc::server::Associations::associations();

    if (_associations != value)
    {
        assocs = value;
        _associations =
           sdbusplus::org::openbmc::server::Associations::associations(value);
        serialize(*this);
    }

    return _associations;
}

} // namespace logging
} // namepsace phosphor
