#include "plugin/cper_descriptor.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging::plugin::cper
{

TEST(CperDescriptorTest, ConstructorStoresMetadata)
{
    nlohmann::json oem{
        {"Vendor", "VENDOR"},
        {"Key", "Value"},
    };

    Descriptor descriptor(DiagnosticDataType::CPER, "notification-guid",
                          "section-guid", 42, oem);

    EXPECT_EQ(descriptor.diagnosticDataType(), DiagnosticDataType::CPER);

    EXPECT_EQ(descriptor.notificationType(), "notification-guid");

    EXPECT_EQ(descriptor.sectionType(), "section-guid");

    EXPECT_EQ(descriptor.cperFd(), 42);

    ASSERT_EQ(descriptor.oem().size(), 2);

    EXPECT_EQ(descriptor.oem().at("Vendor"), "VENDOR");

    EXPECT_EQ(descriptor.oem().at("Key"), "Value");
}

TEST(CperDescriptorTest, EmptyOEMMetadata)
{
    Descriptor descriptor(DiagnosticDataType::CPER, "notification-guid",
                          "section-guid", 42, {});

    EXPECT_TRUE(descriptor.oem().empty());
}

} // namespace phosphor::logging::plugin::cper
