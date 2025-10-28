// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2023 IBM Corporation

#include "journal.hpp"

#include "util.hpp"

#include <phosphor-logging/lg2.hpp>

namespace openpower::pels
{

using namespace phosphor::logging;

/**
 * @class JournalCloser
 *
 * Closes the journal on destruction
 */
class JournalCloser
{
  public:
    JournalCloser() = delete;
    JournalCloser(const JournalCloser&) = delete;
    JournalCloser(JournalCloser&&) = delete;
    JournalCloser& operator=(const JournalCloser&) = delete;
    JournalCloser& operator=(JournalCloser&&) = delete;

    explicit JournalCloser(sd_journal* journal) : journal{journal} {}

    ~JournalCloser()
    {
        sd_journal_close(journal);
    }

  private:
    sd_journal* journal{nullptr};
};

void Journal::sync() const
{
    auto start = std::chrono::steady_clock::now();

    util::journalSync();

    auto end = std::chrono::steady_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (duration.count() > 100)
    {
        lg2::info("Journal sync took {DURATION}ms", "DURATION",
                  duration.count());
    }
}

std::vector<std::string> Journal::getMessages(const std::string& syslogID,
                                              size_t maxMessages) const
{
    // The message registry JSON schema will also fail if a zero is in the JSON
    if (0 == maxMessages)
    {
        lg2::error(
            "maxMessages value of zero passed into Journal::getMessages");
        return std::vector<std::string>{};
    }

    sd_journal* journal;
    int rc = sd_journal_open(&journal, SD_JOURNAL_LOCAL_ONLY);
    if (rc < 0)
    {
        throw std::runtime_error{
            std::string{"Failed to open journal: "} + strerror(-rc)};
    }

    JournalCloser closer{journal};

    if (!syslogID.empty())
    {
        std::string match{"SYSLOG_IDENTIFIER=" + syslogID};

        rc = sd_journal_add_match(journal, match.c_str(), 0);
        if (rc < 0)
        {
            throw std::runtime_error{
                std::string{"Failed to add journal match: "} + strerror(-rc)};
        }
    }

    // Loop through matching entries from newest to oldest
    std::vector<std::string> messages;
    messages.reserve(maxMessages);
    std::string sID, pid, message, timeStamp, line;

    SD_JOURNAL_FOREACH_BACKWARDS(journal)
    {
        timeStamp = getTimeStamp(journal);
        sID = getFieldValue(journal, "SYSLOG_IDENTIFIER");
        pid = getFieldValue(journal, "_PID");
        message = getFieldValue(journal, "MESSAGE");

        line = timeStamp + " " + sID + "[" + pid + "]: " + message;
        messages.emplace(messages.begin(), line);

        if (messages.size() >= maxMessages)
        {
            break;
        }
    }

    return messages;
}

std::string Journal::getFieldValue(sd_journal* journal,
                                   const std::string& field) const
{
    std::string value{};

    const void* data{nullptr};
    size_t length{0};
    int rc = sd_journal_get_data(journal, field.c_str(), &data, &length);
    if (rc < 0)
    {
        if (-rc == ENOENT)
        {
            // Current entry does not include this field; return empty value
            return value;
        }
        else
        {
            throw std::runtime_error{
                std::string{"Failed to read journal entry field: "} +
                strerror(-rc)};
        }
    }

    // Get value from field data.  Field data in format "FIELD=value".
    std::string dataString{static_cast<const char*>(data), length};
    std::string::size_type pos = dataString.find('=');
    if ((pos != std::string::npos) && ((pos + 1) < dataString.size()))
    {
        // Value is substring after the '='
        value = dataString.substr(pos + 1);
    }

    return value;
}

std::string Journal::getTimeStamp(sd_journal* journal) const
{
    // Get realtime (wallclock) timestamp of current journal entry.  The
    // timestamp is in microseconds since the epoch.
    uint64_t usec{0};
    int rc = sd_journal_get_realtime_usec(journal, &usec);
    if (rc < 0)
    {
        throw std::runtime_error{
            std::string{"Failed to get journal entry timestamp: "} +
            strerror(-rc)};
    }

    // Convert to number of seconds since the epoch
    time_t secs = usec / 1000000;

    // Convert seconds to tm struct required by strftime()
    struct tm* timeStruct = gmtime(&secs);
    if (timeStruct == nullptr)
    {
        throw std::runtime_error{
            std::string{"Invalid journal entry timestamp: "} + strerror(errno)};
    }

    // Convert tm struct into a date/time string
    char timeStamp[80];
    strftime(timeStamp, sizeof(timeStamp), "%b %d %H:%M:%S", timeStruct);

    return timeStamp;
}

} // namespace openpower::pels
