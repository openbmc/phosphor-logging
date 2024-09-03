#include "elog_entry.hpp"
#include "extensions.hpp"

#include <sdbusplus/test/sdbus_mock.hpp>

#include <gtest/gtest.h>

using namespace phosphor::logging;

void startup1(internal::Manager& /*manager*/) {}

void startup2(internal::Manager& /*manager*/) {}

void create1(const std::string& /*message*/, uint32_t /*id*/,
             uint64_t /*timestamp*/, Entry::Level /*severity*/,
             const AdditionalDataArg& /*additionalData*/,
             const AssociationEndpointsArg& /*assocs*/, const FFDCArg& /*ffdc*/)
{}

void create2(const std::string& /*message*/, uint32_t /*id*/,
             uint64_t /*timestamp*/, Entry::Level /*severity*/,
             const AdditionalDataArg& /*additionalData*/,
             const AssociationEndpointsArg& /*assocs*/, const FFDCArg& /*ffdc*/)
{}

void deleteLog1(uint32_t /*id*/) {}

void deleteLog2(uint32_t /*id*/) {}

void deleteProhibited1(uint32_t /*id*/, bool& prohibited)
{
    prohibited = true;
}

void deleteProhibited2(uint32_t /*id*/, bool& prohibited)
{
    prohibited = true;
}

void logIDWithHwIsolation1(std::vector<uint32_t>& logIDs)
{
    logIDs.push_back(1);
}

void logIDWithHwIsolation2(std::vector<uint32_t>& logIDs)
{
    logIDs.push_back(2);
}

DISABLE_LOG_ENTRY_CAPS()
REGISTER_EXTENSION_FUNCTION(startup1)
REGISTER_EXTENSION_FUNCTION(startup2)
REGISTER_EXTENSION_FUNCTION(create1)
REGISTER_EXTENSION_FUNCTION(create2)
REGISTER_EXTENSION_FUNCTION(deleteProhibited1)
REGISTER_EXTENSION_FUNCTION(deleteProhibited2)
REGISTER_EXTENSION_FUNCTION(logIDWithHwIsolation1)
REGISTER_EXTENSION_FUNCTION(logIDWithHwIsolation2)
REGISTER_EXTENSION_FUNCTION(deleteLog1)
REGISTER_EXTENSION_FUNCTION(deleteLog2)

TEST(ExtensionsTest, FunctionCallTest)
{
    sdbusplus::SdBusMock sdbusMock;
    sdbusplus::bus_t bus = sdbusplus::get_mocked_new(&sdbusMock);
    internal::Manager manager(bus, "testpath");

    EXPECT_EQ(Extensions::getStartupFunctions().size(), 2);
    for (auto& s : Extensions::getStartupFunctions())
    {
        s(manager);
    }

    AdditionalDataArg ad;
    AssociationEndpointsArg assocs;
    FFDCArg ffdc;
    EXPECT_EQ(Extensions::getCreateFunctions().size(), 2);
    for (auto& c : Extensions::getCreateFunctions())
    {
        c("test", 5, 6, Entry::Level::Informational, ad, assocs, ffdc);
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

    EXPECT_EQ(Extensions::getLogIDWithHwIsolationFunctions().size(), 2);
    std::vector<uint32_t> ids;
    for (size_t i = 0; i < 2; ++i)
    {
        auto getLogIDWithHwIsolation =
            Extensions::getLogIDWithHwIsolationFunctions()[i];
        getLogIDWithHwIsolation(ids);
        if (i == 0)
        {
            EXPECT_EQ(ids.size(), 1);
            EXPECT_EQ(ids[0], 1);
        }
        if (i == 1)
        {
            EXPECT_EQ(ids.size(), 2);
            EXPECT_EQ(ids[1], 2);
        }
    }

    EXPECT_TRUE(Extensions::disableDefaultLogCaps());
}
