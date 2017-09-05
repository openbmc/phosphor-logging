#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>
#include "elog_serialize.hpp"

namespace phosphor
{
namespace logging
{

/** @brief Function required by Cereal to perform serialization.
 *  @tparam Archive - Cereal archive type (binary in our case).
 *  @param[in] a - reference to Cereal archive.
 *  @param[in] e - const reference to error entry.
 */
template<class Archive>
void save(Archive& a, const Entry& e)
{
    a(e.id(), e.severity(), e.timestamp(),
      e.message(), e.additionalData(),
      e.sdbusplus::org::openbmc::server::Associations::associations(),
      e.resolved());
}

/** @brief Function required by Cereal to perform deserialization.
 *  @tparam Archive - Cereal archive type (binary in our case).
 *  @param[in] a - reference to Cereal archive.
 *  @param[in] e - reference to error entry.
 */
template<class Archive>
void load(Archive& a, Entry& e)
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
    if (fs::exists(path))
    {
        std::ifstream is(path.c_str(), std::ios::in | std::ios::binary);
        cereal::BinaryInputArchive iarchive(is);
        iarchive(e);
        return true;
    }
    return false;
}

} // namespace logging
} // namespace phosphor
