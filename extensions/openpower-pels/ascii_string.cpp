// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "ascii_string.hpp"

#include "pel_types.hpp"

namespace openpower
{
namespace pels
{
namespace src
{

AsciiString::AsciiString(Stream& stream)
{
    unflatten(stream);
}

AsciiString::AsciiString(const message::Entry& entry)
{
    // Power Error:  1100RRRR
    // BMC Error:    BDSSRRRR
    // where:
    //  RRRR = reason code
    //  SS = subsystem ID

    // First is type, like 'BD'
    setByte(0, entry.src.type);

    // Next is '00', or subsystem ID
    if (entry.src.type == static_cast<uint8_t>(SRCType::powerError))
    {
        setByte(2, 0x00);
    }
    else // BMC Error
    {
        // If subsystem wasn't specified, it should get set later in
        // the SRC constructor.  Default to 'other' in case it doesn't.
        setByte(2, entry.subsystem ? entry.subsystem.value() : 0x70);
    }

    // Then the reason code
    setByte(4, entry.src.reasonCode >> 8);
    setByte(6, entry.src.reasonCode & 0xFF);

    // Padded with spaces
    for (size_t offset = 8; offset < asciiStringSize; offset++)
    {
        _string[offset] = ' ';
    }
}

void AsciiString::flatten(Stream& stream) const
{
    stream.write(_string.data(), _string.size());
}

void AsciiString::unflatten(Stream& stream)
{
    stream.read(_string.data(), _string.size());

    // Only allow certain ASCII characters as other entities will
    // eventually want to display this.
    std::for_each(_string.begin(), _string.end(), [](auto& c) {
        if (!isalnum(c) && (c != ' ') && (c != '.') && (c != ':') && (c != '/'))
        {
            c = ' ';
        }
    });
}

std::string AsciiString::get() const
{
    std::string string{_string.begin(), _string.begin() + _string.size()};
    return string;
}

void AsciiString::setByte(size_t byteOffset, uint8_t value)
{
    assert(byteOffset < asciiStringSize);

    char characters[3];
    sprintf(characters, "%02X", value);

    auto writeOffset = byteOffset;
    _string[writeOffset++] = characters[0];
    _string[writeOffset] = characters[1];
}

} // namespace src
} // namespace pels
} // namespace openpower
