#pragma once

#include <filesystem>
#include <span>
#include <string_view>

namespace phosphor::logging::plugin::utils
{

/**
 * @brief Reference to a persisted artifact.
 *
 * Artifacts are stored in phosphor-logging managed storage and
 * referenced by plugin descriptors and runtime plugins.
 *
 * An empty path indicates that persistence failed or the artifact
 * is otherwise unavailable.
 */
struct ArtifactRef
{
    /** Persisted artifact location. */
    std::filesystem::path path;

    /**
     * @brief Check whether the reference is valid.
     *
     * @return true if the artifact reference is valid.
     */
    explicit operator bool() const noexcept
    {
        return !path.empty();
    }
};

/**
 * @brief Default location used to persist artifacts.
 */
inline const std::filesystem::path defaultArtifactDirectory{
    "/var/lib/phosphor-logging/artifacts"};

/**
 * @brief Generic artifact infrastructure payload limit.
 *
 * This limit acts as a safeguard against accidental storage
 * exhaustion caused by malformed requests or future plugin
 * implementations.
 *
 * Individual plugins may enforce stricter limits.
 */
inline constexpr std::size_t maxArtifactPayloadSize = 16 * 1024 * 1024;

/**
 * @brief Persist an artifact payload.
 *
 * Stores the supplied payload in phosphor-logging managed
 * storage and returns a reference to the persisted artifact.
 *
 * Artifact allocation and naming are implementation details
 * intentionally hidden behind this interface.
 *
 * Failures are logged and result in an empty ArtifactRef.
 *
 * @param[in] data Artifact payload.
 * @param[in] prefix Artifact filename prefix.
 * @param[in] root Artifact storage directory.
 *
 * @return Reference to the persisted artifact. An empty
 *         reference indicates failure.
 */
ArtifactRef persistArtifact(
    std::span<const uint8_t> data, std::string_view prefix,
    const std::filesystem::path& root = defaultArtifactDirectory);

/**
 * @brief Open a persisted artifact.
 *
 * Opens an artifact for read-only access.
 *
 * Failures are logged and result in -1.
 *
 * @param[in] artifact Artifact reference.
 *
 * @return Read-only file descriptor on success, otherwise -1.
 */
int openArtifact(const ArtifactRef& artifact);

/**
 * @brief Remove a persisted artifact.
 *
 * Failures are logged and otherwise ignored.
 *
 * @param[in] artifact Artifact reference.
 */
void removeArtifact(const ArtifactRef& artifact);

} // namespace phosphor::logging::plugin::utils
