#include "ael_metadata.hpp"
#include "redfish/amd_redfish_projector.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging::extensions::ael::redfish
{

TEST(AMDRedfishProjectorTest, EmptyMetadata)
{
    const nlohmann::json metadata;
    const auto result = project(metadata);

    EXPECT_TRUE(result.empty());
}

TEST(AMDRedfishProjectorTest, RedfishOverridePayload)
{
    nlohmann::json metadata;
    metadata[std::string(fields::Redfish)] = {
        {"AFID", 1234},
        {"Description", "Override"},
    };
    const auto result = project(metadata);

    EXPECT_EQ(result["AFID"], 1234);
    EXPECT_EQ(result["Description"], "Override");
}

TEST(AMDRedfishProjectorTest, ProjectAFIDMetadata)
{
    nlohmann::json metadata;
    metadata[std::string(fields::AFID)] = 10000001ULL;
    metadata[std::string(fields::Description)] = "GPU temperature fault";
    const auto result = project(metadata);
    ASSERT_TRUE(result.contains("AMDFieldIdentifiers"));
    ASSERT_EQ(result["AMDFieldIdentifiers"].size(), 1U);

    EXPECT_EQ(result["AMDFieldIdentifiers"][0]["AFID"], 10000001ULL);
    EXPECT_EQ(result["AMDFieldIdentifiers"][0]["Description"],
              "GPU temperature fault");
}

TEST(AMDRedfishProjectorTest, ProjectServiceableUnits)
{
    nlohmann::json metadata;
    metadata[std::string(fields::AFID)] = 10000001ULL;
    metadata[std::string(fields::FruList)] = "/redfish/v1/Systems/system,"
                                             "/redfish/v1/Chassis/chassis";

    const auto result = project(metadata);
    const auto& units = result["AMDFieldIdentifiers"][0]["ServiceableUnits"];
    ASSERT_EQ(units.size(), 2U);

    EXPECT_EQ(units[0]["@odata.id"], "/redfish/v1/Systems/system");
    EXPECT_EQ(units[1]["@odata.id"], "/redfish/v1/Chassis/chassis");
}

} // namespace phosphor::logging::extensions::ael::redfish
