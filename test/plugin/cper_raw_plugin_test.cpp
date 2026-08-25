#include "plugin/cper_raw_descriptor.hpp"
#include "plugin/cper_raw_plugin.hpp"
#include "plugin/plugin_registry.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging::plugin::cperraw
{

class CperRawPluginTest : public ::testing::Test
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

    static Descriptor makeDescriptor()
    {
        Properties properties{
            .artifact = {.path = "/tmp/cperraw-artifact"},
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

TEST_F(CperRawPluginTest, RegistrationLookup)
{
    PluginRegistry registry;
    registerPlugin(registry);

    EXPECT_NE(registry.lookup(cperraw::interface), nullptr);
}

TEST_F(CperRawPluginTest, LookupUnknownInterfaceReturnsNull)
{
    PluginRegistry registry;

    EXPECT_EQ(registry.lookup("xyz.openbmc_project.Logging.Extension.Unknown"),
              nullptr);
}

TEST_F(CperRawPluginTest, FactoryCreate)
{
    Factory factory;
    auto descriptor = makeDescriptor();
    auto plugin = factory.create(makeContext(), descriptor);
    ASSERT_NE(plugin, nullptr);

    EXPECT_EQ(plugin->interface(), cperraw::interface);
}

TEST_F(CperRawPluginTest, FactoryCreatesRawPlugin)
{
    Factory factory;
    auto descriptor = makeDescriptor();
    auto plugin = factory.create(makeContext(), descriptor);
    ASSERT_NE(plugin, nullptr);

    EXPECT_NE(dynamic_cast<Plugin*>(plugin.get()), nullptr);
}

TEST_F(CperRawPluginTest, FactoryRejectsWrongDescriptor)
{
    Factory factory;
    FakeDescriptor descriptor;

    EXPECT_THROW(factory.create(makeContext(), descriptor),
                 std::invalid_argument);
}

TEST_F(CperRawPluginTest, DescriptorPropertiesPropagated)
{
    Properties properties{
        .artifact = {.path = "/tmp/cperraw-artifact"},
    };
    Descriptor descriptor(std::move(properties));
    Plugin plugin(makeContext(), descriptor);

    EXPECT_EQ(plugin.interface(), cperraw::interface);
}

TEST_F(CperRawPluginTest, DescriptorReturnsArtifact)
{
    constexpr auto artifactPath = "/tmp/cperraw-artifact";
    Properties properties{
        .artifact = {.path = artifactPath},
    };

    Descriptor descriptor(std::move(properties));
    EXPECT_EQ(descriptor.properties().artifact.path, artifactPath);
}

} // namespace phosphor::logging::plugin::cperraw
