// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "pce_identity.hpp"

namespace openpower
{
namespace pels
{
namespace src
{

PCEIdentity::PCEIdentity(Stream& pel)
{
    pel >> _type >> _size >> _flags >> _mtms;

    // Whatever is left is the enclosure name.
    if (_size < (4 + _mtms.flattenedSize()))
    {
        throw std::runtime_error("PCE identity structure size field too small");
    }

    size_t pceNameSize = _size - (4 + _mtms.flattenedSize());

    _pceName.resize(pceNameSize);
    pel >> _pceName;
}

void PCEIdentity::flatten(Stream& pel) const
{
    pel << _type << _size << _flags << _mtms << _pceName;
}

} // namespace src
} // namespace pels
} // namespace openpower
