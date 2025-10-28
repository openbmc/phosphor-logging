// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "mru.hpp"

#include <phosphor-logging/lg2.hpp>

namespace openpower
{
namespace pels
{
namespace src
{

// The MRU substructure supports up to 15 MRUs.
static constexpr size_t maxMRUs = 15;

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

    size_t actualSize =
        sizeof(_type) + sizeof(_size) + sizeof(_flags) + sizeof(_reserved4B) +
        (sizeof(MRUCallout) * _mrus.size());
    if (_size != actualSize)
    {
        lg2::warning(
            "MRU callout section in PEL with {NUM_MRUS} MRUs has listed size "
            "{SUBSTRUCTURE_SIZE} that doesn't match the actual size "
            "{ACTUAL_SIZE}",
            "SUBSTRUCTURE_SIZE", _size, "NUM_MRUS", _mrus.size(), "ACTUAL_SIZE",
            actualSize);
    }
}

MRU::MRU(const std::vector<MRUCallout>& mrus)
{
    if (mrus.empty())
    {
        lg2::error("Trying to create a MRU section with no MRUs");
        throw std::runtime_error{"Trying to create a MRU section with no MRUs"};
    }

    _mrus = mrus;
    if (_mrus.size() > maxMRUs)
    {
        _mrus.resize(maxMRUs);
    }

    _type = substructureType;
    _size = sizeof(_type) + sizeof(_size) + sizeof(_flags) +
            sizeof(_reserved4B) + (sizeof(MRUCallout) * _mrus.size());
    _flags = _mrus.size();
    _reserved4B = 0;
}

void MRU::flatten(Stream& pel) const
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
