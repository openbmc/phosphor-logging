#pragma once

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/object.hpp>
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
class Entry : public details::ServerObject<details::EntryIface>
{
    public:
        Entry() = delete;
        Entry(const Entry&) = delete;
        Entry& operator=(const Entry&) = delete;
        Entry(Entry&&) = delete;
        Entry& operator=(Entry&&) = delete;
        virtual ~Entry() = default;

        /** @brief Constructor to put object onto bus at a dbus path.
         *         Defer signal registration (pass true for deferSignal to the
         *         base class) until after the properties are set.
         *  @param[in] bus - Bus to attach to.
         *  @param[in] path - Path to attach at.
         *  @param[in] properties - Desired Entry properties.
         */
        Entry(sdbusplus::bus::bus& bus,
              const std::string& path,
              uint32_t idErr,
              Level severityErr,
              std::string msgErr,
              std::vector<std::string>&& additionalDataErr) :
              details::ServerObject<details::EntryIface>
                    (bus, path.c_str(), true)
        {
            id(idErr);
            severity(severityErr);
            message(std::move(msgErr));
            additionalData(std::move(additionalDataErr));

            // Emit deferred signal.
            this->emit_object_added();
        };

};

} // namespace logging
} // namespace phosphor
