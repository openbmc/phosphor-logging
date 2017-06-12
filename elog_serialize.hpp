#pragma once

#include <string>
#include <vector>
#include <experimental/filesystem>
#include "elog_entry.hpp"
#include "config.h"

namespace phosphor
{
namespace logging
{

namespace fs = std::experimental::filesystem;

/** @brief Function required by Cereal to perform serialization.
 *  @tparam Archive - Cereal archive type (binary in our case).
 *  @param[in] a - reference to Cereal archive.
 *  @param[in] e - const reference to error entry.
 */
template<class Archive>
void save(Archive& a, const Entry& e)
{
    a(e.id(), e.severity(), e.timestamp(),
      e.message(), e.additionalData(), e.associations(), e.resolved());
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
    e.resolved(resolved);
    e.associations(associations);
}

/** @brief Serialize and persist error d-bus object
 *  @param[in] a - const reference to error entry.
 *  @param[in] dir - pathname of directory where the serialized error will
 *                   be placed.
 *  @return fs::path - pathname of persisted error file
 */
fs::path serialize(const Entry& e,
                   const fs::path& dir = fs::path(ERRLOG_PERSIST_PATH));

/** @brief Deserialze a persisted error into a d-bus object
 *  @param[in] path - pathname of persisted error file
 *  @param[in] e - reference to error object which is the target of
 *             deserialization.
 *  @return bool - true if the deserialization was successful, false otherwise.
 */
bool deserialize(const fs::path& path, Entry& e);

} // namespace logging
} // namespace phosphor
