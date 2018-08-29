#include "config.h"

#include <phosphor-logging/sdjournal.hpp>

namespace phosphor
{
namespace logging
{

SdJournalImpl sdjournal_impl;
SdJournalImpl* sdjournal_ptr = &sdjournal_impl;

SdJournalImpl* SwampJouralImpl(SdJournalImpl* with)
{
    SdJournalImpl* curr = sdjournal_ptr;
    sdjournal_ptr = with;
    return curr;
}

} // namespace logging
} // namespace phosphor
