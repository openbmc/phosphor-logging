#pragma once

#include <ctype.h>
#include <stdio.h>

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
} // namespace pels
} // namespace openpower
