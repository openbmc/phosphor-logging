#include "extensions/openpower-pels/dbus_types.hpp"

#include <gtest/gtest.h>

namespace openpower::pels
{

TEST(DBusTypesTest, ObjectManagerPropertyTypes)
{
    DBusInterfaceMap interfaces{{"com.example.Inventory",
                                 {{"Present", true},
                                  {"Count", uint16_t{1}},
                                  {"BaseBIOSTable", BiosAttributes{}}}}};

    const auto& properties = interfaces.at("com.example.Inventory");
    EXPECT_TRUE(std::get<bool>(properties.at("Present")));
    EXPECT_EQ(std::get<uint16_t>(properties.at("Count")), 1);
    EXPECT_TRUE(
        std::get<BiosAttributes>(properties.at("BaseBIOSTable")).empty());
}

} // namespace openpower::pels
