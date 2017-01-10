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
        struct Properties
        {
            uint32_t id;
            Level severity;
            std::string msg;
            std::vector<std::string> additionalData;
        };

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
        Entry(sdbusplus::bus::bus& bus, const std::string& path,
            const Properties& properties) :
            details::ServerObject<details::EntryIface>(bus, path.c_str(), true)
        {
            this->id(std::move(properties.id));
            this->severity(std::move(properties.severity));
            this->message(std::move(properties.msg));
            this->additionalData(std::move(properties.additionalData));

            // Emit deferred signal.
            this->emit_object_added();
        };

};

} // namespace logging
} // namespace phosphor
