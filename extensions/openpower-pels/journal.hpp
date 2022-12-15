#pragma once

#include <systemd/sd-journal.h>

#include <string>
#include <vector>

namespace openpower::pels
{

/**
 * @class JournalBase
 * Abstract class to read messages from the journal.
 */
class JournalBase
{
  public:
    JournalBase() = default;
    virtual ~JournalBase() = default;
    JournalBase(const JournalBase&) = default;
    JournalBase& operator=(const JournalBase&) = default;
    JournalBase(JournalBase&&) = default;
    JournalBase& operator=(JournalBase&&) = default;

    /**
     * @brief Get messages from the journal
     *
     * @param syslogID - The SYSLOG_IDENTIFIER field value
     * @param maxMessages - Max number of messages to get
     *
     * @return The messages
     */
    virtual std::vector<std::string> getMessages(const std::string& syslogID,
                                                 size_t maxMessages) const = 0;

    /**
     * @brief Call journalctl --sync to write unwritten journal data to disk
     */
    virtual void sync() const = 0;
};

/**
 * @class Journal
 *
 * Reads from the journal.
 */
class Journal : public JournalBase
{
  public:
    Journal() = default;
    ~Journal() = default;
    Journal(const Journal&) = default;
    Journal& operator=(const Journal&) = default;
    Journal(Journal&&) = default;
    Journal& operator=(Journal&&) = default;

    /**
     * @brief Get messages from the journal
     *
     * @param syslogID - The SYSLOG_IDENTIFIER field value
     * @param maxMessages - Max number of messages to get
     *
     * @return The messages
     */
    std::vector<std::string> getMessages(const std::string& syslogID,
                                         size_t maxMessages) const override;

    /**
     * @brief Call journalctl --sync to write unwritten journal data to disk
     */
    void sync() const override;

  private:
    /**
     * @brief Gets a field from the current journal entry
     *
     * @param journal - pointer to current journal entry
     * @param field - The field name whose value to get
     *
     * @return std::string - The field value
     */
    std::string getFieldValue(sd_journal* journal,
                              const std::string& field) const;

    /**
     * @brief Gets a readable timestamp from the journal entry
     *
     * @param journal - pointer to current journal entry
     *
     * @return std::string - A timestamp string
     */
    std::string getTimeStamp(sd_journal* journal) const;
};
} // namespace openpower::pels
