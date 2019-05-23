#include "elog_entry.hpp"
#include "extensions.hpp"

#include <gtest/gtest.h>

using namespace phosphor::logging;

// gtest doesn't like this happening in another file, so do it here.
StartupFunctions Extensions::startupFunctions{};
CreateFunctions Extensions::createFunctions{};
DeleteFunctions Extensions::deleteFunctions{};
DeleteProhibitedFunctions Extensions::deleteProhibitedFunctions{};
Extensions::DefaultErrorCaps Extensions::defaultErrorCaps =
    Extensions::DefaultErrorCaps::enable;

void startup1(internal::Manager& manager)
{
}

void startup2(internal::Manager& manager)
{
}

void create1(const std::string& message, uint32_t id, uint64_t timestamp,
             Entry::Level severity, const AdditionalDataArg& additionalData,
             const AssociationEndpointsArg& assocs)
{
}

void create2(const std::string& message, uint32_t id, uint64_t timestamp,
             Entry::Level severity, const AdditionalDataArg& additionalData,
             const AssociationEndpointsArg& assocs)
{
}

void deleteLog1(uint32_t id)
{
}

void deleteLog2(uint32_t id)
{
}

void deleteProhibited1(uint32_t id, bool& prohibited)
{
    prohibited = true;
}

void deleteProhibited2(uint32_t id, bool& prohibited)
{
    prohibited = true;
}

DISABLE_LOG_ENTRY_CAPS();
REGISTER_EXTENSION_FUNCTION(startup1);
REGISTER_EXTENSION_FUNCTION(startup2);
REGISTER_EXTENSION_FUNCTION(create1);
REGISTER_EXTENSION_FUNCTION(create2);
REGISTER_EXTENSION_FUNCTION(deleteProhibited1);
REGISTER_EXTENSION_FUNCTION(deleteProhibited2);
REGISTER_EXTENSION_FUNCTION(deleteLog1);
REGISTER_EXTENSION_FUNCTION(deleteLog2);

TEST(ExtensionsTest, FunctionCallTest)
{
    auto bus = sdbusplus::bus::new_default();
    internal::Manager manager(bus, "testpath");

    EXPECT_EQ(Extensions::getCreateFunctions().size(), 2);
    for (auto& s : Extensions::getStartupFunctions())
    {
        s(manager);
    }

    AdditionalDataArg ad;
    AssociationEndpointsArg assocs;
    EXPECT_EQ(Extensions::getCreateFunctions().size(), 2);
    for (auto& c : Extensions::getCreateFunctions())
    {
        c("test", 5, 6, Entry::Level::Informational, ad, assocs);
    }

    EXPECT_EQ(Extensions::getDeleteFunctions().size(), 2);
    for (auto& d : Extensions::getDeleteFunctions())
    {
        d(5);
    }

    EXPECT_EQ(Extensions::getDeleteProhibitedFunctions().size(), 2);
    for (auto& p : Extensions::getDeleteProhibitedFunctions())
    {
        bool prohibited = false;
        p(5, prohibited);
        EXPECT_TRUE(prohibited);
    }

    EXPECT_TRUE(Extensions::disableDefaultLogCaps());
}
