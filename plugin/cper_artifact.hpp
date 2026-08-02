#pragma once

#include <filesystem>
#include <string>

namespace phosphor::logging::plugin::cper
{

/**
 * @brief Default location for persisted CPER artifacts.
 */
inline const std::filesystem::path defaultArtifactDirectory{
    "/var/lib/phosphor-logging/cper"};

/**
 * @brief Buffer size used when copying CPER artifacts.
 */
inline constexpr std::size_t artifactCopyBufferSize = 64 * 1024;

/**
 * @brief Return the artifact path associated with a log entry.
 *
 * The artifact filename is derived from the log entry object
 * path and placed under the supplied root directory.
 *
 * Example:
 *
 *   /xyz/openbmc_project/logging/entry/42
 *
 * becomes:
 *
 *   <root>/42.bin
 *
 * @param[in] objectPath Log entry object path.
 * @param[in] root Artifact storage root directory.
 *
 * @return Path of the associated CPER artifact.
 */
std::filesystem::path artifactPath(
    const std::string& objectPath,
    const std::filesystem::path& root = defaultArtifactDirectory);

/**
 * @brief Persist CPER artifact data.
 *
 * Copies CPER data from the supplied file descriptor into
 * phosphor-logging owned storage.
 *
 * Artifact transport is optional. A negative file descriptor
 * indicates that no artifact was supplied.
 *
 * @param[in] objectPath Log entry object path.
 * @param[in] fd Source CPER file descriptor.
 * @param[in] root Artifact storage root directory.
 *
 * @return Path to the persisted artifact on success, or an
 *         empty path if no artifact was supplied or persistence
 *         failed.
 */
std::filesystem::path persistArtifact(
    const std::string& objectPath, int fd,
    const std::filesystem::path& root = defaultArtifactDirectory);

} // namespace phosphor::logging::plugin::cper
