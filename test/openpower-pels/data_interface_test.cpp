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
