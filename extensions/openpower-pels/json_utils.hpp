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
const uint8_t indentLevel = 4;
const uint8_t colAlign = 32;
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
 * @brief Inserts key-value into a JSON string
 *
 * @param[in] jsonStr - The JSON string
 * @param[in] fieldName - The JSON key to insert
 * @param[in] fieldValue - The JSON value to insert
 * @param[in] indentCount - Indent count for the line
 */
void jsonInsert(std::string& jsonStr, const std::string& fieldName,
                std::string& fieldValue, uint8_t indentCount);

/**
 * @brief Inserts key-value array into a JSON string
 *
 * @param[in] jsonStr - The JSON string
 * @param[in] fieldName - The JSON key to insert
 * @param[in] values - The JSON array to insert
 * @param[in] indentCount - Indent count for the line
 */
void jsonInsertArray(std::string& jsonStr, const std::string& fieldName,
                     std::vector<std::string>& values, uint8_t indentCount);
} // namespace pels
} // namespace openpower
