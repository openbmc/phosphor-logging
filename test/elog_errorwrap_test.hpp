#include "log_manager.hpp"
#include "xyz/openbmc_project/Common/error.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sdbusplus/bus.hpp>
#include <experimental/filesystem>
#include <phosphor-logging/elog.hpp>
#include <phosphor-logging/log.hpp>
#include <phosphor-logging/elog-errors.hpp>
#include "elog_serialize.hpp"
#include "config.h"

namespace phosphor
{
namespace logging
{
namespace internal
{

namespace fs = std::experimental::filesystem;

class journalInterface
{
    public:
        virtual void journalSync() = 0;
        virtual int sd_journal_open(sd_journal** j, int k) = 0;
        virtual int sd_journal_get_data(sd_journal* j,
                                        const char* transactionIdVar,
                                        const void** data,
                                        size_t length) = 0;
        virtual void sd_journal_close(sd_journal* j) = 0;
};

class journalImpl : public journalInterface
{
    public:
        void journalSync();
        int sd_journal_open(sd_journal** j, int k);
        int sd_journal_get_data(sd_journal* j,
                                const char* transactionIdVar,
                                const void** data,
                                size_t length);
        void sd_journal_close(sd_journal* j);
};



int journalImpl::sd_journal_open(sd_journal** j, int k)
{
    return ::sd_journal_open(j, k);
}

void journalImpl::journalSync()
{
}

int journalImpl::sd_journal_get_data(sd_journal* j,
                                     const char* transactionIdVar,
                                     const void** data,
                                     size_t length)
{
    return ::sd_journal_get_data(j, transactionIdVar, data, &length);
}

void journalImpl::sd_journal_close(sd_journal* j)
{
    return ::sd_journal_close(j);
}


class  MockJournal : public Manager
{
    public:
        MockJournal(sdbusplus::bus::bus& bus,
                    const char* objPath): Manager(bus, objPath) {};
        MOCK_METHOD0(journalSync, void());
        MOCK_METHOD2(sd_journal_open, int(sd_journal**, int));
        MOCK_METHOD4(sd_journal_get_data, int(sd_journal*,
                                              const char*,
                                              const void**, size_t));
        MOCK_METHOD1(sd_journal_close, void(sd_journal*));
};



class TestLogManager : public testing::Test
{
    public:
        sdbusplus::bus::bus bus;
        MockJournal manager;
        TestLogManager()
            : bus(sdbusplus::bus::new_default()),
              manager(bus, "/xyz/openbmc_test/abc")
        {
            fs::create_directories(ERRLOG_PERSIST_PATH);
        }

        ~TestLogManager()
        {
            fs::remove_all(ERRLOG_PERSIST_PATH);
        }

};


}// nmaespace internal
}// namespace logging
}// namespace phosphor
