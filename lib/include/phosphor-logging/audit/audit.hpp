/**
 * @brief phosphor audit API
 *
 * This file is part of the phosphor-logging audit project.
 *
 */

#pragma once

namespace phosphor {
namespace logging {
namespace audit {

uint32_t notify_event(std::uint64_t type, std::uint64_t requestId,
                      std::uint64_t rc, std::string user);

} // namespace audit
} // namespace logging
} // namespace phosphor
