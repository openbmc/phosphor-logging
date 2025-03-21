#include "config.h"

#include "elog_serialize.hpp"

#include <cereal/archives/binary.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/vector.hpp>
#include <phosphor-logging/lg2.hpp>

#include <fstream>

// Register class version
// From cereal documentation;
// "This macro should be placed at global scope"
CEREAL_CLASS_VERSION(phosphor::logging::Entry, CLASS_VERSION)

namespace phosphor
{
namespace logging
{

/** @brief Function required by Cereal to perform serialization.
 *  @tparam Archive - Cereal archive type (binary in our case).
 *  @param[in] a       - reference to Cereal archive.
 *  @param[in] e       - const reference to error entry.
 *  @param[in] version - Class version that enables handling
 *                       a serialized data across code levels
 */
template <class Archive>
void save(Archive& a, const Entry& e, const std::uint32_t /*version*/)
{
    a(e.id(), e.severity(), e.timestamp(), e.message(), e.additionalData(),
      e.associations(), e.resolved(), e.version(), e.updateTimestamp(),
      e.eventId(), e.resolution());
}

/** @brief Function required by Cereal to perform deserialization.
 *  @tparam Archive - Cereal archive type (binary in our case).
 *  @param[in] a       - reference to Cereal archive.
 *  @param[in] e       - reference to error entry.
 *  @param[in] version - Class version that enables handling
 *                       a serialized data across code levels
 */
template <class Archive>
void load(Archive& a, Entry& e, const std::uint32_t version)
{
    using namespace sdbusplus::server::xyz::openbmc_project::logging;

    uint32_t id{};
    Entry::Level severity{};
    uint64_t timestamp{};
    std::string message{};
    std::map<std::string, std::string> additionalData{};
    bool resolved{};
    AssociationList associations{};
    std::string fwVersion{};
    uint64_t updateTimestamp{};
    std::string eventId{};
    std::string resolution{};

    if (version < std::stoul(FIRST_CEREAL_CLASS_VERSION_WITH_FWLEVEL))
    {
        std::vector<std::string> additionalData_old{};
        a(id, severity, timestamp, message, additionalData_old, associations,
          resolved);
        updateTimestamp = timestamp;
        additionalData = util::additional_data::parse(additionalData_old);
    }
    else if (version < std::stoul(FIRST_CEREAL_CLASS_VERSION_WITH_UPDATE_TS))
    {
        std::vector<std::string> additionalData_old{};
        a(id, severity, timestamp, message, additionalData_old, associations,
          resolved, fwVersion);
        updateTimestamp = timestamp;
        additionalData = util::additional_data::parse(additionalData_old);
    }
    else if (version < std::stoul(FIRST_CEREAL_CLASS_VERSION_WITH_EVENTID))
    {
        std::vector<std::string> additionalData_old{};
        a(id, severity, timestamp, message, additionalData_old, associations,
          resolved, fwVersion, updateTimestamp);
        additionalData = util::additional_data::parse(additionalData_old);
    }
    else if (version < std::stoul(FIRST_CEREAL_CLASS_VERSION_WITH_RESOLUTION))
    {
        std::vector<std::string> additionalData_old{};
        a(id, severity, timestamp, message, additionalData_old, associations,
          resolved, fwVersion, updateTimestamp, eventId);
        additionalData = util::additional_data::parse(additionalData_old);
    }
    else if (version <
             std::stoul(FIRST_CEREAL_CLASS_VERSION_WITH_METADATA_DICT))
    {
        std::vector<std::string> additionalData_old{};
        a(id, severity, timestamp, message, additionalData_old, associations,
          resolved, fwVersion, updateTimestamp, eventId, resolution);
        additionalData = util::additional_data::parse(additionalData_old);
    }
    else
    {
        a(id, severity, timestamp, message, additionalData, associations,
          resolved, fwVersion, updateTimestamp, eventId, resolution);
    }

    e.id(id, true);
    e.severity(severity, true);
    e.timestamp(timestamp, true);
    e.message(message, true);
    e.additionalData(additionalData, true);
    e.sdbusplus::server::xyz::openbmc_project::logging::Entry::resolved(
        resolved, true);
    e.associations(associations, true);
    e.version(fwVersion, true);
    e.purpose(sdbusplus::server::xyz::openbmc_project::software::Version::
                  VersionPurpose::BMC,
              true);
    e.updateTimestamp(updateTimestamp, true);
    e.eventId(eventId, true);
    e.resolution(resolution, true);
}

fs::path getEntrySerializePath(uint32_t id, const fs::path& dir)
{
    return dir / std::to_string(id);
}

fs::path serialize(const Entry& e, const fs::path& dir)
{
    auto path = getEntrySerializePath(e.id(), dir);
    std::ofstream os(path.c_str(), std::ios::binary);
    cereal::BinaryOutputArchive oarchive(os);
    oarchive(e);
    return path;
}

bool deserialize(const fs::path& path, Entry& e)
{
    try
    {
        if (fs::exists(path))
        {
            std::ifstream is(path.c_str(), std::ios::in | std::ios::binary);
            cereal::BinaryInputArchive iarchive(is);
            iarchive(e);
            return true;
        }
        return false;
    }
    catch (const std::exception& ex)
    {
        lg2::error("Failed restoring {PATH}: {EXCEPTION}", "PATH", path,
                   "EXCEPTION", ex);
        // Save it for later debug. Just write over any previous ones.
        auto saveDir = paths::error().parent_path();
        fs::rename(path, saveDir / "corrupt_error");
        return false;
    }
}

} // namespace logging
} // namespace phosphor
