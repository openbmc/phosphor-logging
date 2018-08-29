#pragma once

#include <phosphor-logging/sdjournal.hpp>

#include <gmock/gmock.h>

namespace phosphor
{
namespace logging
{

class SdJournalMock : public SdJournalImpl
{
  public:
    virtual ~SdJournalMock() = default;

    /* Set your mock to catch this call. */
    MOCK_METHOD1(journal_send_call, int(const char*));

    int journal_send(const char* fmt, ...) override
    {
        return journal_send_call(fmt);
    }
}

} // namespace logging
} // namespace phosphor
