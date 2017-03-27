#pragma once

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/object.hpp>
#include "xyz/openbmc_project/Logging/Entry/server.hpp"
#include "org/openbmc/Associations/server.hpp"

namespace phosphor
{
namespace logging
{

using EntryIfaces = sdbusplus::server::object::object<
    sdbusplus::xyz::openbmc_project::Logging::server::Entry,
    sdbusplus::org::openbmc::server::Associations>;

using AssociationList =
     std::vector<std::tuple<std::string, std::string, std::string>>;

/** @class Entry
 *  @brief OpenBMC logging entry implementation.
 *  @details A concrete implementation for the
 *  xyz.openbmc_project.Logging.Entry and
 *  org.openbmc.Associations DBus APIs.
 */
class Entry : public EntryIfaces
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
         *  @param[in] idErr - The error entry id.
         *  @param[in] timestampErr - The commit timestamp.
         *  @param[in] severityErr - The severity of the error.
         *  @param[in] msgErr - The message of the error.
         *  @param[in] additionalDataErr - The error metadata.
         */
        Entry(sdbusplus::bus::bus& bus,
              const std::string& path,
              uint32_t idErr,
              uint64_t timestampErr,
              Level severityErr,
              std::string&& msgErr,
              std::vector<std::string>&& additionalDataErr,
              AssociationList&& objects) :
              EntryIfaces(bus, path.c_str(), true)
        {
            id(idErr);
            severity(severityErr);
            timestamp(timestampErr);
            message(std::move(msgErr));
            additionalData(std::move(additionalDataErr));
            associations(std::move(objects));
            resolved(false);

            // Emit deferred signal.
            this->emit_object_added();
        };

        /** @brief Set resolution status of the error.
         *  @param[in] value - boolean indicating resolution
         *  status (true = resolved)
         *  @returns value of 'Resolved' property
         */
        virtual bool resolved(bool value)
        {
            if(value)
            {
                associations({});
            }
            return sdbusplus::xyz::openbmc_project::
                   Logging::server::Entry::resolved(value);
        }

};

} // namespace logging
} // namespace phosphor
