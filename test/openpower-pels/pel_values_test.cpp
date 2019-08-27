#include "extensions/openpower-pels/pel_values.hpp"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include <gtest/gtest.h>

using namespace openpower::pels::pel_values;

const PELValues pelValues = {
    {0x50, "cec_hardware", "cec_hardware"},
    {0x51, "cec_sp_a", "cec_sp_a"},
    {0x52, "cec_sp_b", "cec_sp_b"},
    {0x53, "cec_node_controller", "cec_node_controller"},
    {0x55, "cec_vpd", "cec_vpd"},
    {0x56, "cec_i2c", "cec_i2c"},
    {0x57, "cec_chip_iface", "cec_chip_iface"},
    {0x58, "cec_clocks", "cec_clocks"},
    {0x59, "cec_op_panel", "cec_op_panel"},
    {0x5A, "cec_tod", "cec_tod"},
    {0x5B, "cec_storage_device", "cec_storage_device"},
    {0x5C, "cec_sp_hyp_iface", "cec_sp_hyp_iface"},
    {0x5D, "cec_service_network", "cec_service_network"},
    {0x5E, "cec_sp_hostboot_iface", "cec_sp_hostboot_iface"}};

TEST(PELFieldsTest, TestFindFields)
{
    auto s = findByValue(0x5D, pelValues);
    ASSERT_NE(s, pelValues.end());
    ASSERT_EQ(0x5D, std::get<fieldValuePos>(*s));
    ASSERT_EQ("cec_service_network", std::get<registryNamePos>(*s));

    s = findByName("cec_clocks", pelValues);
    ASSERT_NE(s, pelValues.end());
    ASSERT_EQ(0x58, std::get<fieldValuePos>(*s));
    ASSERT_EQ("cec_clocks", std::get<registryNamePos>(*s));

    s = findByValue(0xFF, pelValues);
    ASSERT_EQ(s, pelValues.end());

    s = findByName("foo", pelValues);
    ASSERT_EQ(s, pelValues.end());
}
