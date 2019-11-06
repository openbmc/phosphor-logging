/**
 * Copyright © 2019 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "callout.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{
namespace src
{

using namespace phosphor::logging;

Callout::Callout(Stream& pel)
{
    pel >> _size >> _flags >> _priority >> _locationCodeSize;

    if (_locationCodeSize)
    {
        _locationCode.resize(_locationCodeSize);
        pel >> _locationCode;
    }

    size_t currentSize = 4 + _locationCodeSize;

    // Read in the substructures until the end of this structure.
    // Any stream overflows will throw an exception up to the SRC constructor
    while (_size > currentSize)
    {
        // Peek the type
        uint16_t type = 0;
        pel >> type;
        pel.offset(pel.offset() - 2);

        switch (type)
        {
            case FRUIdentity::substructureType:
            {
                _fruIdentity = std::make_unique<FRUIdentity>(pel);
                currentSize += _fruIdentity->flattenedSize();
                break;
            }
            case PCEIdentity::substructureType:
            {
                _pceIdentity = std::make_unique<PCEIdentity>(pel);
                currentSize += _pceIdentity->flattenedSize();
                break;
            }
            case MRU::substructureType:
            {
                _mru = std::make_unique<MRU>(pel);
                currentSize += _mru->flattenedSize();
                break;
            }
            default:
                log<level::ERR>("Invalid Callout subsection type",
                                entry("CALLOUT_TYPE=0x%X", type));
                throw std::runtime_error("Invalid Callout subsection type");
                break;
        }
    }
}

size_t Callout::flattenedSize()
{
    size_t size = sizeof(_size) + sizeof(_flags) + sizeof(_priority) +
                  sizeof(_locationCodeSize) + _locationCodeSize;

    size += _fruIdentity ? _fruIdentity->flattenedSize() : 0;
    size += _pceIdentity ? _pceIdentity->flattenedSize() : 0;
    size += _mru ? _mru->flattenedSize() : 0;

    return size;
}

void Callout::flatten(Stream& pel) const
{
    pel << _size << _flags << _priority << _locationCodeSize;

    if (_locationCodeSize)
    {
        pel << _locationCode;
    }

    if (_fruIdentity)
    {
        _fruIdentity->flatten(pel);
    }

    if (_pceIdentity)
    {
        _pceIdentity->flatten(pel);
    }
    if (_mru)
    {
        _mru->flatten(pel);
    }
}

} // namespace src
} // namespace pels
} // namespace openpower
