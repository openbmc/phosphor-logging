/**
 * Copyright © 2020 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "config.h"

#include "util.hpp"

#include <poll.h>
#include <sys/inotify.h>
#include <systemd/sd-bus.h>
#include <systemd/sd-journal.h>
#include <unistd.h>

#include <phosphor-logging/lg2.hpp>
#include <sdbusplus/bus.hpp>

#include <chrono>

namespace phosphor::logging::util
{

std::optional<std::string> getOSReleaseValue(const std::string& key)
{
    std::ifstream versionFile{BMC_VERSION_FILE};
    std::string line;
    std::string keyPattern{key + '='};

    while (std::getline(versionFile, line))
    {
        if (line.substr(0, keyPattern.size()).find(keyPattern) !=
            std::string::npos)
        {
            // If the value isn't surrounded by quotes, then pos will be
            // npos + 1 = 0, and the 2nd arg to substr() will be npos
            // which means get the rest of the string.
            auto value = line.substr(keyPattern.size());
            std::size_t pos = value.find_first_of('"') + 1;
            return value.substr(pos, value.find_last_of('"') - pos);
        }
    }

    return std::nullopt;
}

void journalSync()
{
    bool syncRequested = false;
    auto fd = -1;
    auto rc = -1;
    auto wd = -1;
    auto bus = sdbusplus::bus::new_default();

    auto start = std::chrono::duration_cast<std::chrono::microseconds>(
                     std::chrono::steady_clock::now().time_since_epoch())
                     .count();

    // Make a request to sync the journal with the SIGRTMIN+1 signal and
    // block until it finishes, waiting at most 5 seconds.
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

            auto method = bus.new_method_call(SYSTEMD_BUSNAME, SYSTEMD_PATH,
                                              SYSTEMD_INTERFACE, "KillUnit");
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

} // namespace phosphor::logging::util
