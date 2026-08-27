#include "ael_common_info.hpp"

namespace phosphor::logging::extensions::ael
{

CommonInfo getCommonInfo()
{
    CommonInfo info;

    // TODO replace with actual data
    info.rackId = "AMD:RACK_ID";

    return info;
}

} // namespace phosphor::logging::extensions::ael
