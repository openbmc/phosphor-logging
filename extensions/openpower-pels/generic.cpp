// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "generic.hpp"

#include <phosphor-logging/lg2.hpp>

namespace openpower
{
namespace pels
{

void Generic::unflatten(Stream& stream)
{
    stream >> _header;

    if (_header.size <= SectionHeader::flattenedSize())
    {
        throw std::out_of_range(
            "Generic::unflatten: SectionHeader::size field too small");
    }

    size_t dataLength = _header.size - SectionHeader::flattenedSize();
    _data.resize(dataLength);

    stream >> _data;
}

void Generic::flatten(Stream& stream) const
{
    stream << _header << _data;
}

Generic::Generic(Stream& pel)
{
    try
    {
        unflatten(pel);
        validate();
    }
    catch (const std::exception& e)
    {
        lg2::error("Cannot unflatten generic section: {EXCEPTION}", "EXCEPTION",
                   e);
        _valid = false;
    }
}

void Generic::validate()
{
    // Nothing to validate
    _valid = true;
}

} // namespace pels
} // namespace openpower
