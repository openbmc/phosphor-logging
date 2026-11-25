#include "lib/lg2_logger.hpp"

#include <phosphor-logging/lg2/log_control.hpp>
#include <sdbusplus/bus.hpp>

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <variant>

#include <gtest/gtest.h>

namespace
{

constexpr auto busName = "xyz.openbmc_project.test.LogControl";
constexpr auto objectPath = "/org/freedesktop/LogControl1";
constexpr auto interfaceName = "org.freedesktop.LogControl1";
constexpr auto propertiesInterface = "org.freedesktop.DBus.Properties";

/** Runs a bus with the LogControl1 object on a background thread. */
class TestLogControl : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        lg2::details::log_threshold().store(lg2::level::info);

        control = std::make_unique<lg2::log_control>(server);
        server.request_name(busName);

        worker = std::thread([this]() {
            while (!done.load())
            {
                server.wait(uint64_t{50000});
                server.process_discard();
            }
        });
    }

    void TearDown() override
    {
        done.store(true);
        worker.join();
        control.reset();
    }

    std::string getProperty(const std::string& name)
    {
        auto m = client.new_method_call(busName, objectPath,
                                        propertiesInterface, "Get");
        m.append(interfaceName, name);

        std::variant<std::string> value;
        client.call(m).read(value);
        return std::get<std::string>(value);
    }

    void setProperty(const std::string& name, const std::string& value)
    {
        auto m = client.new_method_call(busName, objectPath,
                                        propertiesInterface, "Set");
        m.append(interfaceName, name, std::variant<std::string>(value));
        client.call(m);
    }

    // Separate connections: the server is processed on the worker thread.
    sdbusplus::bus_t server = sdbusplus::bus::new_bus();
    sdbusplus::bus_t client = sdbusplus::bus::new_bus();
    std::unique_ptr<lg2::log_control> control;
    std::atomic<bool> done{false};
    std::thread worker;
};

TEST_F(TestLogControl, DefaultsMatchStartupState)
{
    EXPECT_EQ(getProperty("LogLevel"), "info");
    EXPECT_EQ(getProperty("LogTarget"), "journal");
    EXPECT_FALSE(getProperty("SyslogIdentifier").empty());
}

TEST_F(TestLogControl, SetLogLevelUpdatesThreshold)
{
    setProperty("LogLevel", "debug");
    EXPECT_EQ(lg2::details::log_threshold().load(), lg2::level::debug);
    EXPECT_EQ(getProperty("LogLevel"), "debug");

    setProperty("LogLevel", "err");
    EXPECT_EQ(lg2::details::log_threshold().load(), lg2::level::error);
    EXPECT_EQ(getProperty("LogLevel"), "err");
}

TEST_F(TestLogControl, EveryLevelNameRoundTrips)
{
    for (const auto& name : {"emerg", "alert", "crit", "err", "warning",
                             "notice", "info", "debug"})
    {
        setProperty("LogLevel", name);
        EXPECT_EQ(getProperty("LogLevel"), name);
    }
}

TEST_F(TestLogControl, InvalidLogLevelIsRejected)
{
    EXPECT_THROW(setProperty("LogLevel", "bogus"), sdbusplus::exception_t);
    EXPECT_EQ(lg2::details::log_threshold().load(), lg2::level::info);
}

TEST_F(TestLogControl, JournalLogTargetIsAccepted)
{
    setProperty("LogTarget", "journal");
    EXPECT_EQ(getProperty("LogTarget"), "journal");
}

TEST_F(TestLogControl, UnsupportedLogTargetIsRejected)
{
    EXPECT_THROW(setProperty("LogTarget", "console"), sdbusplus::exception_t);
    EXPECT_THROW(setProperty("LogTarget", "kmsg"), sdbusplus::exception_t);
    EXPECT_EQ(getProperty("LogTarget"), "journal");
}

TEST_F(TestLogControl, SyslogIdentifierIsReadOnly)
{
    EXPECT_THROW(setProperty("SyslogIdentifier", "something"),
                 sdbusplus::exception_t);
}

} // namespace
