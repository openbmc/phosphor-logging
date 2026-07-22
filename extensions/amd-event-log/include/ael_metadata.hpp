#pragma once

#include <string_view>

namespace phosphor::logging::extensions::ael
{

namespace constants
{

constexpr std::string_view SchemaVersion = "1.0";

} // namespace constants

namespace fields
{

constexpr std::string_view Version = "AEL.VERSION";
constexpr std::string_view AFID = "AEL.AFID";
constexpr std::string_view FruList = "AEL.FRU_LIST";
constexpr std::string_view RackId = "AEL.RACK_ID";

/**
 * @brief Pre-rendered AMD OEM Redfish payload.
 *
 * When present, the payload is treated as the
 * authoritative Redfish representation and no
 * projection is performed.
 */
constexpr std::string_view Redfish = "AEL.REDFISH";

} // namespace fields

} // namespace phosphor::logging::extensions::ael
