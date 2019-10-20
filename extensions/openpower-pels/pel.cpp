#include "pel.hpp"

#include "bcd_time.hpp"
#include "log_id.hpp"
#include "pel_values.hpp"
#include "section_factory.hpp"
#include "stream.hpp"
#include "tools/peltoolutils.hpp"

#include <iostream>
#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{
namespace message = openpower::pels::message;
namespace pv = openpower::pels::pel_values;

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

std::string PEL::printPEL(Section& section, std::string& buf)
{
    char tmpB[5];
    if (section.valid())
    {
        uint8_t id[] = {static_cast<uint8_t>(section.header().id >> 8),
                        static_cast<uint8_t>(section.header().id)};
        sprintf(tmpB, "%c%c", id[0], id[1]);
        std::string sectionID(tmpB);
        std::string sectionName = pv::sectionTitles[sectionID];
        buf += "\n\"" + sectionName + "\":[\n ";
        std::vector<uint8_t> data;
        Stream s{data};
        section.flatten(s);
        std::string dstr = dumpHex(std::data(data), data.size());
        buf += dstr + "\n],\n";
    }
    else
        buf += "\n\"Invalid Section  \":[\n invalid \n],\n";
    return buf;
}

void PEL::toJSON()
{
    std::string buf = "{";
    printPEL(*(_ph.get()), buf);
    printPEL(*(_uh.get()), buf);
    for (auto& section : this->optionalSections())
    {
        printPEL(*(section.get()), buf);
    }
    buf += "}";
    std::size_t found = buf.rfind(",");
    if (found != std::string::npos)
        buf.replace(found, 1, "");
    std::cout << buf << std::endl;
}
} // namespace pels
} // namespace openpower
