#include "pel.hpp"

#include "bcd_time.hpp"
#include "failing_mtms.hpp"
#include "hexdump.hpp"
#include "log_id.hpp"
#include "pel_rules.hpp"
#include "pel_values.hpp"
#include "section_factory.hpp"
#include "src.hpp"
#include "stream.hpp"
#include "user_data_formats.hpp"

#include <iostream>
#include <phosphor-logging/log.hpp>

namespace openpower
{
namespace pels
{
namespace message = openpower::pels::message;
namespace pv = openpower::pels::pel_values;

PEL::PEL(const message::Entry& entry, uint32_t obmcLogID, uint64_t timestamp,
         phosphor::logging::Entry::Level severity,
         const AdditionalData& additionalData,
         const DataInterfaceBase& dataIface)
{
    _ph = std::make_unique<PrivateHeader>(entry.componentID, obmcLogID,
                                          timestamp);
    _uh = std::make_unique<UserHeader>(entry, severity);

    auto src = std::make_unique<SRC>(entry, additionalData);
    _optionalSections.push_back(std::move(src));

    auto mtms = std::make_unique<FailingMTMS>(dataIface);
    _optionalSections.push_back(std::move(mtms));

    if (!additionalData.empty())
    {
        auto ud = util::makeADUserDataSection(additionalData);
        _optionalSections.push_back(std::move(ud));
    }

    _ph->setSectionCount(2 + _optionalSections.size());

    checkRulesAndFix();
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
        _ph->setOBMCLogID(obmcLogID);
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
    _ph->setCommitTimestamp(getBCDTime(now));
}

void PEL::assignID()
{
    _ph->setID(generatePELID());
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

std::optional<SRC*> PEL::primarySRC() const
{
    auto src = std::find_if(
        _optionalSections.begin(), _optionalSections.end(), [](auto& section) {
            return section->header().id ==
                   static_cast<uint16_t>(SectionID::primarySRC);
        });
    if (src != _optionalSections.end())
    {
        return static_cast<SRC*>(src->get());
    }

    return std::nullopt;
}

void PEL::checkRulesAndFix()
{
    auto [actionFlags, eventType] =
        pel_rules::check(_uh->actionFlags(), _uh->eventType(), _uh->severity());

    _uh->setActionFlags(actionFlags);
    _uh->setEventType(eventType);
}

namespace util
{

std::unique_ptr<UserData> makeADUserDataSection(const AdditionalData& ad)
{
    assert(!ad.empty());
    nlohmann::json json;

    // Remove the 'ESEL' entry, as it contains a full PEL in the value.
    if (ad.getValue("ESEL"))
    {
        auto newAD = ad;
        newAD.remove("ESEL");
        json = newAD.toJSON();
    }
    else
    {
        json = ad.toJSON();
    }

    auto jsonString = json.dump();
    std::vector<uint8_t> jsonData(jsonString.begin(), jsonString.end());

    return std::make_unique<UserData>(
        static_cast<uint16_t>(ComponentID::phosphorLogging),
        static_cast<uint8_t>(UserDataFormat::json),
        static_cast<uint8_t>(UserDataFormatVersion::json), jsonData);
}

} // namespace util
void PEL::printSectionInJSON(Section& section, std::string& buf) const
{
    char tmpB[5];
    if (section.valid())
    {
        uint8_t id[] = {static_cast<uint8_t>(section.header().id >> 8),
                        static_cast<uint8_t>(section.header().id)};
        sprintf(tmpB, "%c%c", id[0], id[1]);
        std::string sectionID(tmpB);
        std::string sectionName = pv::sectionTitles.count(sectionID)
                                      ? pv::sectionTitles.at(sectionID)
                                      : "Unknown Section";
        buf += "\n\"" + sectionName + "\":[\n ";
        std::vector<uint8_t> data;
        Stream s{data};
        section.flatten(s);
        std::string dstr = dumpHex(std::data(data), data.size());
        buf += dstr + "\n],\n";
    }
    else
    {
        buf += "\n\"Invalid Section  \":[\n invalid \n],\n";
    }
}

void PEL::toJSON()
{
    std::string buf = "{";
    printSectionInJSON(*(_ph.get()), buf);
    printSectionInJSON(*(_uh.get()), buf);
    for (auto& section : this->optionalSections())
    {
        printSectionInJSON(*(section.get()), buf);
    }
    buf += "}";
    std::size_t found = buf.rfind(",");
    if (found != std::string::npos)
        buf.replace(found, 1, "");
    std::cout << buf << std::endl;
}
} // namespace pels
} // namespace openpower
