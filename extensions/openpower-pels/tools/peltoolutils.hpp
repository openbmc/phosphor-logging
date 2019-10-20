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
 * @brief escape json - use it for PEL hex dumps.
 */
std::string escapeJSON(const std::string& input);

/**
 * @brief get hex dump for PEL section in json format.
 */
char* dumpHex2(const void* data, size_t size);
} // namespace pels
} // namespace openpower
