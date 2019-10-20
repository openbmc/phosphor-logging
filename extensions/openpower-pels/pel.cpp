#include "pel.hpp"

#include "bcd_time.hpp"
#include "log_id.hpp"
#include "section_factory.hpp"
#include "stream.hpp"
#include "tools/peltoolutils.hpp"

#include <iostream>
#include <phosphor-logging/log.hpp>
#include <vector>

namespace openpower
{
namespace pels
{
namespace message = openpower::pels::message;

PEL::PEL(const message::Entry& entry, uint32_t obmcLogID, uint64_t timestamp,
         phosphor::logging::Entry::Level severity)
{
    _ph = std::make_unique<PrivateHeader>(entry.componentID, obmcLogID,
                                          timestamp);
    _uh = std::make_unique<UserHeader>(entry, severity);

    // Add future sections here and update the count

    _ph->sectionCount() = 2;
}

PEL::PEL(std::vector<uint8_t>& data) : PEL(data, 0)
{
}

PEL::PEL(std::vector<uint8_t>& data, uint32_t obmcLogID)
{
    populateFromRawData(data, obmcLogID);
}

void PEL::populateFromRawData(std::vector<uint8_t>& data, uint32_t obmcLogID)
{
    Stream pelData{data};
    _ph = std::make_unique<PrivateHeader>(pelData);
    if (obmcLogID != 0)
    {
        _ph->obmcLogID() = obmcLogID;
    }

    _uh = std::make_unique<UserHeader>(pelData);

    // Use the section factory to create the rest of the objects
    for (size_t i = 2; i < _ph->sectionCount(); i++)
    {
        auto section = section_factory::create(pelData);
        _optionalSections.push_back(std::move(section));
    }
}

bool PEL::valid() const
{
    bool valid = _ph->valid();

    if (valid)
    {
        valid = _uh->valid();
    }

    if (valid)
    {
        if (!std::all_of(_optionalSections.begin(), _optionalSections.end(),
                         [](const auto& section) { return section->valid(); }))
        {
            valid = false;
        }
    }

    return valid;
}

void PEL::setCommitTime()
{
    auto now = std::chrono::system_clock::now();
    _ph->commitTimestamp() = getBCDTime(now);
}

void PEL::assignID()
{
    _ph->id() = generatePELID();
}

void PEL::flatten(std::vector<uint8_t>& pelBuffer)
{
    Stream pelData{pelBuffer};

    if (!valid())
    {
        using namespace phosphor::logging;
        log<level::WARNING>("Unflattening an invalid PEL");
    }

    _ph->flatten(pelData);
    _uh->flatten(pelData);

    for (auto& section : _optionalSections)
    {
        section->flatten(pelData);
    }
}

std::vector<uint8_t> PEL::data()
{
    std::vector<uint8_t> pelData;
    flatten(pelData);
    return pelData;
}

void PEL::toJson()
{
    char tmpB[5];
    std::string buf = "{";
    std::unique_ptr<PrivateHeader>& ph = this->privateHeader();
    if (ph->valid())
    {
        uint8_t hi_lo[] = {(uint8_t)(ph->header().id >> 8),
                           (uint8_t)ph->header().id}; // { 0xAA, 0xFF }
        sprintf(tmpB, "%c%c", hi_lo[0], hi_lo[1]);
        std::string tmpSr(tmpB);
        std::string sr2 = sectionTitles[tmpSr];
        buf += "\n\"" + sr2 + "\":[\n ";
        std::vector<uint8_t> _data;
        Stream s{_data};
        ph->flatten(s);
        std::string dstr = dumpHex2(std::data(_data), _data.size());
        buf += dstr + "\n],\n";
    }
    std::unique_ptr<UserHeader>& uh = this->userHeader();
    if (uh->valid())
    {
        uint8_t hi_lo[] = {(uint8_t)(uh->header().id >> 8),
                           (uint8_t)uh->header().id}; // { 0xAA, 0xFF }
        sprintf(tmpB, "%c%c", hi_lo[0], hi_lo[1]);
        std::string tmpSr(tmpB);
        std::string sr2 = sectionTitles[tmpSr];
        buf += "\n\"" + sr2 + "\":[\n ";
        std::vector<uint8_t> _data;
        Stream s{_data};
        uh->flatten(s);
        std::string dstr = dumpHex2(std::data(_data), _data.size());
        buf += dstr + "\n],\n";
    }
    for (auto& section : this->optionalSections())
    {
        if (section->valid())
        {
            uint8_t hi_lo[] = {(uint8_t)(section->header().id >> 8),
                               (uint8_t)section->header().id}; // { 0xAA, 0xFF }
            sprintf(tmpB, "%c%c", hi_lo[0], hi_lo[1]);
            std::string tmpSr(tmpB);
            std::string sr2 = sectionTitles[tmpSr];
            buf += "\n\"" + sr2 + "\":[\n ";
            std::vector<uint8_t> _data;
            Stream s{_data};
            section->flatten(s);
            std::string dstr = dumpHex2(std::data(_data), _data.size());
            buf += dstr + "\n],\n";
        }
    }
    buf += "}";
    std::size_t found = buf.rfind(",");
    if (found != std::string::npos)
        buf.replace(found, 1, "");
    std::cout << buf << std::endl;
}
} // namespace pels
} // namespace openpower
