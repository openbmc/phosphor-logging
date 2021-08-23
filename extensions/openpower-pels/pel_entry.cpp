
#include "pel_entry.hpp"

namespace openpower
{
namespace pels
{

bool PELEntry::managementSystemAck(bool value)
{
    auto current = sdbusplus::org::open_power::Logging::PEL::server::Entry::
        managementSystemAck();
    if (value != current)
    {
        current = sdbusplus::org::open_power::Logging::PEL::server::Entry::
            managementSystemAck(value);
        // Update HMC acknowledge field in PEL
        _repo->setPELHMCTransState(
            getMyId(),
            (value ? TransmissionState::acked : TransmissionState::newPEL));
        Repository::LogID id{Repository::LogID::Obmc(getMyId())};
    }
    return current;
}

} // namespace pels
} // namespace openpower