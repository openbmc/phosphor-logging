extern "C" {
#include <libpdbg.h>
}

#include "fapi_data_process.hpp"

#include <attributes_info.H>
#include <fmt/format.h>
#include <libphal.H>
#include <phal_exception.H>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <list>
#include <map>
#include <phosphor-logging/elog.hpp>
#include <sstream>
#include <string>

namespace openpower
{
namespace pels
{
namespace phal
{

using namespace phosphor::logging;
using namespace openpower::phal::exception;

/**
 * Used to pass buffer to pdbg callback api to get required target
 * data (attributes) based on given data (attribute).
 */
struct TargetInfo
{
    ATTR_PHYS_BIN_PATH_Type physBinPath;
    ATTR_LOCATION_CODE_Type locationCode;
    ATTR_PHYS_DEV_PATH_Type physDevPath;
    ATTR_MRU_ID_Type mruId;

    bool deconfigure;

    TargetInfo()
    {
        memset(&physBinPath, '\0', sizeof(physBinPath));
        memset(&locationCode, '\0', sizeof(locationCode));
        memset(&physDevPath, '\0', sizeof(physDevPath));
        mruId = 0;
        deconfigure = false;
    }
};

/**
 * Used to return in callback function which are used to get
 * physical path value and it binary format value.
 *
 * The value for constexpr defined based on pdbg_target_traverse function usage.
 */
constexpr int continueTgtTraversal = 0;
constexpr int requireAttrFound = 1;
constexpr int requireAttrNotFound = 2;

/**
 * @brief Used to get target location code from phal device tree
 *
 * @param[in] target current device tree target
 * @param[out] appPrivData used for accessing|storing from|to application
 *
 * @return 0 to continue traverse, non-zero to stop traverse
 */
int pdbgCallbackToGetTgtReqAttrsVal(struct pdbg_target* target,
                                    void* appPrivData)
{
    using namespace openpower::phal::pdbg;

    TargetInfo* targetInfo = static_cast<TargetInfo*>(appPrivData);

    ATTR_PHYS_BIN_PATH_Type physBinPath;
    /**
     * TODO: Issue: phal/pdata#16
     * Should not use direct pdbg api to read attribute. Need to use DT_GET_PROP
     * macro for bmc app's and this will call libdt-api api but, it will print
     * "pdbg_target_get_attribute failed" trace if attribute is not found and
     * this callback will call recursively by using pdbg_target_traverse() until
     * find expected attribute based on return code from this callback. Because,
     * need to do target iteration to get actual attribute (ATTR_PHYS_BIN_PATH)
     * value when device tree target info doesn't know to read attribute from
     * device tree. So, Due to this error trace user will get confusion while
     * looking traces. Hence using pdbg api to avoid trace until libdt-api
     * provides log level setup.
     */
    if (!pdbg_target_get_attribute(
            target, "ATTR_PHYS_BIN_PATH",
            std::stoi(dtAttr::fapi2::ATTR_PHYS_BIN_PATH_Spec),
            dtAttr::fapi2::ATTR_PHYS_BIN_PATH_ElementCount, physBinPath))
    {
        return continueTgtTraversal;
    }

    if (std::memcmp(physBinPath, targetInfo->physBinPath,
                    sizeof(physBinPath)) != 0)
    {
        return continueTgtTraversal;
    }

    // Found Target, now collect the required attributes associated to the
    // target. Incase of any attribute read failure, initialize the data with
    // default value.

    try
    {
        // Get location code information
        openpower::phal::pdbg::getLocationCode(target,
                                               targetInfo->locationCode);
    }
    catch (const std::exception& e)
    {
        // log message and continue with default data
        log<level::ERR>(fmt::format("getLocationCode({}): Exception({})",
                                    pdbg_target_path(target), e.what())
                            .c_str());
    }

    if (DT_GET_PROP(ATTR_PHYS_DEV_PATH, target, targetInfo->physDevPath))
    {
        log<level::ERR>(
            fmt::format("Could not read({}) PHYS_DEV_PATH attribute",
                        pdbg_target_path(target))
                .c_str());
    }

    if (DT_GET_PROP(ATTR_MRU_ID, target, targetInfo->mruId))
    {
        log<level::ERR>(fmt::format("Could not read({}) ATTR_MRU_ID attribute",
                                    pdbg_target_path(target))
                            .c_str());
    }

    return requireAttrFound;
}

/**
 * @brief Used to get target info (attributes data)
 *
 * To get target required attributes value using another attribute value
 * ("PHYS_BIN_PATH" which is present in same target attributes list) by using
 * "ipdbg_target_traverse" api because, here we have attribute value only and
 * doesn't have respective device tree target info to get required attributes
 * values from it attributes list.
 *
 * @param[in] physBinPath to pass PHYS_BIN_PATH value
 * @param[out] targetInfo to pas buufer to fill with required attributes
 *
 * @return true on success otherwise false
 */
bool getTgtReqAttrsVal(const std::vector<uint8_t>& physBinPath,
                       TargetInfo& targetInfo)
{
    std::memcpy(&targetInfo.physBinPath, physBinPath.data(),
                sizeof(targetInfo.physBinPath));

    int ret = pdbg_target_traverse(NULL, pdbgCallbackToGetTgtReqAttrsVal,
                                   &targetInfo);
    if (ret == 0)
    {
        log<level::ERR>(fmt::format("Given ATTR_PHYS_BIN_PATH value({}) "
                                    "not found in phal device tree",
                                    targetInfo.physBinPath)
                            .c_str());
        return false;
    }
    else if (ret == requireAttrNotFound)
    {
        return false;
    }

    return true;
}

/**
 * @brief GET PEL priority from pHAL priority
 *
 * The pHAL callout priority is in different format than PEL format
 * so, this api is used to return current phal supported priority into
 * PEL expected format.
 *
 * @param[in] phalPriority used to pass phal priority format string
 *
 * @return pel priority format string else empty if failure
 *
 * @note For "NONE" returning "L" (LOW)
 */
static std::string getPelPriority(const std::string& phalPriority)
{
    const std::map<std::string, std::string> priorityMap = {
        {"HIGH", "H"}, {"MEDIUM", "M"}, {"LOW", "L"}, {"NONE", "L"}};

    auto it = priorityMap.find(phalPriority);
    if (it == priorityMap.end())
    {
        log<level::ERR>(fmt::format("Unsupported phal priority({}) is given "
                                    "to get pel priority format",
                                    phalPriority)
                            .c_str());
        return "H";
    }

    return it->second;
}

void convertFAPItoPELformat(FFDC& ffdc, json& pelJSONFmtCalloutDataList,
                            FFDCData& ffdcUserData)
{
    if (ffdc.ffdc_type == FFDC_TYPE_HWP)
    {
        // Adding hardware procedures return code details
        ffdcUserData.emplace_back("HWP_RC", ffdc.hwp_errorinfo.rc);
        ffdcUserData.emplace_back("HWP_RC_DESC", ffdc.hwp_errorinfo.rc_desc);

        // Adding hardware procedures required ffdc data for debug
        for_each(
            ffdc.hwp_errorinfo.ffdcs_data.begin(),
            ffdc.hwp_errorinfo.ffdcs_data.end(),
            [&ffdcUserData](std::pair<std::string, std::string>& ele) -> void {
                std::string keyWithPrefix("HWP_FFDC_");
                keyWithPrefix.append(ele.first);

                ffdcUserData.emplace_back(keyWithPrefix, ele.second);
            });

        // Adding hardware callout details
        int calloutCount = 0;
        for_each(
            ffdc.hwp_errorinfo.hwcallouts.begin(),
            ffdc.hwp_errorinfo.hwcallouts.end(),
            [&ffdcUserData, &calloutCount,
             &pelJSONFmtCalloutDataList](const HWCallout& hwCallout) -> void {
                calloutCount++;
                std::stringstream keyPrefix;
                keyPrefix << "HWP_HW_CO_" << std::setfill('0') << std::setw(2)
                          << calloutCount << "_";

                ffdcUserData.emplace_back(
                    std::string(keyPrefix.str()).append("HW_ID"),
                    hwCallout.hwid);

                ffdcUserData.emplace_back(
                    std::string(keyPrefix.str()).append("PRIORITY"),
                    hwCallout.callout_priority);

                phal::TargetInfo targetInfo;
                phal::getTgtReqAttrsVal(hwCallout.target_entity_path,
                                        targetInfo);

                std::string locationCode = std::string(targetInfo.locationCode);
                ffdcUserData.emplace_back(
                    std::string(keyPrefix.str()).append("LOC_CODE"),
                    locationCode);

                std::string physPath = std::string(targetInfo.physDevPath);
                ffdcUserData.emplace_back(
                    std::string(keyPrefix.str()).append("PHYS_PATH"), physPath);

                ffdcUserData.emplace_back(
                    std::string(keyPrefix.str()).append("CLK_POS"),
                    std::to_string(hwCallout.clkPos));

                json jsonCalloutData;
                jsonCalloutData["LocationCode"] = locationCode;
                std::string pelPriority =
                    getPelPriority(hwCallout.callout_priority);
                jsonCalloutData["Priority"] = pelPriority;

                if (targetInfo.mruId != 0)
                {
                    jsonCalloutData["MRUs"] = json::array({
                        {{"ID", targetInfo.mruId}, {"Priority", pelPriority}},
                    });
                }

                pelJSONFmtCalloutDataList.emplace_back(jsonCalloutData);
            });

        // Adding CDG (callout, deconfigure and guard) targets details
        calloutCount = 0;
        for_each(
            ffdc.hwp_errorinfo.cdg_targets.begin(),
            ffdc.hwp_errorinfo.cdg_targets.end(),
            [&ffdcUserData, &calloutCount,
             &pelJSONFmtCalloutDataList](const CDG_Target& cdg_tgt) -> void {
                calloutCount++;
                std::stringstream keyPrefix;
                keyPrefix << "HWP_CDG_TGT_" << std::setfill('0') << std::setw(2)
                          << calloutCount << "_";

                phal::TargetInfo targetInfo;
                targetInfo.deconfigure = cdg_tgt.deconfigure;

                phal::getTgtReqAttrsVal(cdg_tgt.target_entity_path, targetInfo);

                std::string locationCode = std::string(targetInfo.locationCode);
                ffdcUserData.emplace_back(
                    std::string(keyPrefix.str()).append("LOC_CODE"),
                    locationCode);
                std::string physPath = std::string(targetInfo.physDevPath);
                ffdcUserData.emplace_back(
                    std::string(keyPrefix.str()).append("PHYS_PATH"), physPath);

                ffdcUserData.emplace_back(
                    std::string(keyPrefix.str()).append("CO_REQ"),
                    (cdg_tgt.callout == true ? "true" : "false"));

                ffdcUserData.emplace_back(
                    std::string(keyPrefix.str()).append("CO_PRIORITY"),
                    cdg_tgt.callout_priority);

                ffdcUserData.emplace_back(
                    std::string(keyPrefix.str()).append("DECONF_REQ"),
                    (cdg_tgt.deconfigure == true ? "true" : "false"));

                ffdcUserData.emplace_back(
                    std::string(keyPrefix.str()).append("GUARD_REQ"),
                    (cdg_tgt.guard == true ? "true" : "false"));

                ffdcUserData.emplace_back(
                    std::string(keyPrefix.str()).append("GUARD_TYPE"),
                    cdg_tgt.guard_type);

                json jsonCalloutData;
                jsonCalloutData["LocationCode"] = locationCode;
                std::string pelPriority =
                    getPelPriority(cdg_tgt.callout_priority);
                jsonCalloutData["Priority"] = pelPriority;

                if (targetInfo.mruId != 0)
                {
                    jsonCalloutData["MRUs"] = json::array({
                        {{"ID", targetInfo.mruId}, {"Priority", pelPriority}},
                    });
                }
                jsonCalloutData["Deconfigured"] = cdg_tgt.deconfigure;
                jsonCalloutData["Guarded"] = cdg_tgt.guard;
                jsonCalloutData["GuardType"] = cdg_tgt.guard_type;
                jsonCalloutData["EntityPath"] = cdg_tgt.target_entity_path;

                pelJSONFmtCalloutDataList.emplace_back(jsonCalloutData);
            });

        // Adding procedure callout
        calloutCount = 0;
        for_each(ffdc.hwp_errorinfo.procedures_callout.begin(),
                 ffdc.hwp_errorinfo.procedures_callout.end(),
                 [&ffdcUserData, &calloutCount, &pelJSONFmtCalloutDataList](
                     const ProcedureCallout& procCallout) -> void {
                     calloutCount++;
                     std::stringstream keyPrefix;
                     keyPrefix << "HWP_PROC_CO_" << std::setfill('0')
                               << std::setw(2) << calloutCount << "_";
                     ffdcUserData.emplace_back(
                         std::string(keyPrefix.str()).append("PRIORITY"),
                         procCallout.callout_priority);
                     ffdcUserData.emplace_back(
                         std::string(keyPrefix.str()).append("MAINT_PROCEDURE"),
                         procCallout.proc_callout);
                     json jsonCalloutData;
                     jsonCalloutData["Procedure"] = procCallout.proc_callout;
                     std::string pelPriority =
                         getPelPriority(procCallout.callout_priority);
                     jsonCalloutData["Priority"] = pelPriority;
                     pelJSONFmtCalloutDataList.emplace_back(jsonCalloutData);
                 });
    }
    else if ((ffdc.ffdc_type != FFDC_TYPE_NONE) &&
             (ffdc.ffdc_type != FFDC_TYPE_UNSUPPORTED))
    {
        log<level::ERR>(fmt::format("Unsupported phal FFDC type to create PEL. "
                                    "MSG: {}",
                                    ffdc.message)
                            .c_str());
    }
}

} // namespace phal
} // namespace pels
} // namespace openpower
