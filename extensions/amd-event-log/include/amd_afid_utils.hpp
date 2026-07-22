#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace phosphor::logging::extensions::ael
{

/**
 * @brief AMD Field ID metadata.
 */
struct AFIDInfo
{
    /** AMD Field Identifier. */
    uint64_t afid{};

    /** Associated FRU inventory paths. */
    std::vector<std::string> frus;
};

/**
 * @brief Lookup AFID metadata associated with an event.
 *
 * @param[in] message Event message identifier.
 * @param[in] addData Event additional data.
 *
 * @return AFID metadata if a matching entry is found.
 */
std::optional<AFIDInfo> lookupAFID(const std::string& message,
                                   const std::vector<std::string>& addData);

} // namespace phosphor::logging::extensions::ael
