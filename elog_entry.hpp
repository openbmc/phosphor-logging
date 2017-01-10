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
         *  @param[in] bus - Bus to attach to.
         *  @param[in] path - Path to attach at.
         */
        Entry(sdbusplus::bus::bus& bus, const char* path,
              Properties properties) :
              details::ServerObject<details::EntryIface>(bus, path)
        {
            sdbusplus::xyz::openbmc_project::Logging::server::
                Entry::id(properties.id);
            sdbusplus::xyz::openbmc_project::Logging::server::
                Entry::severity(properties.severity);
            sdbusplus::xyz::openbmc_project::Logging::server::
                Entry::message(properties.msg);
            sdbusplus::xyz::openbmc_project::Logging::server::
                Entry::additionalData(properties.additionalData);
        };

};

} // namespace logging
} // namespace phosphor
