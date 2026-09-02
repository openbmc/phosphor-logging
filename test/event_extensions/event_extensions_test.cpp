#include "event_extensions/manager.hpp"
#include "event_extensions/registry.hpp"
#include "event_extensions/request.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging::event_extensions
{

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

TEST(RegistryTest, UnknownProviderLookup)
{
    Registry registry;

    EXPECT_EQ(registry.lookup("test.interface"), nullptr);
}

} // namespace phosphor::logging::event_extensions
