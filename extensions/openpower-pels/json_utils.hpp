#pragma once

#include <ctype.h>
#include <stdio.h>

#include <cstdint>
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
 * @param[in] const void* data - Raw PEL data
 * @param[i] size_t size - size of Raw PEL
 * @param[in] size_t indentCount - The number of indent levels to indent
 * @param[in] bool toJson - if true, output lines as JSON array, else print
 *            output as plain text
 * @return char * - the Hex dump
 */
char* dumpHex(const void* data, size_t size, size_t indentCount,
              bool toJson = true);

/**
 * @brief Inserts key-value into a JSON string
 *
 * @param[in] jsonStr - The JSON string
 * @param[in] fieldName - The JSON key to insert
 * @param[in] fieldValue - The JSON value to insert
 * @param[in] indentCount - Indent count for the line
 */
void jsonInsert(std::string& jsonStr, const std::string& fieldName,
                const std::string& fieldValue, uint8_t indentCount);

/**
 * @brief Inserts key-value array into a JSON string
 *
 * @param[in] jsonStr - The JSON string
 * @param[in] fieldName - The JSON key to insert
 * @param[in] values - The JSON array to insert
 * @param[in] indentCount - Indent count for the line
 */
void jsonInsertArray(std::string& jsonStr, const std::string& fieldName,
                     const std::vector<std::string>& values,
                     uint8_t indentCount);

/**
 * @brief Converts an integer to a formatted string
 * @param[in] format - the format of output string
 * @param[in] number - the integer to convert
 * @return std::string - the formatted string
 */
template <typename T>
std::string getNumberString(const char* format, T number)
{
    char* value = nullptr;
    std::string numString;

    static_assert(std::is_integral<T>::value, "Integral required.");

    int len = asprintf(&value, format, number);
    if (len >= 0)
    {
        numString = value;
    }
    else
    {
        throw std::invalid_argument(
            std::string("getNumberString: invalid format string: ") + format);
    }
    free(value);

    return numString;
}

/**
 * @brief helper function to trim trailing whitespaces
 * @return std::string - trimmed string
 * @param[in] std::string - string to trim
 */
std::string trimEnd(std::string s);

/**
 * @brief Returns the component name for the component ID.
 *
 * It will try to look up the name to use in JSON files based on
 * the creator ID.  If PHYP, will convert the component ID to
 * two characters.
 *
 * If nothing else, it will just return the name as a string like
 * "0x1234".
 *
 * @return std::string - The component name
 */
std::string getComponentName(uint16_t compID, uint8_t creatorID);

} // namespace pels
} // namespace openpower
