#include "ael_info.hpp"

#include <gtest/gtest.h>

namespace phosphor::logging::extensions::ael
{

TEST(AELInfoTest, DefaultConstruction)
{
    AELInfo info;

    EXPECT_EQ(info.version, std::string(constants::SchemaVersion));

    EXPECT_EQ(info.afid, 0);

    EXPECT_TRUE(info.fruList.empty());
}

} // namespace phosphor::logging::extensions::ael
