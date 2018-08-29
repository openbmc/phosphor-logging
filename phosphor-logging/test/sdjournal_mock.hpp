#pragma once

#include <phosphor-logging/sdjournal.hpp>

#include "gmock/gmock.h"

namespace phosphor
{
namespace logging
{

class SdJournalMock : public SdJournalImpl
{
  public:
    virtual ~SdJournalMock() = default;

    MOCK_METHOD(journal_send_call, int());

    int journal_send(const char* fmt, ...) override
    {
        return journal_send_call();
    }
}

} // namespace logging
} // namespace phosphor
