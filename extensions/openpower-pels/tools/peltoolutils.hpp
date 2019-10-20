#pragma once

#include <ctype.h>
#include <stdio.h>

#include <fstream>
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
 * @param[in] std::string - the unescaped JSON as a string literal
 * @return std::string - escaped JSON string literal
 */
std::string escapeJSON(const std::string& input);

/**
 * @brief get hex dump for PEL section in json format.
 * @param[in] const void* - Raw PEL data
 * @param[i] size_t - size of Raw PEL
 * @return char * - the Hex dump
 */
char* dumpHex(const void* data, size_t size);

/**
 * @brief get data form raw PEL file.
 * @param[in] std::string Name of file with raw PEL
 * @return std::vector<uint8_t> char vector read from raw PEL file.
 */
std::vector<uint8_t> getFileData(std::string name);

} // namespace pels
} // namespace openpower
