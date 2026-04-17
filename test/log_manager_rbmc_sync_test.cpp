#include "elog_entry.hpp"
#include "elog_serialize.hpp"
#include "log_manager.hpp"
#include "paths.hpp"

#include <unistd.h>

#include <sdbusplus/test/sdbus_mock.hpp>
#include <sdeventplus/event.hpp>

#include <filesystem>
#include <memory>
#include <string>

#include <gtest/gtest.h>

namespace phosphor::logging::test
{
namespace fs = std::filesystem;

class LogManagerRedundantBMCSyncTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        sd_event_default(&sdEvent);
        bus = sdbusplus::get_mocked_new(&sdbusMock);
        manager = std::make_unique<internal::Manager>(bus, OBJ_INTERNAL);

        // paths::error() is shared across test binaries, so do not delete the
        // whole directory here. Just make sure it exists and clean up only the
        // file used by this test.
        fs::create_directories(paths::error());

        const auto* testInfo =
            ::testing::UnitTest::GetInstance()->current_test_info();

        tempEntryDir = fs::temp_directory_path() /
                       ("elog_sync_temp_" + std::to_string(::getpid()) + "_" +
                        std::string(testInfo->name()));
        fs::remove_all(tempEntryDir);
        fs::create_directories(tempEntryDir);

        entryId = 77;
        repoEntryPath = paths::error() / std::to_string(entryId);

        fs::remove(repoEntryPath);
    }

    void TearDown() override
    {
        fs::remove(repoEntryPath);
        fs::remove_all(tempEntryDir);

        manager.reset();

        if (sdEvent != nullptr)
        {
            sd_event_unref(sdEvent);
            sdEvent = nullptr;
        }
    }

    fs::path writeTempSerializedEntry(Entry::Level level,
                                      const std::string& message, bool resolved)
    {
        auto tempEntryPath = tempEntryDir / std::to_string(entryId);

        std::string entryMessage = message;
        std::map<std::string, std::string> additionalData{{"TEST", "1"}};
        AssociationList associations{};

        auto entry = std::make_unique<Entry>(
            bus, std::string(OBJ_ENTRY) + "/" + std::to_string(entryId),
            entryId, 100, level, std::move(entryMessage),
            std::move(additionalData), std::move(associations), "fw",
            tempEntryPath.string(), *manager);

        entry->resolved(resolved, true);
        serialize(*entry, tempEntryDir);
        return tempEntryPath;
    }

    void processPendingEvents()
    {
        sdeventplus::Event event(sdEvent);
        event.run(std::chrono::milliseconds(1));
    }

    sdbusplus::SdBusMock sdbusMock;
    sdbusplus::bus_t bus{nullptr};
    std::unique_ptr<internal::Manager> manager;
    sd_event* sdEvent{};
    fs::path tempEntryDir;
    fs::path repoEntryPath;
    uint32_t entryId{};
};

TEST_F(LogManagerRedundantBMCSyncTest, EventLogRestore)
{
    manager->setupErrorFileWatch();

    auto tempEntryPath = writeTempSerializedEntry(Entry::Level::Informational,
                                                  "first message", false);

    // Simulate sync moving the serialized event log into the watched repo
    // path. The inotify handler should restore the entry from disk.
    fs::rename(tempEntryPath, repoEntryPath);
    processPendingEvents();

    ASSERT_TRUE(manager->entries.contains(entryId));
    EXPECT_EQ(manager->entries.at(entryId)->message(), "first message");
    EXPECT_EQ(manager->entries.at(entryId)->resolved(), false);
}

TEST_F(LogManagerRedundantBMCSyncTest, EventLogRefresh)
{
    manager->setupErrorFileWatch();

    auto tempEntryPath = writeTempSerializedEntry(Entry::Level::Informational,
                                                  "first message", false);

    // First move restores the synced event log entry into memory.
    fs::rename(tempEntryPath, repoEntryPath);
    processPendingEvents();

    ASSERT_TRUE(manager->entries.contains(entryId));
    EXPECT_EQ(manager->entries.at(entryId)->message(), "first message");
    EXPECT_EQ(manager->entries.at(entryId)->resolved(), false);

    tempEntryPath =
        writeTempSerializedEntry(Entry::Level::Error, "updated message", true);

    // Move an updated version of the same serialized file into place to
    // simulate sync refreshing an existing event log entry.
    fs::rename(tempEntryPath, repoEntryPath);
    processPendingEvents();

    ASSERT_TRUE(manager->entries.contains(entryId));
    EXPECT_EQ(manager->entries.at(entryId)->message(), "updated message");
    EXPECT_EQ(manager->entries.at(entryId)->resolved(), true);
}

TEST_F(LogManagerRedundantBMCSyncTest, EventLogDelete)
{
    manager->setupErrorFileWatch();

    auto tempEntryPath = writeTempSerializedEntry(Entry::Level::Informational,
                                                  "first message", false);

    // First restore the entry so the delete path has something to remove.
    fs::rename(tempEntryPath, repoEntryPath);
    processPendingEvents();

    ASSERT_TRUE(manager->entries.contains(entryId));

    // Simulate sync removing the serialized event log file. The inotify
    // handler should remove the corresponding in-memory entry.
    fs::remove(repoEntryPath);
    processPendingEvents();

    EXPECT_FALSE(manager->entries.contains(entryId));
}

} // namespace phosphor::logging::test
