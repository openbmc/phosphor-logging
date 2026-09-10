#include "redfish/amd_redfish_projector.hpp"

#include "ael_metadata.hpp"
#include "redfish/amd_redfish_utils.hpp"

namespace phosphor::logging::extensions::ael::redfish
{

namespace
{

/**
 * @brief Project AEL metadata into AMD Redfish format.
 *
 * Converts internal AMD metadata into the AMD OEM Redfish
 * payload representation.
 *
 * @param[in] amdMetadata Internal AMD metadata.
 * @param[out] output Generated AMD Redfish payload.
 */
void projectAEL(const nlohmann::json& amdMetadata, nlohmann::json& output)
{
    auto afidIt = amdMetadata.find(std::string(fields::AFID));
    if (afidIt == amdMetadata.end() || !afidIt->is_number_unsigned())
    {
        return;
    }

    nlohmann::json fieldId;
    fieldId["AFID"] = afidIt->get<uint64_t>();
    auto descIt = amdMetadata.find(std::string(fields::Description));
    fieldId["Description"] =
        (descIt != amdMetadata.end()) ? *descIt : nlohmann::json(nullptr);
    auto fruIt = amdMetadata.find(std::string(fields::FruList));
    if (fruIt != amdMetadata.end() && fruIt->is_string())
    {
        nlohmann::json serviceableUnits = nlohmann::json::array();
        for (const auto& fru : splitFRUs(fruIt->get<std::string>()))
        {
            serviceableUnits.push_back({{"@odata.id", fru}});
        }
        if (!serviceableUnits.empty())
        {
            fieldId["ServiceableUnits"] = std::move(serviceableUnits);
            fieldId["ServiceableUnits@odata.count"] =
                fieldId["ServiceableUnits"].size();
        }
    }

    auto schemaIt = amdMetadata.find(std::string(fields::Schema));
    output["@odata.type"] =
        (schemaIt != amdMetadata.end()) ? *schemaIt : nlohmann::json(nullptr);
    auto versionIt = amdMetadata.find(std::string(fields::Version));
    if (versionIt != amdMetadata.end())
    {
        output["Version"] = *versionIt;
    }
    output["AMDFieldIdentifiers"] = nlohmann::json::array();
    output["AMDFieldIdentifiers"].push_back(std::move(fieldId));
    output["AMDFieldIdentifiers@odata.count"] =
        output["AMDFieldIdentifiers"].size();
    auto rackIt = amdMetadata.find(std::string(fields::RackUnitPosition));

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
    projectAEL(amdMetadata, output);

    return output;
}

} // namespace phosphor::logging::extensions::ael::redfish
