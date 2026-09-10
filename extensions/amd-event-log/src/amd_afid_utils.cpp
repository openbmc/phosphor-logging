#include "amd_afid_utils.hpp"

#include "amd_afid_db_gen.hpp"

namespace phosphor::logging::extensions::ael
{

AFIDStaticInfo getStaticData()
{
    const auto result = ::getStaticData();

    return AFIDStaticInfo{std::string(result.version), result.fallthroughAFID,
                          std::string(result.rackUnitPosition),
                          std::string(result.schema)};
}

AFIDInfo lookupAFID(const std::string& message,
                    const std::map<std::string, std::string>& additionalData)
{
    const auto result = ::lookupAFID(message, additionalData);

    return AFIDInfo{
        result.afid,
        std::vector<std::string>(result.origins.begin(), result.origins.end()),
        std::vector<std::string>(result.redfishMappings.begin(),
                                 result.redfishMappings.end()),
        std::string(result.description)};
}

} // namespace phosphor::logging::extensions::ael
