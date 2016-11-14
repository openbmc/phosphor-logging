#pragma once
#include <tuple>
#include <systemd/sd-bus.h>
#include <sdbusplus/server.hpp>

namespace sdbusplus
{
namespace xyz
{
namespace openbmc_project
{
namespace Logging
{
namespace server
{

class Manager
{
    public:
        /* Define all of the basic class operations:
         *     Not allowed:
         *         - Default constructor to avoid nullptrs.
         *         - Copy operations due to internal unique_ptr.
         *     Allowed:
         *         - Move operations.
         *         - Destructor.
         */
        Manager() = delete;
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;
        Manager(Manager&&) = default;
        Manager& operator=(Manager&&) = default;
        virtual ~Manager() = default;

        /** @brief Constructor to put object onto bus at a dbus path.
         *  @param[in] bus - Bus to attach to.
         *  @param[in] path - Path to attach at.
         */
        Manager(bus::bus& bus, const char* path);



        /** @brief Implementation for Commit
         *  Write the requested error/event entry with its associated metadata fields to flash.
         *
         *  @param[in] transactionId - The unique identifier of the journal entry(ies) to be committed.
         *  @param[in] errMsg - The error exception message associated with the error event log to be committed.
         */
        virtual void commit(
            uint64_t transactionId,
            std::string errMsg) = 0;




    private:

        /** @brief sd-bus callback for Commit
         */
        static int _callback_Commit(
            sd_bus_message*, void*, sd_bus_error*);


        static constexpr auto _interface = "xyz.openbmc_project.Logging.Manager";
        static const vtable::vtable_t _vtable[];
        sdbusplus::server::interface::interface
                _xyz_openbmc_project_Logging_Manager_interface;


};


} // namespace server
} // namespace Logging
} // namespace openbmc_project
} // namespace xyz
} // namespace sdbusplus

