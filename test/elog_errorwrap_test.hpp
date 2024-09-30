#include "config.h"

#include "elog_serialize.hpp"
#include "log_manager.hpp"
#include "paths.hpp"
#include "xyz/openbmc_project/Common/error.hpp"

#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/elog.hpp>
#include <phosphor-logging/log.hpp>
#include <sdbusplus/bus.hpp>

#include <filesystem>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace phosphor
{
namespace logging
{
namespace internal
{

namespace fs = std::filesystem;

class journalInterface
{
  public:
    virtual ~journalInterface() = default;

    virtual void journalSync() = 0;
    virtual int sd_journal_open(sd_journal** j, int k) = 0;
    virtual int sd_journal_get_data(sd_journal* j, const char* transactionIdVar,
                                    const void** data, size_t length) = 0;
    virtual void sd_journal_close(sd_journal* j) = 0;
};

class journalImpl : public journalInterface
{
  public:
    void journalSync() override;
    int sd_journal_open(sd_journal** j, int k) override;
    int sd_journal_get_data(sd_journal* j, const char* transactionIdVar,
                            const void** data, size_t length) override;
    void sd_journal_close(sd_journal* j) override;
};

int journalImpl::sd_journal_open(sd_journal**, int)
{
    return 1;
}

void journalImpl::journalSync()
{
    return;
}

int journalImpl::sd_journal_get_data(sd_journal*, const char*, const void**,
                                     size_t)
{
    return 1;
}

void journalImpl::sd_journal_close(sd_journal*)
{
    return;
}

class MockJournal : public Manager
{
  public:
    MockJournal(sdbusplus::bus_t& bus, const char* objPath) :
        Manager(bus, objPath) {};
    MOCK_METHOD0(journalSync, void());
    MOCK_METHOD2(sd_journal_open, int(sd_journal**, int));
    MOCK_METHOD4(sd_journal_get_data,
                 int(sd_journal*, const char*, const void**, size_t));
    MOCK_METHOD1(sd_journal_close, void(sd_journal*));
};

class TestLogManager : public testing::Test
{
  public:
    sdbusplus::bus_t bus;
    MockJournal manager;
    TestLogManager() :
        bus(sdbusplus::bus::new_default()),
        manager(bus, "/xyz/openbmc_test/abc")
    {
        fs::create_directories(paths::error());
    }

    ~TestLogManager()
    {
        // Leave the directory as other testcases use it and they
        // may be running in parallel from other jobs.
    }
};

} // namespace internal
} // namespace logging
} // namespace phosphor
