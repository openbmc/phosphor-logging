#include "peltoolutils.hpp"

#include <stdio.h>

#include <cstring>
#include <sstream>
#include <string>

namespace openpower
{
namespace pels
{

char* dumpHex2(const void* data, size_t size)
{
    const int symbolSize = 100;
    char* buffer = (char*)calloc(10 * size, sizeof(char));
    char* symbol = (char*)calloc(symbolSize, sizeof(char));
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i)
    {
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
            strcat(buffer, " ");
            if ((i + 1) % 16 == 0)
            {
                snprintf(symbol, symbolSize, "|  %s \n", ascii);
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
                snprintf(symbol, symbolSize, "|  %s \n", ascii);
                strcat(buffer, symbol);
                memset(symbol, 0, strlen(symbol));
            }
        }
    }
    free(symbol);
    return buffer;
}
/**
 * @brief Used to get the hex dump of a PEL.
 */
void hexDump(const void* ptr, int buflen)
{
    unsigned char* buf = (unsigned char*)ptr;
    int i, j;
    for (i = 0; i < buflen; i += 16)
    {
        printf("%06x: ", i);
        for (j = 0; j < 16; j++)
            if (i + j < buflen)
                printf("%02x ", buf[i + j]);
            else
                printf("   ");
        printf(" ");
        for (j = 0; j < 16; j++)
            if (i + j < buflen)
                printf("%c", isprint(buf[i + j]) ? buf[i + j] : '.');
        printf("\n");
    }
}

/**
 * @brief Used to get the hex dump of a part of a section.
 * Implemented by derived classes.
 */
template <typename byte_type = std::uint8_t>
std::string hexdump(const byte_type* buffer, std::size_t size,
                    const std::string& title)
{
    using namespace std;
    ostringstream ost;
    ost << title << '\n';
    const auto bytes = vector<byte_type>(buffer, buffer + size);
    for (auto p = cbegin(bytes); p < cend(bytes);)
    {
        constexpr unsigned int ptrSize = sizeof(void*);
        constexpr auto byteSize = sizeof(byte_type);
        constexpr auto wordsPerRow = 4;
        const auto bytesToGo =
            static_cast<unsigned>(std::distance(p, cend(bytes)));
        const auto nBytes = std::min(ptrSize, bytesToGo);
        const auto bytesPrinted =
            static_cast<unsigned>(std::distance(cbegin(bytes), p));
        const auto isFirstRow = bytesPrinted == 0;
        const auto isNewRow = (bytesPrinted % (ptrSize * wordsPerRow)) == 0;
        const auto isLastRow = (p + nBytes) == cend(bytes);

        if (isNewRow && !isLastRow)
        {
            if (!isFirstRow)
            {
                ost << '\n';
            }
            ost << hex << setw(ptrSize * 2) << setfill('0') << bytesPrinted
                << "   ";
        }

        {
            for_each(p, p + nBytes, [&ost](byte_type byte) {
                ost << ' ' << hex << setw(byteSize * 2) << setfill('0')
                    << static_cast<unsigned>(byte);
            });
            ost << "   ";
        }

        if (isLastRow)
        {
            ost << '\n';
        }

        p += nBytes;
    }
    return ost.str();
}

} // namespace pels
} // namespace openpower
