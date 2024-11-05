#include "config.h"

#include "log_manager.hpp"
#include "paths.hpp"

#include <phosphor-logging/commit.hpp>
#include <sdbusplus/async.hpp>
#include <sdbusplus/server/manager.hpp>
#include <xyz/openbmc_project/Logging/Entry/client.hpp>
#include <xyz/openbmc_project/Logging/event.hpp>

#include <thread>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace phosphor::logging::test
{
using LoggingCleared = sdbusplus::event::xyz::openbmc_project::Logging::Cleared;
using LoggingEntry = sdbusplus::client::xyz::openbmc_project::logging::Entry<>;

// Fixture to spawn the log-manager for dbus-based testing.
class TestLogManagerDbus : public ::testing::Test
{
  protected:
    // Create the daemon and sdbusplus::async::contexts.
    void SetUp() override
    {
        // The daemon requires directories to be created first.
        std::filesystem::create_directories(phosphor::logging::paths::error());

        data = std::make_unique<fixture_data>();
    }

    // Stop the daemon, etc.
    void TearDown() override
    {
        data.reset();
    }

    /** Run a client task, wait for it to complete, and stop daemon. */
    template <typename T>
    void run(T&& t)
    {
        data->client_ctx.spawn(std::move(t) | stdexec::then([this]() {
                                   data->stop(data->client_ctx);
                               }));
        data->client_ctx.run();
    }

    // Data for the fixture.
    struct fixture_data
    {
        fixture_data() :
            client_ctx(), server_ctx(), objManager(server_ctx, OBJ_LOGGING),
            iMgr(server_ctx, OBJ_INTERNAL), mgr(server_ctx, OBJ_LOGGING, iMgr)
        {
            // Create a thread for the daemon.
            task = std::thread([this]() {
                server_ctx.request_name(BUSNAME_LOGGING);
                server_ctx.run();
            });
        }

        ~fixture_data()
        {
            // Stop the server and wait for the thread to exit.
            stop(server_ctx);
            task.join();
        }

        // Spawn a task to gracefully shutdown an sdbusplus::async::context
        static void stop(sdbusplus::async::context& ctx)
        {
            ctx.spawn(stdexec::just() |
                      stdexec::then([&ctx]() { ctx.request_stop(); }));
        }

        sdbusplus::async::context client_ctx;
        sdbusplus::async::context server_ctx;
        sdbusplus::server::manager_t objManager;
        internal::Manager iMgr;
        Manager mgr;
        std::thread task;
    };

    std::unique_ptr<fixture_data> data;

    static constexpr auto journal_unavailable = "UNAVAILABLE";
    std::string last_journal_entry()
    {
        if constexpr (LG2_COMMIT_JOURNAL)
        {
            // When running under Docker, the journal is not available and
            // sd-journal calls just silently pass.  Return a string to make
            // it obvious.
            if (!std::filesystem::exists("/run/systemd/journal/socket"))
            {
                return journal_unavailable;
            }

            sd_journal* j = nullptr;

            sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
            sd_journal_add_match(j, "SYSLOG_IDENTIFIER=test_manager_dbus_tests",
                                 SIZE_MAX);

            SD_JOURNAL_FOREACH_BACKWARDS(j)
            {
                const char* data = nullptr;
                size_t length = 0;

                sd_journal_get_data(j, "MESSAGE", (const void**)&data, &length);

                std::string entry(data, length);
                if (entry.contains("OPENBMC_MESSAGE_ID="))
                {
                    return entry;
                }
            }
        }

        return "";
    }
};

// Ensure we can successfully create and throw an sdbusplus event.
TEST_F(TestLogManagerDbus, GenerateSimpleEvent)
{
    EXPECT_THROW(
        { throw LoggingCleared("NUMBER_OF_LOGS", 1); }, LoggingCleared);
    return;
}

// Call the synchronous version of the commit function and verify that the
// daemon gives us a path.
TEST_F(TestLogManagerDbus, CallCommitSync)
{
    auto path = lg2::commit(LoggingCleared("NUMBER_OF_LOGS", 3));

    if constexpr (LG2_COMMIT_DBUS)
    {
        ASSERT_FALSE(path.str.empty());
        EXPECT_THAT(
            path.str,
            ::testing::StartsWith(
                std::filesystem::path(LoggingEntry::namespace_path::value) /
                LoggingEntry::namespace_path::entry));
    }

    if constexpr (LG2_COMMIT_JOURNAL)
    {
        auto entry = last_journal_entry();
        if (entry != journal_unavailable)
        {
            EXPECT_THAT(entry, ::testing::HasSubstr(
                                   "\"xyz.openbmc_project.Logging.Cleared\":"));
            EXPECT_THAT(entry, ::testing::HasSubstr("\"NUMBER_OF_LOGS\":3"));
        }
    }
}

// Call the asynchronous version of the commit function and verify that the
// metadata is saved correctly.
TEST_F(TestLogManagerDbus, CallCommitAsync)
{
    sdbusplus::message::object_path path{};
    std::string log_count{};
    pid_t pid = 0;
    std::string source_file{};

    auto create_log = [&, this]() -> sdbusplus::async::task<> {
        // Log an event.
        path = co_await lg2::commit(data->client_ctx,
                                    LoggingCleared("NUMBER_OF_LOGS", 6));

        if constexpr (LG2_COMMIT_DBUS)
        {
            // Grab the additional data.
            auto additionalData = co_await LoggingEntry(data->client_ctx)
                                      .service(Entry::default_service)
                                      .path(path.str)
                                      .additional_data();

            // Extract the NUMBER_OF_LOGS, PID, and CODE_FILE.
            for (const auto& value : additionalData)
            {
                auto getValue = [&value]() {
                    return value.substr(value.find_first_of('=') + 1);
                };

                if (value.starts_with("NUMBER_OF_LOGS="))
                {
                    log_count = getValue();
                }
                if (value.starts_with("_PID="))
                {
                    pid = std::stoull(getValue());
                }
                if (value.starts_with("_CODE_FILE="))
                {
                    source_file = getValue();
                }
            }
        }

        co_return;
    };

    run(create_log());

    if constexpr (LG2_COMMIT_DBUS)
    {
        ASSERT_FALSE(path.str.empty());
        ASSERT_FALSE(log_count.empty());

        EXPECT_THAT(
            path.str,
            ::testing::StartsWith(
                std::filesystem::path(LoggingEntry::namespace_path::value) /
                LoggingEntry::namespace_path::entry));

        EXPECT_EQ(log_count, "6");
        EXPECT_EQ(pid, getpid());
        EXPECT_EQ(source_file, std::source_location::current().file_name());
    }

    if constexpr (LG2_COMMIT_JOURNAL)
    {
        auto entry = last_journal_entry();
        if (entry != journal_unavailable)
        {
            EXPECT_THAT(entry, ::testing::HasSubstr(
                                   "\"xyz.openbmc_project.Logging.Cleared\":"));
            EXPECT_THAT(entry, ::testing::HasSubstr("\"NUMBER_OF_LOGS\":6"));
        }
    }
}

} // namespace phosphor::logging::test
