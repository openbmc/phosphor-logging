#include "mru.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{
namespace src
{

using namespace phosphor::logging;

MRU::MRU(Stream& pel)
{
    pel >> _type >> _size >> _flags >> _reserved4B;

    size_t numMRUs = _flags & 0xF;

    for (size_t i = 0; i < numMRUs; i++)
    {
        MRUCallout mru;
        pel >> mru.priority;
        pel >> mru.id;
        _mrus.push_back(std::move(mru));
    }

    size_t actualSize = sizeof(_type) + sizeof(_size) + sizeof(_flags) +
                        sizeof(_reserved4B) +
                        (sizeof(MRUCallout) * _mrus.size());
    if (_size != actualSize)
    {
        log<level::WARNING>("MRU callout section in PEL has listed size that "
                            "doesn't match actual size",
                            entry("SUBSTRUCTURE_SIZE=%lu", _size),
                            entry("NUM_MRUS=%lu", _mrus.size()),
                            entry("ACTUAL_SIZE=%lu", actualSize));
    }
}

void MRU::flatten(Stream& pel)
{
    pel << _type << _size << _flags << _reserved4B;

    for (auto& mru : _mrus)
    {
        pel << mru.priority;
        pel << mru.id;
    }
}
} // namespace src
} // namespace pels
} // namespace openpower
