#pragma once

#include <string>
#include <tuple>
#include <vector>

namespace phosphor
{
namespace logging
{
namespace audit
{

// requestId operations
static constexpr uint32_t POWERON = 1;
static constexpr uint32_t POWEROFF = 2;
static constexpr uint32_t FACTORY_RESET = 3;

} // namespace audit
} // namespace logging
} // namespace phosphor
