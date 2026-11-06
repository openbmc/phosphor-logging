#include "elog_errorwrap_test.hpp"

#include <sdeventplus/event.hpp>

#include <chrono>
#include <fstream>

namespace phosphor
{
namespace logging
{
namespace internal
{

namespace
{

size_t countFiles(const fs::path& dir)
{
    std::error_code ec;
    size_t n = 0;
    for ([[maybe_unused]] const auto& f : fs::directory_iterator(dir, ec))
    {
        ++n;
    }
    return n;
}

// Run the event loop until nothing is left to dispatch.
void drainEvents()
{
    auto event = sdeventplus::Event::get_default();
    for (int i = 0; i < 100000; ++i)
    {
        if (event.run(std::chrono::microseconds(0)) <= 0)
        {
            break;
        }
    }
}

void clearPersistedState()
{
    std::error_code ec;
    fs::remove_all(paths::error(), ec);
    fs::remove_all(paths::error_json(), ec);
    fs::remove_all(paths::pending_removal(), ec);
    fs::remove_all(paths::artifact(), ec);
    fs::create_directories(paths::error());
    fs::create_directories(paths::error_json());
}

} // namespace

class TestLogManagerClear : public TestLogManager
{
  public:
    TestLogManagerClear()
    {
        manager.eraseAll();
        drainEvents();
        clearPersistedState();
    }
};

TEST_F(TestLogManagerClear, largeClearRemovesFilesInBackground)
{
    // Error severity: informational entries are capped (error_info_cap,
    // default 10), which would keep the log below the staging threshold.
    for (size_t i = 0; i < 60; i++)
    {
        manager.commitWithLvl(i, "FOO", 3);
    }
    ASSERT_EQ(60u, countFiles(paths::error()));

    EXPECT_EQ(60u, manager.eraseAll());

    // The live directories are empty at once; the files wait in one
    // committed (not .partial) staging directory.
    EXPECT_EQ(0u, countFiles(paths::error()));
    EXPECT_EQ(0u, manager.getInfoErrSize());
    std::vector<fs::path> staged;
    for (const auto& d : fs::directory_iterator(paths::pending_removal()))
    {
        staged.push_back(d.path());
    }
    ASSERT_EQ(1u, staged.size());
    EXPECT_NE(".partial", staged[0].extension());
    EXPECT_EQ(60u, countFiles(staged[0] / paths::error().filename()));

    drainEvents();
    EXPECT_EQ(0u, countFiles(paths::pending_removal()));
}

TEST_F(TestLogManagerClear, smallClearRemovesFilesDirectly)
{
    for (size_t i = 0; i < 3; i++)
    {
        manager.commitWithLvl(i, "FOO", 6);
    }

    EXPECT_EQ(3u, manager.eraseAll());

    EXPECT_EQ(0u, countFiles(paths::error()));
    EXPECT_EQ(0u, countFiles(paths::pending_removal()));
}

TEST_F(TestLogManagerClear, nonEntryFilesAreKept)
{
    // Error severity: informational entries are capped (error_info_cap,
    // default 10), which would keep the log below the staging threshold.
    for (size_t i = 0; i < 60; i++)
    {
        manager.commitWithLvl(i, "FOO", 3);
    }
    std::ofstream(paths::error() / "not-an-entry") << "x";

    manager.eraseAll();
    drainEvents();

    EXPECT_TRUE(fs::exists(paths::error() / "not-an-entry"));
    EXPECT_EQ(1u, countFiles(paths::error()));
    EXPECT_EQ(0u, countFiles(paths::pending_removal()));
}

void createWithArtifact(MockJournal& manager)
{
    manager.create("FOO", Severity::Error,
                   {{"_EXTENSIONS",
                     R"({"xyz.openbmc_project.Logging.Extension.CPER.Raw":)"
                     R"({"Data":[1,2,3,4]}})"}});
}

TEST_F(TestLogManagerClear, largeClearRemovesArtifactsInBackground)
{
    for (size_t i = 0; i < 60; i++)
    {
        createWithArtifact(manager);
    }
    ASSERT_EQ(60u, countFiles(paths::artifact()));
    // A file no entry refers to is not removed.
    std::ofstream(paths::artifact() / "not-an-artifact") << "x";

    manager.eraseAll();

    // Moved aside with the entry files, not removed in the handler.
    EXPECT_EQ(1u, countFiles(paths::artifact()));
    EXPECT_TRUE(fs::exists(paths::artifact() / "not-an-artifact"));
    std::vector<fs::path> staged;
    for (const auto& d : fs::directory_iterator(paths::pending_removal()))
    {
        staged.push_back(d.path());
    }
    ASSERT_EQ(1u, staged.size());
    EXPECT_EQ(60u, countFiles(staged[0] / paths::artifact().filename()));

    drainEvents();
    EXPECT_EQ(1u, countFiles(paths::artifact()));
    EXPECT_EQ(0u, countFiles(paths::pending_removal()));
}

TEST_F(TestLogManagerClear, smallClearRemovesArtifactsDirectly)
{
    for (size_t i = 0; i < 3; i++)
    {
        createWithArtifact(manager);
    }
    ASSERT_EQ(3u, countFiles(paths::artifact()));

    manager.eraseAll();

    EXPECT_EQ(0u, countFiles(paths::artifact()));
    EXPECT_EQ(0u, countFiles(paths::pending_removal()));
}

// Stage the files of one real entry as a clear would, then erase the
// entry from memory, as if the daemon had restarted.
fs::path stageOneEntry(MockJournal& manager, const std::string& stageName)
{
    manager.commitWithLvl(0, "FOO", 6);
    auto id = std::to_string(manager.lastEntryID());
    auto stage = paths::pending_removal() / stageName;
    fs::create_directories(stage / paths::error().filename());
    fs::create_directories(stage / paths::error_json().filename());
    fs::copy_file(paths::error() / id, stage / paths::error().filename() / id);
    fs::copy_file(paths::error_json() / (id + ".json"),
                  stage / paths::error_json().filename() / (id + ".json"));
    manager.erase(manager.lastEntryID());
    EXPECT_EQ(0u, manager.getInfoErrSize());
    EXPECT_FALSE(fs::exists(paths::error() / id));
    return paths::error() / id;
}

TEST_F(TestLogManagerClear, unfinishedClearIsRolledBackOnRestore)
{
    // A .partial staging directory is what a restart in the middle of
    // eraseAll() leaves: its files go back and the entry is restored.
    auto file = stageOneEntry(manager, "1-1.partial");

    manager.restore();
    drainEvents();

    EXPECT_TRUE(fs::exists(file));
    EXPECT_TRUE(fs::exists(
        file.parent_path().parent_path() / paths::error_json().filename() /
        (file.filename().string() + ".json")));
    EXPECT_EQ(1u, manager.getInfoErrSize());
    EXPECT_EQ(0u, countFiles(paths::pending_removal()));
}

TEST_F(TestLogManagerClear, finishedClearIsRemovedOnRestore)
{
    // A committed staging directory holds only files of erased entries:
    // they are removed and the entry stays erased.
    auto file = stageOneEntry(manager, "1-1");

    manager.restore();
    drainEvents();

    EXPECT_FALSE(fs::exists(file));
    EXPECT_EQ(0u, manager.getInfoErrSize());
    EXPECT_EQ(0u, countFiles(paths::pending_removal()));
}

} // namespace internal
} // namespace logging
} // namespace phosphor
