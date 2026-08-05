#include "plugin/plugin_factory.hpp"
#include "plugin/plugin_registry.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging
{

namespace
{

constexpr auto testInterface = "xyz.openbmc_project.Logging.Test";

class TestFactory : public PluginFactory
{
  public:
    PluginPtr create(const PluginContext&,
                     const plugin::Descriptor&) const override
    {
        return nullptr;
    }
};

} // namespace

TEST(PluginRegistryTest, RegisterAndLookupFactory)
{
    PluginRegistry registry;
    registry.registerPlugin(testInterface, std::make_unique<TestFactory>());

    EXPECT_NE(registry.lookup(testInterface), nullptr);
}

TEST(PluginRegistryTest, LookupUnknownFactory)
{
    PluginRegistry registry;

    EXPECT_EQ(registry.lookup(testInterface), nullptr);
}

TEST(PluginRegistryTest, DuplicateRegistrationKeepsOriginalFactory)
{
    PluginRegistry registry;
    registry.registerPlugin(testInterface, std::make_unique<TestFactory>());
    auto* original = registry.lookup(testInterface);

    EXPECT_NO_THROW(registry.registerPlugin(testInterface,
                                            std::make_unique<TestFactory>()));
    EXPECT_EQ(registry.lookup(testInterface), original);
}

} // namespace phosphor::logging
