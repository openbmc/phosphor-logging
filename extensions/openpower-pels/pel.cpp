#include "pel.hpp"

#include "bcd_time.hpp"
#include "log_id.hpp"
#include "section_factory.hpp"
#include "stream.hpp"
#include "tools/peltoolutils.hpp"

#include <nlohmann/json.hpp>
#include <phosphor-logging/log.hpp>
#include <vector>

namespace openpower
{
namespace pels
{
namespace message = openpower::pels::message;

using namespace nlohmann;
using json = nlohmann::json;

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
    json j;
    for (auto& section : this->optionalSections())
    {
        if (section->valid())
        {
            std::string sr = std::to_string(section->header().id);
            char const* pchar = sr.c_str();
            std::vector<uint8_t> _data;
            Stream s{_data};
            section->flatten(s);
            j.push_back(json::object_t::value_type(
                pchar, dumpHex2(std::data(_data), _data.size())));
        }
    }
    std::string fs = j.dump(10);
    std::cout << fs;
    const char* cstr = fs.c_str();
    printf("|%s|\n", cstr);
}
} // namespace pels
} // namespace openpower
