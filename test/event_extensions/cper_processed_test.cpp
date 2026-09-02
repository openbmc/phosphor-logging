#include "event_extensions/cper/processed.hpp"
#include "event_extensions/registry.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging::event_extensions
{

TEST(CPERProcessedTest, RegisterExtension)
{
    Registry registry;

    cper::processed::registerExtension(registry);

    EXPECT_NE(registry.find(cper::processed::interface), nullptr);
}

TEST(CPERProcessedTest, SerializeProperties)
{
    cper::processed::Properties properties{
        cper::processed::ContentType::CPER,
        "notification-type",
        "section-type",
        {{"Vendor", "AMD"}},
    };

    EXPECT_EQ(properties.diagnosticDataType,
              cper::processed::ContentType::CPER);
    EXPECT_EQ(properties.notificationType, "notification-type");
    EXPECT_EQ(properties.sectionType, "section-type");
    EXPECT_EQ(properties.oem.at("Vendor"), "AMD");
}

TEST(CPERProcessedTest, InterfaceName)
{
    EXPECT_EQ(cper::processed::interface,
              "xyz.openbmc_project.Logging.Extension.CPER.Processed");
}

} // namespace phosphor::logging::event_extensions
