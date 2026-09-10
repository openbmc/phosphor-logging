#include "ael_info.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging::extensions::ael
{

TEST(AELInfoTest, UnknownMessageReturnsNullopt)
{
    auto result = resolveAFID("INVALID.MESSAGE", {});

    EXPECT_FALSE(result.has_value());
}

TEST(AELInfoTest, LookupReturnsAFID)
{
    std::map<std::string, std::string> additionalData{
        {"SENSOR_NAME", "GPU0_TEMP"},
    };
    auto result = resolveAFID("xyz.openbmc_project.Sensor.Threshold."
                              "ReadingAboveLowerCriticalThreshold",
                              additionalData);
    if (!result)
    {
        GTEST_FAIL() << "Expected AFID lookup to succeed";
        return;
    }

    EXPECT_NE(result->afid, 0ULL);
}

TEST(AELInfoTest, LookupReturnsValidMetadataObject)
{
    std::map<std::string, std::string> additionalData{
        {"SENSOR_NAME", "GPU0_TEMP"},
    };
    auto result = resolveAFID("xyz.openbmc_project.Sensor.Threshold."
                              "ReadingAboveLowerCriticalThreshold",
                              additionalData);
    if (!result)
    {
        GTEST_FAIL() << "Expected AFID lookup to succeed";
        return;
    }

    EXPECT_NE(result->afid, 0ULL);
    EXPECT_GE(result->origins.size(), 0U);
    EXPECT_GE(result->redfishMappings.size(), 0U);
}

TEST(AELInfoTest, LookupReturnsConsistentResult)
{
    std::map<std::string, std::string> additionalData{
        {"SENSOR_NAME", "GPU0_TEMP"},
    };
    auto result1 = resolveAFID("xyz.openbmc_project.Sensor.Threshold."
                               "ReadingAboveLowerCriticalThreshold",
                               additionalData);
    auto result2 = resolveAFID("xyz.openbmc_project.Sensor.Threshold."
                               "ReadingAboveLowerCriticalThreshold",
                               additionalData);
    if (!result1 || !result2)
    {
        GTEST_FAIL() << "Expected AFID lookup to succeed";
        return;
    }

    EXPECT_EQ(result1->afid, result2->afid);
    EXPECT_EQ(result1->description, result2->description);
    EXPECT_EQ(result1->origins.size(), result2->origins.size());
    EXPECT_EQ(result1->redfishMappings.size(), result2->redfishMappings.size());
}

TEST(AELInfoTest, EmptyAdditionalDataHandled)
{
    auto result = resolveAFID("xyz.openbmc_project.Sensor.Threshold."
                              "ReadingAboveLowerCriticalThreshold",
                              {});
    if (result)
    {
        EXPECT_NE(result->afid, 0ULL);
    }
}

} // namespace phosphor::logging::extensions::ael
