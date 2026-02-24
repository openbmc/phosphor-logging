extern "C"
{
#include <libpdbg.h>
}

#include "fapi_data_process.hpp"

#include <attributes_info.H>
#include <libphal.H>
#include <phal_exception.H>

#include <phosphor-logging/elog.hpp>
#include <phosphor-logging/lg2.hpp>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <format>
#include <iomanip>
#include <list>
#include <map>
#include <sstream>
#include <string>

namespace openpower
{
namespace pels
{
namespace phal
{

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
    // target. In case of any attribute read failure, initialize the data with
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
        lg2::error("getLocationCode({TARGET_PATH}): Exception({EXCEPTION})",
                   "TARGET_PATH", pdbg_target_path(target), "EXCEPTION", e);
    }

    if (DT_GET_PROP(ATTR_PHYS_DEV_PATH, target, targetInfo->physDevPath))
    {
        lg2::error("Could not read({TARGET_PATH}) PHYS_DEV_PATH attribute",
                   "TARGET_PATH", pdbg_target_path(target));
    }

    if (DT_GET_PROP(ATTR_MRU_ID, target, targetInfo->mruId))
    {
        lg2::error("Could not read({TARGET_PATH}) ATTR_MRU_ID attribute",
                   "TARGET_PATH", pdbg_target_path(target));
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

    int ret = pdbg_target_traverse(nullptr, pdbgCallbackToGetTgtReqAttrsVal,
                                   &targetInfo);
    if (ret == 0)
    {
        std::string fmt;
        for (auto value : targetInfo.physBinPath)
        {
            fmt += std::format("{:02X} ", value);
        }

        lg2::error(
            "Given ATTR_PHYS_BIN_PATH value {ATTR_PHYS_BIN_PATH} not found in phal device tree",
            "ATTR_PHYS_BIN_PATH", fmt);
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
        lg2::error(
            "Unsupported phal priority({PHAL_PRIORITY}) is given to get pel priority format",
            "PHAL_PRIORITY", phalPriority);
        return "H";
    }

    return it->second;
}

/**
 * @brief addPlanarCallout
 *
 * This function will add a json for planar callout in the input json list.
 * The caller can pass this json list into createErrorPEL to apply the callout.
 *
 * @param[in,out] jsonCalloutDataList - json list where callout json will be
 *                  emplaced
 * @param[in] priority - string indicating priority.
 */
static void addPlanarCallout(json& jsonCalloutDataList,
                             const std::string& priority)
{
    json jsonCalloutData;

    // Inventory path for planar
    jsonCalloutData["InventoryPath"] =
        "/xyz/openbmc_project/inventory/system/chassis/motherboard";
    jsonCalloutData["Deconfigured"] = false;
    jsonCalloutData["Guarded"] = false;
    jsonCalloutData["Priority"] = priority;

    jsonCalloutDataList.emplace_back(jsonCalloutData);
}

/**
 * @brief processClockInfoErrorHelper
 *
 * Creates informational PEL for spare clock failure
 *
 * @param[in]  ffdc FFDC data captured by the HWP
 * @param[out] pelJSONFmtCalloutDataList used to store collected callout
 *            data into pel expected format
 * @param[out] ffdcUserData used to store additional ffdc user data to
 *              provided by the SBE FFDC packet.
 *
 * @return NULL
 *
 **/
void processClockInfoErrorHelper(
    const FFDC& ffdc, json& pelJSONFmtCalloutDataList, FFDCData& ffdcUserData)
{
    lg2::info("processClockInfoErrorHelper: FFDC Message[{FFDC_MSG}]",
              "FFDC_MSG", ffdc.message);

    // Adding hardware procedures return code details
    ffdcUserData.emplace_back("HWP_RC", ffdc.hwp_errorinfo.rc);
    ffdcUserData.emplace_back("HWP_RC_DESC", ffdc.hwp_errorinfo.rc_desc);

    // Adding hardware procedures required ffdc data for debug
    for_each(ffdc.hwp_errorinfo.ffdcs_data.cbegin(),
             ffdc.hwp_errorinfo.ffdcs_data.cend(),
             [&ffdcUserData](
                 const std::pair<std::string, std::string>& ele) -> void {
                 std::string keyWithPrefix("HWP_FFDC_");
                 keyWithPrefix.append(ele.first);

                 ffdcUserData.emplace_back(keyWithPrefix, ele.second);
             });
    // get clock position information
    auto clk_pos = 0xFF; // Invalid position.
    for (auto& hwCallout : ffdc.hwp_errorinfo.hwcallouts)
    {
        if ((hwCallout.hwid == "PROC_REF_CLOCK") ||
            (hwCallout.hwid == "PCI_REF_CLOCK"))
        {
            clk_pos = hwCallout.clkPos;
            break;
        }
    }
    // Adding CDG (Only deconfigure) targets details
    for_each(ffdc.hwp_errorinfo.cdg_targets.begin(),
             ffdc.hwp_errorinfo.cdg_targets.end(),
             [&ffdcUserData, &pelJSONFmtCalloutDataList,
              clk_pos](const CDG_Target& cdg_tgt) -> void {
                 json jsonCalloutData;
                 std::string pelPriority = "H";
                 jsonCalloutData["Priority"] = pelPriority; // Not used
                 jsonCalloutData["SymbolicFRU"] =
                     "REFCLK" + std::to_string(clk_pos);
                 jsonCalloutData["Deconfigured"] = cdg_tgt.deconfigure;
                 jsonCalloutData["EntityPath"] = cdg_tgt.target_entity_path;
                 pelJSONFmtCalloutDataList.emplace_back(jsonCalloutData);
             });
}

void convertFAPItoPELformat(FFDC& ffdc, json& pelJSONFmtCalloutDataList,
                            FFDCData& ffdcUserData)
{
    if (ffdc.ffdc_type == FFDC_TYPE_SPARE_CLOCK_INFO)
    {
        processClockInfoErrorHelper(ffdc, pelJSONFmtCalloutDataList,
                                    ffdcUserData);
        return;
    }

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

                ffdcUserData.emplace_back(
                    std::string(keyPrefix.str()).append("CALLOUT_PLANAR"),
                    (hwCallout.isPlanarCallout == true ? "true" : "false"));

                std::string pelPriority =
                    getPelPriority(hwCallout.callout_priority);

                if (hwCallout.isPlanarCallout)
                {
                    addPlanarCallout(pelJSONFmtCalloutDataList, pelPriority);
                }
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
        lg2::error("Unsupported phal FFDC type to create PEL. MSG: {FFDC_MSG}",
                   "FFDC_MSG", ffdc.message);
    }
}

} // namespace phal
} // namespace pels
} // namespace openpower
