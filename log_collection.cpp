#include "log_collection.hpp"
#include "log_manager.hpp"

namespace phosphor
{
namespace logging
{


void Collection::delete_()
{
    parent.eraseAll();
}

} // namespace logging
} // namepsace phosphor
