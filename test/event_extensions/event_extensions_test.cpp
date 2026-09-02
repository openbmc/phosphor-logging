#include "event_extensions/registry.hpp"
#include "event_extensions/request.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging::event_extensions
{

namespace
{

ExtensionPtr createExtension(const Context&, const Request&)
{
    return {};
}

} // namespace

TEST(RequestTest, DefaultConstruct)
{
    Request request;
    EXPECT_TRUE(request.interface.empty());
    EXPECT_TRUE(request.data.is_null());
}

TEST(RequestTest, EmptyRequestList)
{
    RequestList requests;
    EXPECT_TRUE(requests.empty());
}

TEST(RegistryTest, UnknownExtensionLookup)
{
    Registry registry;
    EXPECT_EQ(registry.find("test.interface"), nullptr);
}

TEST(RegistryTest, RegisterExtension)
{
    Registry registry;

    registry.registerExtension("test.interface", createExtension);

    EXPECT_NE(registry.find("test.interface"), nullptr);
}

TEST(RegistryTest, DuplicateExtensionRegistration)
{
    Registry registry;

    registry.registerExtension("test.interface", createExtension);

    registry.registerExtension("test.interface", createExtension);

    EXPECT_NE(registry.find("test.interface"), nullptr);
}

} // namespace phosphor::logging::event_extensions
