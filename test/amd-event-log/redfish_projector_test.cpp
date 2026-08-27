#include "ael_metadata.hpp"
#include "redfish/amd_redfish_projector.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging::extensions::ael::redfish
{

TEST(RedfishProjectorTest, EmptyMetadata)
{
    nlohmann::json metadata;
    auto result = project(metadata);

    EXPECT_TRUE(result.contains("@odata.type"));
    EXPECT_FALSE(result.contains("AMDFieldIdentifiers"));
}

TEST(RedfishProjectorTest, ProjectAFID)
{
    nlohmann::json metadata;
    metadata[std::string(fields::AFID)] = "12001";

    auto result = project(metadata);
    ASSERT_TRUE(result.contains("AMDFieldIdentifiers"));

    const auto& ids = result["AMDFieldIdentifiers"];
    ASSERT_EQ(ids.size(), 1);

    EXPECT_EQ(ids[0]["AFID"], 12001);
    EXPECT_EQ(ids[0]["Description"], "Compute Tray Error");
    EXPECT_EQ(result["AMDFieldIdentifiers@odata.count"], 1);
}

TEST(RedfishProjectorTest, UnknownAFID)
{
    nlohmann::json metadata;
    metadata[std::string(fields::AFID)] = "999999";

    auto result = project(metadata);
    ASSERT_TRUE(result.contains("AMDFieldIdentifiers"));

    const auto& ids = result["AMDFieldIdentifiers"];
    ASSERT_EQ(ids.size(), 1);
    EXPECT_EQ(ids[0]["Description"], "Unknown Error");
}

TEST(RedfishProjectorTest, PassthroughPayload)
{
    nlohmann::json payload = {
        {"@odata.type", "#AMD_Message.v1_0_0.AMD_Message"},
        {"AMDFieldIdentifiers",
         nlohmann::json::array(
             {{{"AFID", 12345}, {"Description", "PreRendered"}}})},
    };

    nlohmann::json metadata;
    metadata[std::string(fields::Redfish)] = payload;

    auto result = project(metadata);
    EXPECT_EQ(result, payload);
}

} // namespace phosphor::logging::extensions::ael::redfish
