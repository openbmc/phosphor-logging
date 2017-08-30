#pragma once

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/object.hpp>
#include "xyz/openbmc_project/Logging/Entry/server.hpp"
#include "xyz/openbmc_project/Object/Delete/server.hpp"
#include "org/openbmc/Associations/server.hpp"

namespace phosphor
{
namespace logging
{

using EntryIfaces = sdbusplus::server::object::object<
    sdbusplus::xyz::openbmc_project::Logging::server::Entry,
    sdbusplus::xyz::openbmc_project::Object::server::Delete,
    sdbusplus::org::openbmc::server::Associations>;

using AssociationList =
     std::vector<std::tuple<std::string, std::string, std::string>>;

namespace internal
{
class Manager;
}

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
         *  @param[in] parent - The error's parent.
         */
        Entry(sdbusplus::bus::bus& bus,
              const std::string& path,
              uint32_t idErr,
              uint64_t timestampErr,
              Level severityErr,
              std::string&& msgErr,
              std::vector<std::string>&& additionalDataErr,
              AssociationList&& objects,
              internal::Manager& parent) :
              EntryIfaces(bus, path.c_str(), true),
              parent(parent)
        {
            id(idErr);
            severity(severityErr);
            timestamp(timestampErr);
            message(std::move(msgErr));
            additionalData(std::move(additionalDataErr));
            associations(std::move(objects));
            // Store a copy of associations in case we need to recreate
            assocs = associations();
            sdbusplus::xyz::openbmc_project::
                Logging::server::Entry::resolved(false);

            // Emit deferred signal.
            this->emit_object_added();
        };

        /** @brief Constructor that puts an "empty" error object on the bus,
         *         with only the id property populated. Rest of the properties
         *         to be set by the caller. Caller should emit the added signal.
         *  @param[in] bus - Bus to attach to.
         *  @param[in] path - Path to attach at.
         *  @param[in] id - The error entry id.
         *  @param[in] parent - The error's parent.
         */
        Entry(sdbusplus::bus::bus& bus,
              const std::string& path,
              uint32_t entryId,
              internal::Manager& parent) :
              EntryIfaces(bus, path.c_str(), true),
              parent(parent)
        {
            id(entryId);
        };

        /** @brief Set resolution status of the error.
         *  @param[in] value - boolean indicating resolution
         *  status (true = resolved)
         *  @returns value of 'Resolved' property
         */
        bool resolved(bool value) override;

        using sdbusplus::xyz::openbmc_project::
              Logging::server::Entry::resolved;

        /** @brief Delete this d-bus object.
         */
        void delete_() override;

    private:
        /** @brief This entry's associations */
        AssociationList assocs = {};

        /** @brief This entry's parent */
        internal::Manager& parent;
};

} // namespace logging
} // namespace phosphor
