#pragma once

#include <systemd/sd-journal.h>

#include <cstdarg>

namespace phosphor
{
namespace logging
{

/**
 * Implementation that calls into real sd_journal methods.
 */
class SdJournalImpl
{
  public:
    SdJournalImpl() = default;
    ~SdJournalImpl() = default;
    SdJournalImpl(const SdJournalImpl &) = default;
    SdJournalImpl &operator=(const SdJournalImpl &) = default;
    SdJournalImpl(SdJournalImpl &&) = default;
    SdJournalImpl &operator=(SdJournalImpl &&) = default;

    /**
     * Provide a fake method that's called by the real method so we can catch
     * the journal_send call in testing.
     */
    virtual int journal_send_call()
    {
        return 0;
    };

    /**
     * Send the information to sd_journal_send.
     *
     * @param[in] fmt - c string format.
     * @param[in] ... - parameters.
     * @return value from sd_journal_send
     */
    virtual int journal_send(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);

        int rc = ::sd_journal_send(fmt, args);
        va_end(args);

        return rc;
    }
};

extern SdJournalImpl* sdjournal_ptr;

/**
 * Swap out the sdjournal_ptr used by log<> such that a test
 * won't need to hit the real sd_journal and fail.
 *
 * @param[in] with - pointer to your sdjournal_mock object.
 * @return pointer to the previously stored sdjournal_ptr.
 */
SdJournalImpl* SwampJouralImpl(SdJournalImpl* with);

} // namespace logging
} // namespace phosphor
