
#include "audit_manager.hpp"

#include <poll.h>
#include <sys/inotify.h>
#include <systemd/sd-bus.h>
#include <systemd/sd-journal.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <phosphor-logging/lg2.hpp>

namespace phosphor
{
namespace audit
{

void Manager::notify(uint64_t transactionId)
{

    /* auto additionalData =  */ getAdditionalData(transactionId);

    return;
}

std::vector<std::string> Manager::getAdditionalData(uint64_t transactionId)
{
    static constexpr auto transactionIdVar = std::string_view{"TRANSACTION_ID"};
    static constexpr auto transactionIdVarSize = transactionIdVar.size();
    static constexpr auto transactionIdVarOffset = transactionIdVarSize + 1;
    static std::set<std::string> metalist = {
        "EVENT_TYPE", "EVENT_RC", "EVENT_USER", "EVENT_ADDR", "MESSAGE"};

    // Flush all the pending log messages into the journal
    journalSync();

    sd_journal* j = nullptr;
    int rc = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (rc < 0)
    {
        lg2::error("Failed to open journal: {ERROR}", "ERROR", strerror(-rc));
        return {};
    }

    std::vector<std::string> additionalData;
    std::string transactionIdStr = std::to_string(transactionId);

    // Read the journal from the end to get the most recent entry first.
    // The result from the sd_journal_get_data() is of the form VARIABLE=value.
    SD_JOURNAL_FOREACH_BACKWARDS(j)
    {
        const char* data = nullptr;
        size_t length = 0;

        // Look for the transaction id metadata variable
        rc = sd_journal_get_data(j, transactionIdVar.data(),
                                 (const void**)&data, &length);
        if (rc < 0)
        {
            // This journal entry does not have the TRANSACTION_ID metadata
            // variable.
            continue;
        }

        // journald does not guarantee that sd_journal_get_data() returns NULL
        // terminated strings, so need to specify the size to use to compare,
        // use the returned length instead of anything that relies on NULL
        // terminators like strlen(). The data variable is in the form of
        // 'TRANSACTION_ID=1234'. Remove the TRANSACTION_ID characters plus the
        // (=) sign to do the comparison. 'data + transactionIdVarOffset' will
        // be in the form of '1234'. 'length - transactionIdVarOffset' will be
        // the length of '1234'.
        if ((length <= (transactionIdVarOffset)) ||
            (transactionIdStr.compare(0, transactionIdStr.size(),
                                      data + transactionIdVarOffset,
                                      length - transactionIdVarOffset) != 0))
        {
            // The value of the TRANSACTION_ID metadata is not the requested
            // transaction id number.
            continue;
        }

        // Search for all metadata variables in the current journal entry.
        for (auto meta : metalist)
        {
            if (sd_journal_get_data(j, meta.c_str(), (const void**)&data,
                                    &length) < 0)
            {
                // Metadata variable not found, check next metadata variable.
                continue;
            }

            // Metadata variable found, save it and remove it from the set.
            additionalData.emplace_back(data, length);
        }
    }
    sd_journal_close(j);

    return additionalData;
}

void Manager::journalSync()
{
    using namespace std::chrono;
    bool syncRequested = false;
    auto fd = -1;
    auto rc = -1;
    auto wd = -1;
    auto bus = sdbusplus::bus::new_default();

    auto start =
        duration_cast<microseconds>(steady_clock::now().time_since_epoch())
            .count();

    // Each time an error log is committed, a request to sync the journal must
    // occur and block that error log commit until it completes. A 5sec block is
    // done to allow sufficient time for the journal to be synced.
    //
    // Number of loop iterations = 3 for the following reasons:
    // Iteration #1: Requests a journal sync by killing the journald service.
    // Iteration #2: Setup an inotify watch to monitor the synced file that
    //               journald updates with the timestamp the last time the
    //               journal was flushed.
    // Iteration #3: Poll to wait until inotify reports an event which blocks
    //               the error log from being commited until the sync completes.
    constexpr auto maxRetry = 3;
    for (int i = 0; i < maxRetry; i++)
    {
        // Read timestamp from synced file
        constexpr auto syncedPath = "/run/systemd/journal/synced";
        std::ifstream syncedFile(syncedPath);
        if (syncedFile.fail())
        {
            // If the synced file doesn't exist, a sync request will create it.
            if (errno != ENOENT)
            {
                lg2::error(
                    "Failed to open journal synced file {FILENAME}: {ERROR}",
                    "FILENAME", syncedPath, "ERROR", strerror(errno));
                return;
            }
        }
        else
        {
            // Only read the synced file if it exists.
            // See if a sync happened by now
            std::string timestampStr;
            std::getline(syncedFile, timestampStr);
            auto timestamp = std::stoll(timestampStr);
            if (timestamp >= start)
            {
                break;
            }
        }

        // Let's ask for a sync, but only once
        if (!syncRequested)
        {
            syncRequested = true;

            constexpr auto JOURNAL_UNIT = "systemd-journald.service";
            auto signal = SIGRTMIN + 1;

            auto method = bus.new_method_call(
                "org.freedesktop.systemd1", "/org/freedesktop/systemd1",
                "org.freedesktop.systemd1.Manager", "KillUnit");
            method.append(JOURNAL_UNIT, "main", signal);
            bus.call(method);
            if (method.is_method_error())
            {
                lg2::error("Failed to kill journal service");
                break;
            }

            continue;
        }

        // Let's install the inotify watch, if we didn't do that yet. This watch
        // monitors the syncedFile for when journald updates it with a newer
        // timestamp. This means the journal has been flushed.
        if (fd < 0)
        {
            fd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
            if (fd < 0)
            {
                lg2::error("Failed to create inotify watch: {ERROR}", "ERROR",
                           strerror(errno));
                return;
            }

            constexpr auto JOURNAL_RUN_PATH = "/run/systemd/journal";
            wd = inotify_add_watch(fd, JOURNAL_RUN_PATH,
                                   IN_MOVED_TO | IN_DONT_FOLLOW | IN_ONLYDIR);
            if (wd < 0)
            {
                lg2::error("Failed to watch journal directory: {PATH}: {ERROR}",
                           "PATH", JOURNAL_RUN_PATH, "ERROR", strerror(errno));
                close(fd);
                return;
            }
            continue;
        }

        // Let's wait until inotify reports an event
        struct pollfd fds = {
            fd,
            POLLIN,
            0,
        };
        constexpr auto pollTimeout = 5; // 5 seconds
        rc = poll(&fds, 1, pollTimeout * 1000);
        if (rc < 0)
        {
            lg2::error("Failed to add event: {ERROR}", "ERROR",
                       strerror(errno));
            inotify_rm_watch(fd, wd);
            close(fd);
            return;
        }
        else if (rc == 0)
        {
            lg2::info("Poll timeout ({TIMEOUT}), no new journal synced data",
                      "TIMEOUT", pollTimeout);
            break;
        }

        // Read from the specified file descriptor until there is no new data,
        // throwing away everything read since the timestamp will be read at the
        // beginning of the loop.
        constexpr auto maxBytes = 64;
        uint8_t buffer[maxBytes];
        while (read(fd, buffer, maxBytes) > 0)
            ;
    }

    if (fd != -1)
    {
        if (wd != -1)
        {
            inotify_rm_watch(fd, wd);
        }
        close(fd);
    }

    return;
}

} // namespace audit
} // namespace phosphor