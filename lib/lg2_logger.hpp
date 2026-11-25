#pragma once

#include <phosphor-logging/lg2/level.hpp>

#include <atomic>

namespace lg2::details
{

/** Most verbose level emitted to the journal; mutable via LogControl1. */
std::atomic<level>& log_threshold();

} // namespace lg2::details
