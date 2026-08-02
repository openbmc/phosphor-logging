#include "plugin/cper_processed_descriptor.hpp"
#include "plugin/cper_processed_plugin.hpp"
#include "plugin/plugin_registry.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging::plugin::cperprocessed
{

class PluginTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        bus = std::make_unique<sdbusplus::bus_t>(sdbusplus::bus::new_default());
    }

    PluginContext makeContext(
        const std::string& path = "/xyz/openbmc_project/logging/entry/1")
    {
        return PluginContext{
            *bus,
            path,
        };
    }

    static Descriptor makeDescriptor(ContentType type, OemMetadata oem = {})
    {
        Properties properties{
            .diagnosticDataType = type,
            .notificationType = "notification-guid",
            .sectionType = "section-guid",
            .oem = std::move(oem),
        };

        return Descriptor(std::move(properties));
    }

    std::unique_ptr<sdbusplus::bus_t> bus;
};

class FakeDescriptor : public plugin::Descriptor
{
  public:
    std::string_view interface() const override
    {
        return "xyz.openbmc_project.Logging.Extension.Fake";
    }
};

TEST_F(PluginTest, RegistrationLookup)
{
    PluginRegistry registry;
    registerPlugin(registry);

    EXPECT_NE(registry.lookup(cperprocessed::interface), nullptr);
}

TEST_F(PluginTest, LookupUnknownInterfaceReturnsNull)
{
    PluginRegistry registry;

    EXPECT_EQ(registry.lookup("xyz.openbmc_project.Logging.Extension.Unknown"),
              nullptr);
}

TEST_F(PluginTest, FactoryCreate)
{
    Factory factory;
    auto descriptor = makeDescriptor(ContentType::CPER);
    auto plugin = factory.create(makeContext(), descriptor);
    ASSERT_NE(plugin, nullptr);

    EXPECT_EQ(plugin->interface(), cperprocessed::interface);
}

TEST_F(PluginTest, FactoryCreatesPlugin)
{
    Factory factory;
    auto descriptor = makeDescriptor(ContentType::CPER);
    auto plugin = factory.create(makeContext(), descriptor);
    ASSERT_NE(plugin, nullptr);

    EXPECT_NE(dynamic_cast<Plugin*>(plugin.get()), nullptr);
}

TEST_F(PluginTest, FactoryRejectsWrongDescriptor)
{
    Factory factory;
    FakeDescriptor descriptor;

    EXPECT_THROW(factory.create(makeContext(), descriptor),
                 std::invalid_argument);
}

TEST_F(PluginTest, CreatePluginWithCperSectionType)
{
    auto descriptor = makeDescriptor(ContentType::CPERSection);
    Factory factory;
    auto plugin = factory.create(makeContext(), descriptor);
    ASSERT_NE(plugin, nullptr);

    EXPECT_EQ(plugin->interface(), cperprocessed::interface);
}

TEST_F(PluginTest, DescriptorPropertiesPropagated)
{
    OemMetadata oem{
        {"OemVendor",
         R"({"@odata.type":"#Example.v1_0_0.Record","Property":"Value"})"},
    };
    Properties properties{
        .diagnosticDataType = ContentType::CPERSection,
        .notificationType = "notification-guid",
        .sectionType = "section-guid",
        .oem = oem,
    };
    Descriptor descriptor(std::move(properties));
    Plugin plugin(makeContext(), descriptor);

    EXPECT_EQ(plugin.diagnosticDataType(), ContentType::CPERSection);
    EXPECT_EQ(plugin.notificationType(), "notification-guid");
    EXPECT_EQ(plugin.sectionType(), "section-guid");
    EXPECT_EQ(plugin.oem(), oem);
    EXPECT_EQ(plugin.oem().at("OemVendor"),
              R"({"@odata.type":"#Example.v1_0_0.Record","Property":"Value"})");
}

TEST_F(PluginTest, EmptyOemMetadata)
{
    auto descriptor = makeDescriptor(ContentType::CPER);
    Plugin plugin(makeContext(), descriptor);

    EXPECT_TRUE(plugin.oem().empty());
}

} // namespace phosphor::logging::plugin::cperprocessed
