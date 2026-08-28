#include "plugin/plugin_factory.hpp"
#include "plugin/plugin_manager.hpp"
#include "plugin/plugin_registry.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging::test
{

namespace
{

constexpr auto testInterface = "xyz.openbmc_project.Logging.Test";

class TestPlugin : public Plugin
{
  public:
    std::string_view interface() const override
    {
        return testInterface;
    }

    void onDelete() override
    {
        deleted = true;
    }

    bool deleted = false;
};

class TestDescriptor : public plugin::Descriptor
{
  public:
    std::string_view interface() const override
    {
        return testInterface;
    }
};

class TestFactory : public PluginFactory
{
  public:
    plugin::DescriptorPtr createDescriptor(
        const plugin::Request&) const override
    {
        return std::make_unique<TestDescriptor>();
    }

    PluginPtr create(const PluginContext&,
                     const plugin::Descriptor&) const override
    {
        return std::make_unique<TestPlugin>();
    }

    PluginPtr deserialize(const PluginContext&,
                          const nlohmann::json&) const override
    {
        return std::make_unique<TestPlugin>();
    }
};

} // namespace

TEST(PluginManagerTest, CreatePlugin)
{
    PluginRegistry registry;
    registry.registerPlugin(testInterface, std::make_unique<TestFactory>());
    PluginManager manager(std::move(registry));
    auto bus = sdbusplus::bus::new_default();
    PluginContext context{
        bus,
        "/xyz/openbmc_project/logging/entry/1",
    };
    plugin::RequestList requests{
        {
            .interface = testInterface,
            .data = {},
        },
    };

    auto plugins = manager.create(context, requests);
    ASSERT_EQ(plugins.size(), 1);

    EXPECT_EQ(plugins.front()->interface(), testInterface);
}

TEST(PluginManagerTest, CreateUnknownPlugin)
{
    PluginRegistry registry;
    PluginManager manager(std::move(registry));
    auto bus = sdbusplus::bus::new_default();
    PluginContext context{
        bus,
        "/xyz/openbmc_project/logging/entry/1",
    };
    plugin::RequestList requests{
        {
            .interface = "unknown.interface",
            .data = {},
        },
    };
    auto plugins = manager.create(context, requests);

    EXPECT_TRUE(plugins.empty());
}

TEST(PluginManagerTest, CreateMultiplePlugins)
{
    PluginRegistry registry;
    registry.registerPlugin(testInterface, std::make_unique<TestFactory>());
    PluginManager manager(std::move(registry));
    auto bus = sdbusplus::bus::new_default();
    PluginContext context{
        bus,
        "/xyz/openbmc_project/logging/entry/1",
    };
    plugin::RequestList requests{
        {
            .interface = testInterface,
            .data = {},
        },
        {
            .interface = testInterface,
            .data = {},
        },
    };

    auto plugins = manager.create(context, requests);
    ASSERT_EQ(plugins.size(), 2);

    EXPECT_EQ(plugins[0]->interface(), testInterface);
    EXPECT_EQ(plugins[1]->interface(), testInterface);
}

TEST(PluginLifecycleTest, DeleteCallback)
{
    TestPlugin plugin;

    EXPECT_FALSE(plugin.deleted);

    plugin.onDelete();

    EXPECT_TRUE(plugin.deleted);
}

TEST(PluginManagerTest, DeserializePlugin)
{
    PluginRegistry registry;
    registry.registerPlugin(testInterface, std::make_unique<TestFactory>());
    PluginManager manager(std::move(registry));
    auto bus = sdbusplus::bus::new_default();
    PluginContext context{
        bus,
        "/xyz/openbmc_project/logging/entry/1",
    };
    nlohmann::json data = {{testInterface, nlohmann::json::object()}};
    auto plugins = manager.deserialize(context, data);

    ASSERT_EQ(plugins.size(), 1);
    EXPECT_EQ(plugins.front()->interface(), testInterface);
}

TEST(PluginManagerTest, DeserializeUnknownPlugin)
{
    PluginRegistry registry;
    PluginManager manager(std::move(registry));
    auto bus = sdbusplus::bus::new_default();
    PluginContext context{
        bus,
        "/xyz/openbmc_project/logging/entry/1",
    };
    nlohmann::json data = {
        {"xyz.openbmc_project.Logging.Unknown", nlohmann::json::object()}};
    auto plugins = manager.deserialize(context, data);

    EXPECT_TRUE(plugins.empty());
}

} // namespace phosphor::logging::test
