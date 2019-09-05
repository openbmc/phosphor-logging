#include "pel.hpp"

#include "bcd_time.hpp"
#include "log_id.hpp"
#include "stream.hpp"

namespace openpower
{
namespace pels
{

PEL::PEL(const std::vector<uint8_t>& data) : PEL(data, 0)
{
}

PEL::PEL(const std::vector<uint8_t>& data, uint32_t obmcLogID) : _rawPEL(data)
{
    populateFromRawData(obmcLogID);
}

void PEL::populateFromRawData(uint32_t obmcLogID)
{
    Stream pelData{_rawPEL};
    _ph = std::make_unique<PrivateHeader>(pelData);
    if (obmcLogID != 0)
    {
        _ph->obmcLogID() = obmcLogID;
    }

    _uh = std::make_unique<UserHeader>(pelData);
}

bool PEL::valid() const
{
    bool valid = _ph->valid();

    if (valid)
    {
        valid = _uh->valid();
    }

    return valid;
}

void PEL::setCommitTime()
{
    auto now = std::chrono::system_clock::now();
    _ph->commitTimestamp() = getBCDTime(now);
}

void PEL::assignID()
{
    _ph->id() = generatePELID();
}

void PEL::flatten(std::vector<uint8_t>& pelBuffer)
{
    Stream pelData{pelBuffer};
    if (_ph->valid())
    {
        _ph->flatten(pelData);
    }
    else
    {
        return;
    }

    if (_uh->valid())
    {
        _uh->flatten(pelData);
    }
}

std::vector<uint8_t> PEL::data()
{
    // Until we can recreate a complete PEL from objects, need to just flatten
    // on top of the original PEL data which we need to keep around for this
    // reason.

    flatten(_rawPEL);
    return _rawPEL;
}

} // namespace pels
} // namespace openpower
