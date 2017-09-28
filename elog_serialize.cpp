#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>

#include "elog_serialize.hpp"
#include <phosphor-logging/log.hpp>

// Class version
// From cereal documentation;
// "This macro should be placed at global scope"
CEREAL_CLASS_VERSION(phosphor::logging::Entry,
                     phosphor::logging::Entry::getVersion());
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
template<class Archive>
void save(Archive& a, const Entry& e, const std::uint32_t version)
{
    a(e.id(), e.severity(), e.timestamp(),
      e.message(), e.additionalData(), e.associations(), e.resolved());
}

/** @brief Function required by Cereal to perform deserialization.
 *  @tparam Archive - Cereal archive type (binary in our case).
 *  @param[in] a       - reference to Cereal archive.
 *  @param[in] e       - reference to error entry.
 *  @param[in] version - Class version that enables handling
 *                       a serialized data across code levels
 */
template<class Archive>
void load(Archive& a, Entry& e, const std::uint32_t version)
{
    using namespace
        sdbusplus::xyz::openbmc_project::Logging::server;

    uint32_t id{};
    Entry::Level severity{};
    uint64_t timestamp{};
    std::string message{};
    std::vector<std::string> additionalData{};
    bool resolved{};
    AssociationList associations{};

    a(id, severity, timestamp, message,
      additionalData, associations, resolved);

    e.id(id);
    e.severity(severity);
    e.timestamp(timestamp);
    e.message(message);
    e.additionalData(additionalData);
    e.sdbusplus::xyz::openbmc_project::
        Logging::server::Entry::resolved(resolved);
    e.associations(associations);
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
    catch(cereal::Exception& e)
    {
        log<level::ERR>(e.what());
        fs::remove(path);
        return false;
    }
}

} // namespace logging
} // namespace phosphor
