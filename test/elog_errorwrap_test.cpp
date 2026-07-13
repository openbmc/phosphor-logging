#include "elog_errorwrap_test.hpp"

#include <vector>

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

TEST_F(TestLogManager, evictsResolvedWithinOldestHalfOnly)
{
    // Fill the real-error cap with unresolved errors.
    std::vector<uint32_t> ids;
    for (size_t i = 0; i < ERROR_CAP; i++)
    {
        ids.push_back(manager.commitWithLvl(i, "FOO", 0));
    }
    EXPECT_EQ(ERROR_CAP, manager.getRealErrSize());

    // Resolve one entry in the oldest half and one in the newer half.
    auto oldHalfResolved = ids.at(5);
    auto newHalfResolved = ids.at(ERROR_CAP / 2 + 1);
    manager.entries[oldHalfResolved]->resolved(true);
    manager.entries[newHalfResolved]->resolved(true);

    // Eviction scans only the oldest half: the resolved entry there is dropped,
    // while the resolved entry in the newer half (recent history) is retained.
    manager.commitWithLvl(ERROR_CAP, "FOO", 0);

    EXPECT_EQ(ERROR_CAP, manager.getRealErrSize());
    EXPECT_EQ(0U, manager.entries.count(oldHalfResolved));
    EXPECT_EQ(1U, manager.entries.count(newHalfResolved));
}

} // namespace internal
} // namespace logging
} // namespace phosphor
