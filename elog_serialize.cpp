#include "config.h"

#include "elog_serialize.hpp"

#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>
#include <phosphor-logging/log.hpp>

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
    using namespace sdbusplus::xyz::openbmc_project::Logging::server;

    uint32_t id{};
    Entry::Level severity{};
    uint64_t timestamp{};
    std::string message{};
    std::vector<std::string> additionalData{};
    bool resolved{};
    AssociationList associations{};
    std::string fwVersion{};
    uint64_t updateTimestamp{};
    std::string eventId{};
    std::string resolution{};

    if (version < std::stoul(FIRST_CEREAL_CLASS_VERSION_WITH_FWLEVEL))
    {
        a(id, severity, timestamp, message, additionalData, associations,
          resolved);
        updateTimestamp = timestamp;
    }
    else if (version < std::stoul(FIRST_CEREAL_CLASS_VERSION_WITH_UPDATE_TS))
    {
        a(id, severity, timestamp, message, additionalData, associations,
          resolved, fwVersion);
        updateTimestamp = timestamp;
    }
    else if (version < std::stoul(FIRST_CEREAL_CLASS_VERSION_WITH_EVENTID))
    {
        a(id, severity, timestamp, message, additionalData, associations,
          resolved, fwVersion, updateTimestamp);
    }
    else if (version < std::stoul(FIRST_CEREAL_CLASS_VERSION_WITH_RESOLUTION))
    {
        a(id, severity, timestamp, message, additionalData, associations,
          resolved, fwVersion, updateTimestamp, eventId);
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
    e.sdbusplus::xyz::openbmc_project::Logging::server::Entry::resolved(
        resolved, true);
    e.associations(associations, true);
    e.version(fwVersion, true);
    e.purpose(sdbusplus::xyz::openbmc_project::Software::server::Version::
                  VersionPurpose::BMC,
              true);
    e.updateTimestamp(updateTimestamp, true);
    e.eventId(eventId, true);
    e.resolution(resolution, true);
}

fs::path serialize(const Entry& e, const fs::path& dir)
{
    auto path = dir / std::to_string(e.id());
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
    catch (const cereal::Exception& e)
    {
        log<level::ERR>(e.what());
        fs::remove(path);
        return false;
    }
    catch (const std::length_error& e)
    {
        // Running into: USCiLab/cereal#192
        // This may be indicating some other issue in the
        // way vector may have been used inside the logging.
        // possibly associations ??. But handling it here for
        // now since we are anyway tossing the log
        // TODO: openbmc/phosphor-logging#8
        log<level::ERR>(e.what());
        fs::remove(path);
        return false;
    }
}

} // namespace logging
} // namespace phosphor
