#pragma once

#include <sdbusplus/server.hpp>
#include "xyz/openbmc_project/Logging/Entry/server.hpp"

namespace phosphor
{
namespace logging
{
namespace details
{

template <typename T>
using ServerObject = typename sdbusplus::server::object::object<T>;

using EntryIface =
    sdbusplus::xyz::openbmc_project::Logging::server::Entry;

} // namespace details

/** @class Entry
 *  @brief OpenBMC logging entry implementation.
 *  @details A concrete implementation for the
 *  xyz.openbmc_project.Logging.Entry DBus API.
 */
class Entry final :
    public details::ServerObject<details::EntryIface>
{
    public:
        Entry() = delete;
        Entry(const Entry&) = delete;
        Entry& operator=(const Entry&) = delete;
        Entry(Entry&&) = default;
        Entry& operator=(Entry&&) = default;
        ~Entry() = default;

        /** @brief Constructor for the Log Entry object
         *  @param[in] bus - DBus bus to attach to.
         *  @param[in] busname - Name of DBus bus to own.
         *  @param[in] obj - Object path to attach to.
         */
        Entry(sdbusplus::bus::bus& bus,
                    const char* busname,
                    const char* obj);
};

} // namespace logging
} // namespace phosphor
