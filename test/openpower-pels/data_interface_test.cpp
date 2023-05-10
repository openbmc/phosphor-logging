#include "extensions/openpower-pels/data_interface.hpp"

#include <gtest/gtest.h>

using namespace openpower::pels;

TEST(DataInterfaceTest, ExtractConnectorLocCode)
{
    {
        auto [base, connector] =
            DataInterface::extractConnectorFromLocCode("Ufcs-P0-C2-T11");

        EXPECT_EQ(base, "Ufcs-P0-C2");
        EXPECT_EQ(connector, "-T11");
    }

    {
        auto [base, connector] =
            DataInterface::extractConnectorFromLocCode("Ufcs-P0-C2");

        EXPECT_EQ(base, "Ufcs-P0-C2");
        EXPECT_TRUE(connector.empty());
    }
}

TEST(DataInterfaceTest, ExtractUptime)
{
    uint64_t seconds = 123456789;
    std::string retUptime = "3y 332d 21h 33m 9s";

    auto bus = sdbusplus::bus::new_default();
    DataInterface dataIntf(bus);
    std::string uptime = dataIntf.getBMCUptime(seconds);

    EXPECT_EQ(uptime, retUptime);
}
