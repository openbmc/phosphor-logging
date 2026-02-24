#pragma once

#include <libekb.H>

#include <nlohmann/json.hpp>

#include <cstdarg>

namespace openpower
{
namespace pels
{
namespace phal
{

using json = nlohmann::json;
using FFDCData = std::vector<std::pair<std::string, std::string>>;

/**
 * @brief Used to convert  processed SBE FFDC FAPI format packets into
 *        PEL expected format.
 *
 * @param[in]  ffdc FFDC data captured by the HWP
 * @param[out] pelJSONFmtCalloutDataList used to store collected callout
 *             data into pel expected format
 * @param[out] ffdcUserData used to store additional ffdc user data to
 *             provided by the SBE FFDC packet.
 *
 * @return NULL
 *
 */
void convertFAPItoPELformat(FFDC& ffdc, json& pelJSONFmtCalloutDataList,
                            FFDCData& ffdcUserData);
} // namespace phal
} // namespace pels
} // namespace openpower
