#include "elog_entry.hpp"
#include "log_manager.hpp"

namespace phosphor
{
namespace logging
{

// TODO Add interfaces to handle the error log id numbering

void Entry::delete_()
{
    parent.erase(id());
}

} // namespace logging
} // namepsace phosphor
