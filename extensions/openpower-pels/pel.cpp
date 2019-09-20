#include "pel.hpp"

#include "bcd_time.hpp"
#include "log_id.hpp"
#include "src.hpp"
#include "stream.hpp"

namespace openpower
{
namespace pels
{
namespace message = openpower::pels::message;

PEL::PEL(const message::Entry& entry, uint32_t obmcLogID, uint64_t timestamp,
         phosphor::logging::Entry::Level severity)
{
    _ph = std::make_unique<PrivateHeader>(entry.componentID, obmcLogID,
                                          timestamp);
    _uh = std::make_unique<UserHeader>(entry, severity);

    // Add future sections here and update the count

    _ph->sectionCount() = 2;
}

PEL::PEL(const std::vector<uint8_t>& data) : PEL(data, 0)
{
}

PEL::PEL(const std::vector<uint8_t>& data, uint32_t obmcLogID) : _rawPEL(data)
{
    _fromStream = true;
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

    _ph->flatten(pelData);

    // If constructed from a PEL stream originally, don't flatten the
    // rest of the objects until we support every PEL section type.
    // Still need the PrivateHeader, as we updated fields in it.
    if (_fromStream)
    {
        return;
    }

    _uh->flatten(pelData);
}

std::vector<uint8_t> PEL::data()
{
    // Until we can recreate a complete PEL from objects, need to just flatten
    // on top of the original PEL data which we need to keep around for this
    // reason.  If creating a PEL from scratch, _rawPEL will get filled in with
    // what we do have.

    flatten(_rawPEL);
    return _rawPEL;
}

} // namespace pels
} // namespace openpower
