#include "plugin/cper_processed_descriptor.hpp"
#include "plugin/cper_processed_plugin.hpp"
#include "plugin/plugin_registry.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging::plugin::cper
{

class CperProcessedPluginTest : public ::testing::Test
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

    static ProcessedDescriptor makeDescriptor(ContentType type,
                                              OemMetadata oem = {})
    {
        Properties properties{
            .diagnosticDataType = type,
            .notificationType = "notification-guid",
            .sectionType = "section-guid",
            .oem = std::move(oem),
        };

        return ProcessedDescriptor(std::move(properties));
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

TEST_F(CperProcessedPluginTest, RegistrationLookup)
{
    PluginRegistry registry;
    registerProcessedPlugin(registry);

    EXPECT_NE(registry.lookup(cper::interface), nullptr);
}

TEST_F(CperProcessedPluginTest, LookupUnknownInterfaceReturnsNull)
{
    PluginRegistry registry;

    EXPECT_EQ(registry.lookup("xyz.openbmc_project.Logging.Extension.Unknown"),
              nullptr);
}

TEST_F(CperProcessedPluginTest, FactoryCreate)
{
    ProcessedFactory factory;
    auto descriptor = makeDescriptor(ContentType::CPER);
    auto plugin = factory.create(makeContext(), descriptor);
    ASSERT_NE(plugin, nullptr);

    EXPECT_EQ(plugin->interface(), cper::interface);
}

TEST_F(CperProcessedPluginTest, FactoryCreatesProcessedPlugin)
{
    ProcessedFactory factory;
    auto descriptor = makeDescriptor(ContentType::CPER);
    auto plugin = factory.create(makeContext(), descriptor);
    ASSERT_NE(plugin, nullptr);

    EXPECT_NE(dynamic_cast<ProcessedPlugin*>(plugin.get()), nullptr);
}

TEST_F(CperProcessedPluginTest, FactoryRejectsWrongDescriptor)
{
    ProcessedFactory factory;
    FakeDescriptor descriptor;

    EXPECT_THROW(factory.create(makeContext(), descriptor),
                 std::invalid_argument);
}

TEST_F(CperProcessedPluginTest, CreatePluginWithCperSectionType)
{
    auto descriptor = makeDescriptor(ContentType::CPERSection);
    ProcessedFactory factory;
    auto plugin = factory.create(makeContext(), descriptor);
    ASSERT_NE(plugin, nullptr);

    EXPECT_EQ(plugin->interface(), cper::interface);
}

TEST_F(CperProcessedPluginTest, DescriptorPropertiesPropagated)
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

    ProcessedDescriptor descriptor(std::move(properties));
    ProcessedPlugin plugin(makeContext(), descriptor);

    EXPECT_EQ(plugin.diagnosticDataType(), ContentType::CPERSection);
    EXPECT_EQ(plugin.notificationType(), "notification-guid");
    EXPECT_EQ(plugin.sectionType(), "section-guid");
    EXPECT_EQ(plugin.oem(), oem);
    EXPECT_EQ(plugin.oem().at("OemVendor"),
              R"({"@odata.type":"#Example.v1_0_0.Record","Property":"Value"})");
}

TEST_F(CperProcessedPluginTest, EmptyOemMetadata)
{
    auto descriptor = makeDescriptor(ContentType::CPER);
    ProcessedPlugin plugin(makeContext(), descriptor);

    EXPECT_TRUE(plugin.oem().empty());
}

TEST_F(CperProcessedPluginTest, BuildPayload)
{
    ProcessedFactory factory;
    nlohmann::json metadata{
        {"AMD",
         {
             {"AEL.VERSION", "1.0"},
             {"AEL.AFID", "4660"},
         }},
    };

    auto payload = factory.buildExtensionPayload(metadata);
    ASSERT_TRUE(payload.contains(oemKey));
    EXPECT_EQ(payload[oemKey]["AMD"], metadata["AMD"].dump());
}

TEST_F(CperProcessedPluginTest, BuildExtensionPayloadEmptyMetadata)
{
    ProcessedFactory factory;
    auto payload = factory.buildExtensionPayload(nlohmann::json::object());

    ASSERT_TRUE(payload.contains(oemKey));
    EXPECT_TRUE(payload[oemKey].empty());
}

} // namespace phosphor::logging::plugin::cper
