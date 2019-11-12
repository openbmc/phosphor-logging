/**
 * Copyright © 2019 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "hexdump.hpp"

#include <stdio.h>

#include <cstring>
#include <sstream>
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
char* dumpHex(const void* data, size_t size)
{
    const int symbolSize = 100;
    char* buffer = (char*)calloc(10 * size, sizeof(char));
    char* symbol = (char*)calloc(symbolSize, sizeof(char));
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i)
    {
        if (i % 16 == 0)
        {
            strcat(buffer, "\"");
        }
        snprintf(symbol, symbolSize, "%02X ", ((unsigned char*)data)[i]);
        strcat(buffer, symbol);
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
            asciiString = escapeJSON(asciiString);
            const char* asciiToPrint = asciiString.c_str();
            strcat(buffer, " ");
            if ((i + 1) % 16 == 0)
            {
                if (i + 1 != size)
                {
                    snprintf(symbol, symbolSize, "|  %s\", \n ", asciiToPrint);
                }
                else
                {
                    snprintf(symbol, symbolSize, "|  %s\" \n ", asciiToPrint);
                }
                strcat(buffer, symbol);
                memset(symbol, 0, strlen(symbol));
            }
            else if (i + 1 == size)
            {
                ascii[(i + 1) % 16] = '\0';
                if ((i + 1) % 16 <= 8)
                {
                    strcat(buffer, " ");
                }
                for (j = (i + 1) % 16; j < 16; ++j)
                {
                    strcat(buffer, "   ");
                }
                std::string asciiString2(ascii);
                asciiString2 = escapeJSON(asciiString2);
                asciiToPrint = asciiString2.c_str();
                snprintf(symbol, symbolSize, "|  %s\" \n ", asciiToPrint);
                strcat(buffer, symbol);
                memset(symbol, 0, strlen(symbol));
            }
        }
    }
    free(symbol);
    return buffer;
}
} // namespace pels
} // namespace openpower
