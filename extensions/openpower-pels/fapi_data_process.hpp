#pragma once

#include <libekb.H>

#include <cstdarg>
#include <nlohmann/json.hpp>

namespace openpower
{
namespace pels
{
namespace sbe
{

using json = nlohmann::json;
using FFDCData = std::vector<std::pair<std::string, std::string>>;

/**
 * @brief Used to convert  processed SBE FFDC FAPI format packets into
 *        PEL expected format.
 *
 * @param[out] pelJSONFmtCalloutDataList used to store collected callout
 *             data into pel expected format
 * @param[out] pelAdditionalData used to store additional data to debug
 *             when getting failure
 *
 * @return NULL
 *
 */
void convertFAPItoPELformat(FFDC& ffdc, json& pelJSONFmtCalloutDataList,
                            FFDCData& pelAdditionalData);
} // namespace sbe
} // namespace pels
} // namespace openpower
