#pragma once

#include "bmc_pos_test_utils.hpp"

#include <gtest/gtest.h>

// Undefine FAIL macro from gtest to avoid conflicts with FAIL
// class in elog-errors.hpp. Tests can still use GTEST_FAIL()
// directly if needed.
#undef FAIL

namespace phosphor::logging::test
{

/**
 * @brief Test fixture for tests that need the BMC position
 *
 * Tests that instantiate internal::Manager should
 * inherit from this fixture.
 */
class BMCPosTestFixture : public ::testing::Test
{
  protected:
    /**
     * @brief Clean up BMC position file directory after each test
     */
    void TearDown() override
    {
        cleanupBMCPosTestDir();
    }
};

} // namespace phosphor::logging::test
