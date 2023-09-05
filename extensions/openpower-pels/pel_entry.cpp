#include "pel_entry.hpp"

#include <xyz/openbmc_project/Common/error.hpp>

namespace openpower
{
namespace pels
{

namespace common_error = sdbusplus::xyz::openbmc_project::Common::Error;

bool PELEntry::managementSystemAck(bool value)
{
    auto current = sdbusplus::server::org::open_power::logging::pel::Entry::
        managementSystemAck();
    if (value != current)
    {
        current = sdbusplus::server::org::open_power::logging::pel::Entry::
            managementSystemAck(value);

        Repository::LogID id{Repository::LogID::Obmc(getMyId())};
        if (auto logId = _repo->getLogID(id); logId.has_value())
        {
            // Update HMC acknowledge bit in PEL
            _repo->setPELHMCTransState(
                logId->pelID.id,
                (value ? TransmissionState::acked : TransmissionState::newPEL));
        }
        else
        {
            throw common_error::InvalidArgument();
        }
    }
    return current;
}

} // namespace pels
} // namespace openpower
