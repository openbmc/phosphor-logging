#include <fstream>
#include <future>
#include <iostream>
#include <chrono>
#include <cstdio>
#include <poll.h>
#include <set>
#include <string>
#include <vector>
#include <sdbusplus/vtable.hpp>
#include <sys/inotify.h>
#include <systemd/sd-bus.h>
#include <systemd/sd-journal.h>
#include <unistd.h>
#include "config.h"
#include "elog_entry.hpp"
#include <phosphor-logging/log.hpp>
#include "log_manager.hpp"
#include "elog_meta.hpp"
#include "elog_serialize.hpp"

using namespace phosphor::logging;
using namespace std::chrono;
extern const std::map<metadata::Metadata,
                      std::function<metadata::associations::Type>> meta;

namespace phosphor
{
namespace logging
{
namespace internal
{
inline auto getLevel(const std::string& errMsg)
{
    auto reqLevel = Entry::Level::Error; // Default to Error

    auto levelmap = g_errLevelMap.find(errMsg);
    if (levelmap != g_errLevelMap.end())
    {
        reqLevel = static_cast<Entry::Level>(levelmap->second);
    }

    return reqLevel;
}

void Manager::commit(uint64_t transactionId, std::string errMsg)
{
    auto level = getLevel(errMsg);
    _commit(transactionId, std::move(errMsg), level);
}

void Manager::commitWithLvl(uint64_t transactionId, std::string errMsg,
                            uint32_t errLvl)
{
    _commit(transactionId, std::move(errMsg),
            static_cast<Entry::Level>(errLvl));
}

void Manager::_commit(uint64_t transactionId, std::string&& errMsg,
                      Entry::Level errLvl)
{
    if (errLvl < Entry::sevLowerLimit)
    {
        if (realErrors.size() >= ERROR_CAP)
        {
            erase(realErrors.front());
        }
    }
    else
    {
        if (infoErrors.size() >= ERROR_INFO_CAP)
        {
            erase(infoErrors.front());
        }
    }
    constexpr const auto transactionIdVar = "TRANSACTION_ID";
    // Length of 'TRANSACTION_ID' string.
    constexpr const auto transactionIdVarSize = strlen(transactionIdVar);
    // Length of 'TRANSACTION_ID=' string.
    constexpr const auto transactionIdVarOffset = transactionIdVarSize + 1;

    // Flush all the pending log messages into the journal
    journalSync();

    sd_journal *j = nullptr;
    int rc = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (rc < 0)
    {
        logging::log<logging::level::ERR>("Failed to open journal",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
        return;
    }

    std::string transactionIdStr = std::to_string(transactionId);
    std::set<std::string> metalist;
    auto metamap = g_errMetaMap.find(errMsg);
    if (metamap != g_errMetaMap.end())
    {
        metalist.insert(metamap->second.begin(), metamap->second.end());
    }

    //Add _PID field information in AdditionalData.
    metalist.insert("_PID");

    std::vector<std::string> additionalData;

    // Read the journal from the end to get the most recent entry first.
    // The result from the sd_journal_get_data() is of the form VARIABLE=value.
    SD_JOURNAL_FOREACH_BACKWARDS(j)
    {
        const char *data = nullptr;
        size_t length = 0;

        // Look for the transaction id metadata variable
        rc = sd_journal_get_data(j, transactionIdVar, (const void **)&data,
                                &length);
        if (rc < 0)
        {
            // This journal entry does not have the TRANSACTION_ID
            // metadata variable.
            continue;
        }

        // journald does not guarantee that sd_journal_get_data() returns NULL
        // terminated strings, so need to specify the size to use to compare,
        // use the returned length instead of anything that relies on NULL
        // terminators like strlen().
        // The data variable is in the form of 'TRANSACTION_ID=1234'. Remove
        // the TRANSACTION_ID characters plus the (=) sign to do the comparison.
        // 'data + transactionIdVarOffset' will be in the form of '1234'.
        // 'length - transactionIdVarOffset' will be the length of '1234'.
        if ((length <= (transactionIdVarOffset)) ||
            (transactionIdStr.compare(0,
                                      transactionIdStr.size(),
                                      data + transactionIdVarOffset,
                                      length - transactionIdVarOffset) != 0))
        {
            // The value of the TRANSACTION_ID metadata is not the requested
            // transaction id number.
            continue;
        }

        // Search for all metadata variables in the current journal entry.
        for (auto i = metalist.cbegin(); i != metalist.cend();)
        {
            rc = sd_journal_get_data(j, (*i).c_str(),
                                    (const void **)&data, &length);
            if (rc < 0)
            {
                // Metadata variable not found, check next metadata variable.
                i++;
                continue;
            }

            // Metadata variable found, save it and remove it from the set.
            additionalData.emplace_back(data, length);
            i = metalist.erase(i);
        }
        if (metalist.empty())
        {
            // All metadata variables found, break out of journal loop.
            break;
        }
    }
    if (!metalist.empty())
    {
        // Not all the metadata variables were found in the journal.
        for (auto& metaVarStr : metalist)
        {
            logging::log<logging::level::INFO>("Failed to find metadata",
                    logging::entry("META_FIELD=%s", metaVarStr.c_str()));
        }
    }

    sd_journal_close(j);

    // Create error Entry dbus object
    entryId++;
    if (errLvl >= Entry::sevLowerLimit)
    {
        infoErrors.push_back(entryId);
    }
    else
    {
        realErrors.push_back(entryId);
    }
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
    auto objPath =  std::string(OBJ_ENTRY) + '/' +
            std::to_string(entryId);

    AssociationList objects {};
    processMetadata(errMsg, additionalData, objects);

    auto e = std::make_unique<Entry>(
                 busLog,
                 objPath,
                 entryId,
                 ms, // Milliseconds since 1970
                 errLvl,
                 std::move(errMsg),
                 std::move(additionalData),
                 std::move(objects),
                 fwVersion,
                 *this);
    serialize(*e);
    entries.insert(std::make_pair(entryId, std::move(e)));
}

void Manager::processMetadata(const std::string& errorName,
                              const std::vector<std::string>& additionalData,
                              AssociationList& objects) const
{
    // additionalData is a list of "metadata=value"
    constexpr auto separator = '=';
    for(const auto& entry: additionalData)
    {
        auto found = entry.find(separator);
        if(std::string::npos != found)
        {
            auto metadata = entry.substr(0, found);
            auto iter = meta.find(metadata);
            if(meta.end() != iter)
            {
                (iter->second)(metadata, additionalData, objects);
            }
        }
    }
}

void Manager::erase(uint32_t entryId)
{
    auto entry = entries.find(entryId);
    if(entries.end() != entry)
    {
        // Delete the persistent representation of this error.
        fs::path errorPath(ERRLOG_PERSIST_PATH);
        errorPath /= std::to_string(entryId);
        fs::remove(errorPath);

        auto removeId = [](std::list<uint32_t>& ids , uint32_t id)
        {
            auto it = std::find(ids.begin(), ids.end(), id);
            if (it != ids.end())
            {
                ids.erase(it);
            }
        };
        if (entry->second->severity() >= Entry::sevLowerLimit)
        {
            removeId(infoErrors, entryId);
        }
        else
        {
            removeId(realErrors, entryId);
        }
        entries.erase(entry);
    }
    else
    {
        logging::log<level::ERR>("Invalid entry ID to delete",
                logging::entry("ID=%d", entryId));
    }
}

void Manager::restore()
{
    auto sanity = [](const auto& id, const auto& restoredId)
    {
        return id == restoredId;
    };
    std::vector<uint32_t> errorIds;

    fs::path dir(ERRLOG_PERSIST_PATH);
    if (!fs::exists(dir) || fs::is_empty(dir))
    {
        return;
    }

    for(auto& file: fs::directory_iterator(dir))
    {
        auto id = file.path().filename().c_str();
        auto idNum = std::stol(id);
        auto e = std::make_unique<Entry>(
                     busLog,
                     std::string(OBJ_ENTRY) + '/' + id,
                     idNum,
                     *this);
        if (deserialize(file.path(), *e))
        {
            //validate the restored error entry id
            if (sanity(static_cast<uint32_t>(idNum), e->id()))
            {
                e->emit_object_added();
                if (e->severity() >= Entry::sevLowerLimit)
                {
                    infoErrors.push_back(idNum);
                }
                else
                {
                    realErrors.push_back(idNum);
                }

                entries.insert(std::make_pair(idNum, std::move(e)));
                errorIds.push_back(idNum);
            }
            else
            {
                logging::log<logging::level::ERR>(
                    "Failed in sanity check while restoring error entry. "
                    "Ignoring error entry",
                    logging::entry("ID_NUM=%d", idNum),
                    logging::entry("ENTRY_ID=%d", e->id()));
            }
        }
    }

    if (!errorIds.empty())
    {
        entryId = *(std::max_element(errorIds.begin(), errorIds.end()));
    }
}

void Manager::journalSync()
{
    bool syncRequested = false;
    auto fd = -1;
    auto rc = -1;
    auto wd = -1;
    auto bus = sdbusplus::bus::new_default();

    auto start =
        duration_cast<microseconds>(steady_clock::now().time_since_epoch())
            .count();

    constexpr auto maxRetry = 2;
    for (int i = 0; i < maxRetry; i++)
    {
        // Read timestamp from synced file
        constexpr auto syncedPath = "/run/systemd/journal/synced";
        std::ifstream syncedFile(syncedPath);
        if (syncedFile.fail())
        {
            log<level::ERR>("Failed to open journal synced file",
                            entry("FILENAME=%s", syncedPath),
                            entry("ERRNO=%d", errno));
            return;
        }

        // See if a sync happened by now
        std::string timestampStr;
        std::getline(syncedFile, timestampStr);
        auto timestamp = stoll(timestampStr);
        if (timestamp >= start)
        {
            return;
        }

        // Let's ask for a sync, but only once
        if (!syncRequested)
        {
            syncRequested = true;

            constexpr auto SYSTEMD_BUSNAME = "org.freedesktop.systemd1";
            constexpr auto SYSTEMD_PATH = "/org/freedesktop/systemd1";
            constexpr auto SYSTEMD_INTERFACE =
                "org.freedesktop.systemd1.Manager";
            constexpr auto JOURNAL_UNIT = "systemd-journald.service";
            auto signal = SIGRTMIN + 1;

            auto method = bus.new_method_call(SYSTEMD_BUSNAME, SYSTEMD_PATH,
                                              SYSTEMD_INTERFACE, "KillUnit");
            method.append(JOURNAL_UNIT, "main", signal);
            bus.call(method);
            if (method.is_method_error())
            {
                log<level::ERR>("Failed to kill journal service");
                return;
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
                log<level::ERR>("Failed to create inotify watch",
                                entry("ERRNO=%d", errno));
                return;
            }

            constexpr auto JOURNAL_RUN_PATH = "/run/systemd/journal";
            wd = inotify_add_watch(fd, JOURNAL_RUN_PATH,
                                   IN_MOVED_TO | IN_DONT_FOLLOW | IN_ONLYDIR);
            if (wd < 0)
            {
                log<level::ERR>("Failed to watch journal directory",
                                entry("PATH=%s", JOURNAL_RUN_PATH),
                                entry("ERRNO=%d", errno));
                close(fd);
                return;
            }
            continue;
        }

        // Let's wait until inotify reports an event
        struct pollfd fds = {
            .fd = fd,
            .events = POLLIN,
        };
        constexpr auto pollTimeout = 5; // 5 seconds
        rc = poll(&fds, 1, pollTimeout * 1000);
        if (rc < 0)
        {
            log<level::ERR>("Failed to add event", entry("ERRNO=%d", errno),
                            entry("ERR=%s", strerror(-rc)));
            inotify_rm_watch(fd, wd);
            close(fd);
            return;
        }
        else if (rc == 0)
        {
            log<level::INFO>("Poll timeout, no new journal synced data",
                             entry("TIMEOUT=%d", pollTimeout));
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

std::string Manager::readFWVersion()
{
    std::string version;
    std::ifstream versionFile{BMC_VERSION_FILE};
    std::string line;
    static constexpr auto VERSION_ID = "VERSION_ID=";

    while (std::getline(versionFile, line))
    {
        if (line.find(VERSION_ID) != std::string::npos)
        {
            auto pos = line.find_first_of('"') + 1;
            version = line.substr(pos, line.find_last_of('"') - pos);
            break;
        }
    }

    if (version.empty())
    {
        log<level::ERR>("Unable to read BMC firmware version");
    }

    return version;
}

} // namespace internal
} // namespace logging
} // namepsace phosphor
