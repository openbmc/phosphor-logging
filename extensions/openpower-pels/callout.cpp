// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "callout.hpp"

#include <phosphor-logging/lg2.hpp>

#include <map>

namespace openpower
{
namespace pels
{
namespace src
{

constexpr size_t locationCodeMaxSize = 80;

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
                lg2::error("Invalid Callout subsection type {CALLOUT_TYPE}",
                           "CALLOUT_TYPE", lg2::hex, type);
                throw std::runtime_error("Invalid Callout subsection type");
                break;
        }
    }
}

Callout::Callout(CalloutPriority priority, const std::string& locationCode,
                 const std::string& partNumber, const std::string& ccin,
                 const std::string& serialNumber) :
    Callout(priority, locationCode, partNumber, ccin, serialNumber,
            std::vector<MRU::MRUCallout>{})
{}

Callout::Callout(CalloutPriority priority, const std::string& locationCode,
                 const std::string& partNumber, const std::string& ccin,
                 const std::string& serialNumber,
                 const std::vector<MRU::MRUCallout>& mrus)
{
    _flags = calloutType | fruIdentIncluded;

    _priority = static_cast<uint8_t>(priority);

    setLocationCode(locationCode);

    _fruIdentity =
        std::make_unique<FRUIdentity>(partNumber, ccin, serialNumber);

    if (!mrus.empty())
    {
        _flags |= mruIncluded;
        _mru = std::make_unique<MRU>(mrus);
    }

    _size = flattenedSize();
}

Callout::Callout(CalloutPriority priority, const std::string& procedure,
                 CalloutValueType type)
{
    _flags = calloutType | fruIdentIncluded;

    _priority = static_cast<uint8_t>(priority);

    _locationCodeSize = 0;

    _fruIdentity = std::make_unique<FRUIdentity>(procedure, type);

    _size = flattenedSize();
}

Callout::Callout(CalloutPriority priority, const std::string& symbolicFRU,
                 CalloutValueType type, const std::string& locationCode,
                 bool trustedLocationCode)
{
    _flags = calloutType | fruIdentIncluded;

    _priority = static_cast<uint8_t>(priority);

    setLocationCode(locationCode);

    _fruIdentity =
        std::make_unique<FRUIdentity>(symbolicFRU, type, trustedLocationCode);

    _size = flattenedSize();
}

void Callout::setLocationCode(const std::string& locationCode)
{
    if (locationCode.empty())
    {
        _locationCodeSize = 0;
        return;
    }

    std::copy(locationCode.begin(), locationCode.end(),
              std::back_inserter(_locationCode));

    if (_locationCode.size() < locationCodeMaxSize)
    {
        // Add a NULL, and then pad to a 4B boundary
        _locationCode.push_back('\0');

        while (_locationCode.size() % 4)
        {
            _locationCode.push_back('\0');
        }
    }
    else
    {
        // Too big - truncate it and ensure it ends in a NULL.
        _locationCode.resize(locationCodeMaxSize);
        _locationCode.back() = '\0';
    }

    _locationCodeSize = _locationCode.size();
}

size_t Callout::flattenedSize() const
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

bool Callout::operator==(const Callout& right) const
{
    if ((_locationCodeSize != 0) || (right.locationCodeSize() != 0))
    {
        return locationCode() == right.locationCode();
    }

    if (!_fruIdentity || !right.fruIdentity())
    {
        return false;
    }

    auto myProc = _fruIdentity->getMaintProc();
    auto otherProc = right.fruIdentity()->getMaintProc();
    if (myProc)
    {
        if (otherProc)
        {
            return *myProc == *otherProc;
        }
        return false;
    }

    auto myPN = _fruIdentity->getPN();
    auto otherPN = right.fruIdentity()->getPN();
    if (myPN && otherPN)
    {
        return *myPN == *otherPN;
    }

    return false;
}

bool Callout::operator>(const Callout& right) const
{
    // Treat all of the mediums the same
    const std::map<std::uint8_t, int> priorities = {
        {'H', 10}, {'M', 9}, {'A', 9}, {'B', 9}, {'C', 9}, {'L', 8}};

    if (!priorities.contains(priority()) ||
        !priorities.contains(right.priority()))
    {
        return false;
    }

    return priorities.at(priority()) > priorities.at(right.priority());
}

} // namespace src
} // namespace pels
} // namespace openpower
