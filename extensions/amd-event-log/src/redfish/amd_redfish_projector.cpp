#include "redfish/amd_redfish_projector.hpp"

#include "ael_metadata.hpp"
#include "redfish/amd_redfish_constants.hpp"
#include "redfish/amd_redfish_utils.hpp"

#include <charconv>
#include <system_error>

namespace phosphor::logging::extensions::ael::redfish
{

namespace
{

void projectAEL(const nlohmann::json& amdMetadata, nlohmann::json& output)
{
    auto afidIt = amdMetadata.find(std::string(fields::AFID));
    if (afidIt == amdMetadata.end() || !afidIt->is_string())
    {
        return;
    }

    const auto& afidStr = afidIt->get_ref<const std::string&>();

    uint64_t afid = 0;
    auto [ptr, ec] =
        std::from_chars(afidStr.data(), afidStr.data() + afidStr.size(), afid);

    if (ec != std::errc() || ptr != afidStr.data() + afidStr.size())
    {
        return;
    }

    nlohmann::json fieldId;

    fieldId["AFID"] = afid;
    fieldId["Description"] = getAFIDDescription(afid);

    auto fruIt = amdMetadata.find(std::string(fields::FruList));

    if (fruIt != amdMetadata.end() && fruIt->is_string())
    {
        nlohmann::json serviceableUnits = nlohmann::json::array();

        for (const auto& fru : splitFRUs(fruIt->get<std::string>()))
        {
            // TODO:
            // Resolve inventory object paths into
            // Redfish resource URIs once inventory-to-
            // Redfish mapping infrastructure becomes
            // available.
            serviceableUnits.push_back({{"InventoryPath", fru}});
        }

        if (!serviceableUnits.empty())
        {
            fieldId["ServiceableUnits"] = std::move(serviceableUnits);

            fieldId["ServiceableUnits@odata.count"] =
                fieldId["ServiceableUnits"].size();
        }
    }

    output["AMDFieldIdentifiers"] = nlohmann::json::array();

    output["AMDFieldIdentifiers"].push_back(std::move(fieldId));

    output["AMDFieldIdentifiers@odata.count"] =
        output["AMDFieldIdentifiers"].size();

    // TODO:
    // Replace AEL.RACK_ID handling with generated
    // platform metadata once common metadata becomes
    // data-driven.
    auto rackIt = amdMetadata.find(std::string(fields::RackId));

    output["RackUnitPosition"] =
        (rackIt != amdMetadata.end()) ? *rackIt : nlohmann::json(nullptr);
}

} // namespace

nlohmann::json project(const nlohmann::json& amdMetadata)
{
    auto redfishIt = amdMetadata.find(std::string(fields::Redfish));
    if (redfishIt != amdMetadata.end())
    {
        return *redfishIt;
    }

    nlohmann::json output;

    output["@odata.type"] = constants::messageOdataType;
    projectAEL(amdMetadata, output);

    return output;
}

} // namespace phosphor::logging::extensions::ael::redfish
