#include <systemd/sd-journal.h>

#include <phosphor-logging/sdjournal.hpp>

#include <cstdarg>

namespace phosphor
{
namespace logging
{

int SdJournalHandler::journal_send_call(const char*)
{
    return 0;
}

int SdJournalHandler::journal_send(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int rc = ::sd_journal_send(fmt, args, NULL);
    va_end(args);

    return rc;
}

SdJournalHandler sdjournal_impl;
SdJournalHandler* sdjournal_ptr = &sdjournal_impl;

SdJournalHandler* SwapJouralHandler(SdJournalHandler* with)
{
    SdJournalHandler* curr = sdjournal_ptr;
    sdjournal_ptr = with;
    return curr;
}

} // namespace logging
} // namespace phosphor
