#include "elog_errorwrap_test.hpp"

namespace phosphor
{
namespace logging
{
namespace internal
{

TEST_F(TestLogManager, logCap)
{
    for (size_t i = 0; i < ERROR_INFO_CAP + 20; i++)
    {
        manager.commitWithLvl(i, "FOO", 6);
    }

    // Max num of Info( and below Sev) errors can be created is qual to
    // ERROR_INFO_CAP
    EXPECT_EQ(ERROR_INFO_CAP, manager.getInfoErrSize());

    for (size_t i = 0; i < ERROR_CAP + 20; i++)
    {
        manager.commitWithLvl(i, "FOO", 0);
    }
    // Max num of high severity errors can be created is qual to ERROR_CAP
    EXPECT_EQ(ERROR_CAP, manager.getRealErrSize());
}

TEST_F(TestLogManager, entryIdNotResetOnEraseAll)
{
    // Create a few entries so the ID counter advances past 0.
    for (size_t i = 0; i < 3; i++)
    {
        manager.commitWithLvl(i, "FOO", 6);
    }

    auto idBeforeErase = manager.lastEntryID();
    EXPECT_NE(0u, idBeforeErase);

    // Clearing all logs must leave the ID counter untouched.
    manager.eraseAll();
    EXPECT_EQ(idBeforeErase, manager.lastEntryID());
}

} // namespace internal
} // namespace logging
} // namespace phosphor
