#pragma once

#include "xyz/openbmc_project/Logging/Entry/server.hpp"
#include "xyz/openbmc_project/Object/Delete/server.hpp"
#include "xyz/openbmc_project/Software/Version/server.hpp"

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/object.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/source/event.hpp>
#include <xyz/openbmc_project/Association/Definitions/server.hpp>
#include <xyz/openbmc_project/Common/FilePath/server.hpp>

namespace phosphor
{
namespace logging
{

using EntryIfaces = sdbusplus::server::object_t<
    sdbusplus::server::xyz::openbmc_project::logging::Entry,
    sdbusplus::server::xyz::openbmc_project::object::Delete,
    sdbusplus::server::xyz::openbmc_project::association::Definitions,
    sdbusplus::server::xyz::openbmc_project::software::Version,
    sdbusplus::server::xyz::openbmc_project::common::FilePath>;

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
 *  xyz.openbmc_project.Associations.Definitions DBus APIs.
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
     *  @param[in] objectPath - Path to attach at.
     *  @param[in] idErr - The error entry id.
     *  @param[in] timestampErr - The commit timestamp.
     *  @param[in] severityErr - The severity of the error.
     *  @param[in] msgErr - The message of the error.
     *  @param[in] additionalDataErr - The error metadata.
     *  @param[in] objects - The list of associations.
     *  @param[in] fwVersion - The BMC code version.
     *  @param[in] filePath - Serialization path
     *  @param[in] parent - The error's parent.
     */
    Entry(sdbusplus::bus_t& bus, const std::string& objectPath, uint32_t idErr,
          uint64_t timestampErr, Level severityErr, std::string&& msgErr,
          std::vector<std::string>&& additionalDataErr,
          AssociationList&& objects, const std::string& fwVersion,
          const std::string& filePath, internal::Manager& parent) :
        EntryIfaces(bus, objectPath.c_str(), EntryIfaces::action::defer_emit),
        parent(parent)
    {
        id(idErr, true);
        severity(severityErr, true);
        timestamp(timestampErr, true);
        updateTimestamp(timestampErr, true);
        message(std::move(msgErr), true);
        additionalData(std::move(additionalDataErr), true);
        associations(std::move(objects), true);
        // Store a copy of associations in case we need to recreate
        assocs = associations();
        sdbusplus::server::xyz::openbmc_project::logging::Entry::resolved(false,
                                                                          true);

        version(fwVersion, true);
        purpose(VersionPurpose::BMC, true);
        path(filePath, true);

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
    Entry(sdbusplus::bus_t& bus, const std::string& path, uint32_t entryId,
          internal::Manager& parent) :
        EntryIfaces(bus, path.c_str(), EntryIfaces::action::defer_emit),
        parent(parent)
    {
        id(entryId, true);
    };

    /** @brief Set resolution status of the error.
     *  @param[in] value - boolean indicating resolution
     *  status (true = resolved)
     *  @returns value of 'Resolved' property
     */
    bool resolved(bool value) override;

    using sdbusplus::server::xyz::openbmc_project::logging::Entry::resolved;

    /** @brief Update eventId string of the error.
     *  @param[in] value - The eventID
     *  @returns New property value
     */
    std::string eventId(std::string value) override;

    using sdbusplus::server::xyz::openbmc_project::logging::Entry::eventId;

    /** @brief Update resolution string of the error.
     *  @param[in] value - The resolution
     *  @returns New property value
     */
    std::string resolution(std::string value) override;

    using sdbusplus::server::xyz::openbmc_project::logging::Entry::resolution;

    /** @brief Delete this d-bus object.
     */
    void delete_() override;

    /** @brief Severity level to check in cap.
     *  @details Errors with severity lesser than this will be
     *           considered as low priority and maximum ERROR_INFO_CAP
     *           number errors of this category will be captured.
     */
    static constexpr auto sevLowerLimit = Entry::Level::Informational;

    /**
     * @brief Returns the file descriptor to the Entry file.
     * @return unix_fd - File descriptor to the Entry file.
     */
    sdbusplus::message::unix_fd getEntry() override;

  private:
    /** @brief This entry's associations */
    AssociationList assocs = {};

    /** @brief This entry's parent */
    internal::Manager& parent;

    /**
     * @brief The event source for closing the Entry file descriptor after it
     *        has been returned from the getEntry D-Bus method.
     */
    std::unique_ptr<sdeventplus::source::Defer> fdCloseEventSource;

    /**
     * @brief Closes the file descriptor passed in.
     * @details This is called from the event loop to close FDs returned from
     * getEntry().
     * @param[in] fd - The file descriptor to close
     * @param[in] source - The event source object used
     */
    void closeFD(int fd, sdeventplus::source::EventBase& source);
};

} // namespace logging
} // namespace phosphor
