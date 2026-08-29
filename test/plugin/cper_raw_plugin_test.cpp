#include "plugin/cper_raw_descriptor.hpp"
#include "plugin/cper_raw_plugin.hpp"
#include "plugin/plugin_registry.hpp"

#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

namespace phosphor::logging::plugin::cper::raw
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

    EXPECT_NE(registry.lookup(cper::raw::interface), nullptr);
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

    EXPECT_EQ(plugin->interface(), interface);
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

    EXPECT_EQ(plugin.interface(), interface);
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

TEST_F(CperRawPluginTest, Serialize)
{
    Plugin plugin(makeContext(), makeDescriptor());
    auto data = plugin.serialize();

    EXPECT_EQ(data.at(artifactPathKey).get<std::filesystem::path>(),
              std::filesystem::path("/tmp/cperraw-artifact"));
}

TEST_F(CperRawPluginTest, Deserialize)
{
    Factory factory;
    auto plugin = factory.deserialize(
        makeContext(),
        {
            {artifactPathKey, std::filesystem::path("/tmp/cperraw-artifact")},
        });
    ASSERT_NE(plugin, nullptr);

    EXPECT_EQ(plugin->interface(), cper::raw::interface);
}

TEST_F(CperRawPluginTest, SerializeDeserializeRoundTrip)
{
    auto descriptor = makeDescriptor();
    const auto expectedArtifact = descriptor.properties().artifact.path;
    Plugin original(makeContext(), std::move(descriptor));
    auto serialized = original.serialize();

    Factory factory;
    auto restored = factory.deserialize(
        makeContext("/xyz/openbmc_project/logging/entry/2"), serialized);
    ASSERT_NE(restored, nullptr);

    EXPECT_EQ(restored->interface(), cper::raw::interface);
    EXPECT_EQ(restored->serialize(), serialized);
    EXPECT_EQ(
        restored->serialize().at(artifactPathKey).get<std::filesystem::path>(),
        expectedArtifact);
}

} // namespace phosphor::logging::plugin::cper::raw
