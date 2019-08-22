#include "user_header.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;
using json = nlohmann::json;

/**
 * @brief Used to get the hex dump of a part of a section.
 * Implemented by derived classes.
 */
template <typename byte_type = std::uint8_t>
std::string hexdump(const byte_type* buffer, std::size_t size,
                    const std::string& title = "HEXDUMP")
{
    using namespace std;
    ostringstream ost;
    ost << title << '\n';
    const auto bytes = vector<byte_type>(buffer, buffer + size);
    for (auto p = cbegin(bytes); p < cend(bytes);)
    {
        constexpr auto ptrSize = sizeof(void*);
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
/**
 * @brief Returns the section header in json format with values in hex.
 *
 * @return  char * - json string with values in hex
 */
const char* UserHeader::toJson()
{
    auto tmp = pel_values::findByValue(this->_eventSeverity,
                                       pel_values::severityValues);
    const char* severity = std::get<pel_values::registryNamePos>(*tmp);
    //TODO FOR ALL OTHER FIELDS
    auto uh = R"(
    {
        "Section Version": "",
        "Sub-section type": "",
        "Log Committed by": "",
        "Subsystem": "",
        "Event Scope": "",
        "Event Severity":""
    }
    )"_json;
    //any Hex dumps would be added here too
    uh["Event Severity"] = severity;
    std::stringstream ss;
    ss << "TODO" << uh.dump();
    const char* c = &*ss.str().begin();
    return c;
}
Stream& operator>>(Stream& s, UserHeader& uh)
{
    s >> uh._header >> uh._eventSubsystem >> uh._eventScope >>
        uh._eventSeverity >> uh._eventType >> uh._reserved4Byte1 >>
        uh._problemDomain >> uh._problemVector >> uh._actionFlags >>
        uh._reserved4Byte2;
    return s;
}

Stream& operator<<(Stream& s, UserHeader& uh)
{
    s << uh._header << uh._eventSubsystem << uh._eventScope << uh._eventSeverity
      << uh._eventType << uh._reserved4Byte1 << uh._problemDomain
      << uh._problemVector << uh._actionFlags << uh._reserved4Byte2;
    return s;
}

UserHeader::UserHeader(Stream& pel)
{
    try
    {
        pel >> *this;
        validate();
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Cannot unflatten user header",
                        entry("ERROR=%s", e.what()));
        _valid = false;
    }
}

void UserHeader::validate()
{
    bool failed = false;
    if (header().id != userHeaderSectionID)
    {
        log<level::ERR>("Invalid user header section ID",
                        entry("ID=0x%X", header().id));
        failed = true;
        return;
    }

    if (header().version != userHeaderVersion)
    {
        log<level::ERR>("Invalid user header version",
                        entry("VERSION=0x%X", header().version));
        failed = true;
        return;
    }

    _valid = (failed) ? false : true;
}

} // namespace pels
} // namespace openpower
