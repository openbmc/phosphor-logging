// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "json_utils.hpp"

#include "paths.hpp"

#include <stdio.h>

#include <nlohmann/json.hpp>

#include <cstring>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

namespace openpower
{
namespace pels
{

std::string escapeJSON(const std::string& input)
{
    std::string output;
    output.reserve(input.length());

    for (const auto c : input)
    {
        switch (c)
        {
            case '"':
                output += "\\\"";
                break;
            case '/':
                output += "\\/";
                break;
            case '\b':
                output += "\\b";
                break;
            case '\f':
                output += "\\f";
                break;
            case '\n':
                output += "\\n";
                break;
            case '\r':
                output += "\\r";
                break;
            case '\t':
                output += "\\t";
                break;
            case '\\':
                output += "\\\\";
                break;
            default:
                output += c;
                break;
        }
    }

    return output;
}
std::unique_ptr<char[]> dumpHex(const void* data, size_t size,
                                size_t indentCount, bool toJson)
{
    const int symbolSize = 100;
    std::string jsonIndent(indentLevel * indentCount, 0x20);
    if (toJson)
    {
        jsonIndent.append("\"");
    }
    std::unique_ptr<char[]> buffer{new char[std::max(70, 10 * (int)size)]()};
    char* symbol = (char*)calloc(symbolSize, sizeof(char));
    char* byteCount = (char*)calloc(11, sizeof(char));
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i)
    {
        if (i % 16 == 0)
        {
            if (!toJson)
            {
                snprintf(byteCount, 11, "%08X  ", static_cast<uint32_t>(i));
                strcat(buffer.get(), byteCount);
            }
            strcat(buffer.get(), jsonIndent.c_str());
        }
        snprintf(symbol, symbolSize, "%02X ", ((unsigned char*)data)[i]);
        strcat(buffer.get(), symbol);
        memset(symbol, 0, strlen(symbol));
        if (((unsigned char*)data)[i] >= ' ' &&
            ((unsigned char*)data)[i] <= '~')
        {
            ascii[i % 16] = ((unsigned char*)data)[i];
        }
        else
        {
            ascii[i % 16] = '.';
        }
        if ((i + 1) % 8 == 0 || i + 1 == size)
        {
            std::string asciiString(ascii);
            if (toJson)
            {
                asciiString = escapeJSON(asciiString);
            }
            strcat(buffer.get(), " ");
            if ((i + 1) % 16 == 0)
            {
                if (i + 1 != size && toJson)
                {
                    snprintf(symbol, symbolSize, "|  %s\",\n",
                             asciiString.c_str());
                }
                else if (toJson)
                {
                    snprintf(symbol, symbolSize, "|  %s\"\n",
                             asciiString.c_str());
                }
                else
                {
                    snprintf(symbol, symbolSize, "|  %s\n",
                             asciiString.c_str());
                }
                strcat(buffer.get(), symbol);
                memset(symbol, 0, strlen(symbol));
            }
            else if (i + 1 == size)
            {
                ascii[(i + 1) % 16] = '\0';
                if ((i + 1) % 16 <= 8)
                {
                    strcat(buffer.get(), " ");
                }
                for (j = (i + 1) % 16; j < 16; ++j)
                {
                    strcat(buffer.get(), "   ");
                }
                std::string asciiString2(ascii);
                if (toJson)
                {
                    asciiString2 = escapeJSON(asciiString2);
                    snprintf(symbol, symbolSize, "|  %s\"\n",
                             asciiString2.c_str());
                }
                else
                {
                    snprintf(symbol, symbolSize, "|  %s\n",
                             asciiString2.c_str());
                }

                strcat(buffer.get(), symbol);
                memset(symbol, 0, strlen(symbol));
            }
        }
    }
    free(byteCount);
    free(symbol);
    return buffer;
}

void jsonInsert(std::string& jsonStr, const std::string& fieldName,
                const std::string& fieldValue, uint8_t indentCount)
{
    const int8_t spacesToAppend =
        colAlign - (indentCount * indentLevel) - fieldName.length() - 3;
    const std::string jsonIndent(indentCount * indentLevel, 0x20);
    jsonStr.append(jsonIndent + "\"" + fieldName + "\":");
    if (spacesToAppend >= 0)
    {
        jsonStr.append(spacesToAppend, 0x20);
    }
    else
    {
        jsonStr.append(1, 0x20);
    }
    jsonStr.append("\"" + fieldValue + "\",\n");
}

void jsonInsertArray(std::string& jsonStr, const std::string& fieldName,
                     const std::vector<std::string>& values,
                     uint8_t indentCount)
{
    const std::string jsonIndent(indentCount * indentLevel, 0x20);
    if (!values.empty())
    {
        jsonStr.append(jsonIndent + "\"" + fieldName + "\": [\n");
        for (size_t i = 0; i < values.size(); i++)
        {
            jsonStr.append(colAlign, 0x20);
            if (i == values.size() - 1)
            {
                jsonStr.append("\"" + values[i] + "\"\n");
            }
            else
            {
                jsonStr.append("\"" + values[i] + "\",\n");
            }
        }
        jsonStr.append(jsonIndent + "],\n");
    }
    else
    {
        const int8_t spacesToAppend =
            colAlign - (indentCount * indentLevel) - fieldName.length() - 3;
        jsonStr.append(jsonIndent + "\"" + fieldName + "\":");
        if (spacesToAppend > 0)
        {
            jsonStr.append(spacesToAppend, 0x20);
        }
        else
        {
            jsonStr.append(1, 0x20);
        }
        jsonStr.append("[],\n");
    }
}

std::string trimEnd(std::string s)
{
    const char* t = " \t\n\r\f\v";
    if (s.find_last_not_of(t) != std::string::npos)
    {
        s.erase(s.find_last_not_of(t) + 1);
    }
    return s;
}

/**
 * @brief Lookup the component ID in a JSON file named
 *        after the creator ID.
 *
 * Keeps a cache of the JSON it reads to live throughout
 * the peltool call as the JSON can be reused across
 * PEL sections or even across PELs.
 *
 * @param[in] compID - The component ID
 * @param[in] creatorID - The creator ID for the PEL
 * @return optional<string> - The comp name, or std::nullopt
 */
static std::optional<std::string> lookupComponentName(uint16_t compID,
                                                      char creatorID)
{
    static std::map<char, nlohmann::json> jsonCache;
    nlohmann::json jsonData;
    nlohmann::json* jsonPtr = &jsonData;
    std::filesystem::path filename{
        std::string{creatorID} + "_component_ids.json"};
    filename = getPELReadOnlyDataPath() / filename;

    auto jsonIt = jsonCache.find(creatorID);
    if (jsonIt != jsonCache.end())
    {
        jsonPtr = &(jsonIt->second);
    }
    else
    {
        std::error_code ec;
        if (!std::filesystem::exists(filename, ec))
        {
            return std::nullopt;
        }

        std::ifstream file{filename};
        if (!file)
        {
            return std::nullopt;
        }

        jsonData = nlohmann::json::parse(file, nullptr, false);
        if (jsonData.is_discarded())
        {
            return std::nullopt;
        }

        jsonCache.emplace(creatorID, jsonData);
    }

    auto id = getNumberString("%04X", compID);

    auto it = jsonPtr->find(id);
    if (it == jsonPtr->end())
    {
        return std::nullopt;
    }

    return it->get<std::string>();
}

/**
 * @brief Convert the component ID to a 2 character string
 *        if both bytes are nonzero
 *
 * e.g. 0x4552 -> "ER"
 *
 * @param[in] compID - The component ID
 * @return optional<string> - The two character string, or std::nullopt.
 */
static std::optional<std::string> convertCompIDToChars(uint16_t compID)
{
    uint8_t first = (compID >> 8) & 0xFF;
    uint8_t second = compID & 0xFF;
    if ((first != 0) && (second != 0))
    {
        std::string id{static_cast<char>(first)};
        id += static_cast<char>(second);
        return id;
    }

    return std::nullopt;
}

std::string getComponentName(uint16_t compID, uint8_t creatorID)
{
    // See if there's a JSON file with the names
    auto name = lookupComponentName(compID, creatorID);

    // If PHYP, convert to ASCII
    if (!name && ('H' == creatorID))
    {
        name = convertCompIDToChars(compID);
    }

    if (!name)
    {
        name = getNumberString("0x%04X", compID);
    }

    return *name;
}

} // namespace pels
} // namespace openpower
