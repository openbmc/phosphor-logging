#pragma once

#include <ctype.h>
#include <stdio.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace openpower
{
namespace pels
{
/**
 * @brief Used to get the hex dump of a part of a section.
 */
void hexDump(const void* ptr, int buflen);
std::string hexDumpString(const void* ptr, int buflen);

/**
 * @brief Used to get the hex dump of a part of a section.
 */
template <typename byte_type = std::uint8_t>
std::string hexdump(const byte_type* buffer, std::size_t size,
                    const std::string& title = "HEXDUMP");
char* dumpHex2(const void* data, size_t size);
} // namespace pels
} // namespace openpower
