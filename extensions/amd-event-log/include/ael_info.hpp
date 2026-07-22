#pragma once

#include "amd_afid_utils.hpp"
#include "extensions.hpp"

#include <phosphor-logging/log.hpp>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace phosphor::logging::extensions::ael
{

/**
 * @brief AEL schema constants.
 */
namespace constants
{

/** Current AEL schema version. */
constexpr std::string_view SchemaVersion = "1.0";

} // namespace constants

/**
 * @brief AEL schema field names.
 */
namespace fields
{

/** AEL schema version. */
constexpr std::string_view Version = "AEL.VERSION";

/** AMD Field Identifier. */
constexpr std::string_view AFID = "AEL.AFID";

/** Associated FRU inventory paths. */
constexpr std::string_view FruList = "AEL.FRU_LIST";

} // namespace fields

/**
 * @brief AMD Event Log schema representation.
 *
 * Represents the complete set of AEL properties exported
 * through the AMD OEM extension.
 */
struct AELInfo
{
    /**
     * AEL schema version.
     */
    std::string version{std::string(constants::SchemaVersion)};

    /**
     * AMD Field Identifier.
     */
    uint64_t afid{};

    /**
     * Associated inventory object paths.
     */
    std::vector<std::string> fruList;
};

/**
 * @class AELInfoProvider
 *
 * @brief Produces AMD Event Log metadata for a phosphor-logging
 *        event.
 *
 * Architecture:
 *
 *     Logging Event
 *            |
 *            v
 *       AdditionalData
 *            |
 *            v
 *        lookupAFID()
 *            |
 *            v
 *          AELInfo
 *
 * Responsibilities:
 *
 *   - Populate AEL.VERSION
 *   - Resolve AEL.AFID
 *   - Populate AEL.FRU_LIST
 *
 * Event producers are not required to emit AEL-specific
 * metadata. AEL properties are derived from the event
 * message identifier, event AdditionalData, and AFID
 * lookup information.
 */
class AELInfoProvider
{
  public:
    /**
     * @brief Construct an AEL information provider.
     *
     * @param[in] msg Event message.
     * @param[in] level Event severity.
     * @param[in] additionalData Event additional data.
     * @param[in] associations Event associations.
     */
    AELInfoProvider(const std::string& msg, Entry::Level level,
                    const AdditionalDataVec& additionalData,
                    const AssociationList& associations);

    ~AELInfoProvider() = default;

    AELInfoProvider(const AELInfoProvider&) = default;
    AELInfoProvider& operator=(const AELInfoProvider&) = delete;

    AELInfoProvider(AELInfoProvider&&) = default;
    AELInfoProvider& operator=(AELInfoProvider&&) = delete;

    /**
     * @brief Generate AEL metadata for the event.
     *
     * @return Populated AEL information.
     */
    AELInfo get() const;

  private:
    /**
     * @brief Convert phosphor-logging severity into
     *        an AEL classification.
     *
     * @return Error, Warning, or Info.
     */
    std::string getDefaultType() const;

    const std::string& msg;
    Entry::Level level;
    const AdditionalDataVec& additionalData;
    const AssociationList& associations;
};

} // namespace phosphor::logging::extensions::ael
