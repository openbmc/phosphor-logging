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

class Entry
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
        Entry() = delete;
        Entry(const Entry&) = delete;
        Entry& operator=(const Entry&) = delete;
        Entry(Entry&&) = default;
        Entry& operator=(Entry&&) = default;
        virtual ~Entry() = default;

        /** @brief Constructor to put object onto bus at a dbus path.
         *  @param[in] bus - Bus to attach to.
         *  @param[in] path - Path to attach at.
         */
        Entry(bus::bus& bus, const char* path);

        enum class Level
        {
            Emergency,
            Alert,
            Critical,
            Error,
            Warning,
            Notice,
            Informational,
            Debug,
        };



        /** Get value of Id */
        virtual uint32_t id() const;
        /** Set value of Id */
        virtual uint32_t id(uint32_t value);
        /** Get value of Severity */
        virtual Level severity() const;
        /** Set value of Severity */
        virtual Level severity(Level value);
        /** Get value of Message */
        virtual std::string message() const;
        /** Set value of Message */
        virtual std::string message(std::string value);
        /** Get value of AdditionalData */
        virtual std::vector<std::string> additionalData() const;
        /** Set value of AdditionalData */
        virtual std::vector<std::string> additionalData(std::vector<std::string> value);

    /** @brief Convert a string to an appropriate enum value.
     *  @param[in] s - The string to convert in the form of
     *                 "xyz.openbmc_project.Logging.Entry.<value name>"
     *  @return - The enum value.
     */
    static Level convertLevelFromString(std::string& s);

    private:

        /** @brief sd-bus callback for get-property 'Id' */
        static int _callback_get_Id(
            sd_bus*, const char*, const char*, const char*,
            sd_bus_message*, void*, sd_bus_error*);
        /** @brief sd-bus callback for set-property 'Id' */
        static int _callback_set_Id(
            sd_bus*, const char*, const char*, const char*,
            sd_bus_message*, void*, sd_bus_error*);

        /** @brief sd-bus callback for get-property 'Severity' */
        static int _callback_get_Severity(
            sd_bus*, const char*, const char*, const char*,
            sd_bus_message*, void*, sd_bus_error*);
        /** @brief sd-bus callback for set-property 'Severity' */
        static int _callback_set_Severity(
            sd_bus*, const char*, const char*, const char*,
            sd_bus_message*, void*, sd_bus_error*);

        /** @brief sd-bus callback for get-property 'Message' */
        static int _callback_get_Message(
            sd_bus*, const char*, const char*, const char*,
            sd_bus_message*, void*, sd_bus_error*);
        /** @brief sd-bus callback for set-property 'Message' */
        static int _callback_set_Message(
            sd_bus*, const char*, const char*, const char*,
            sd_bus_message*, void*, sd_bus_error*);

        /** @brief sd-bus callback for get-property 'AdditionalData' */
        static int _callback_get_AdditionalData(
            sd_bus*, const char*, const char*, const char*,
            sd_bus_message*, void*, sd_bus_error*);
        /** @brief sd-bus callback for set-property 'AdditionalData' */
        static int _callback_set_AdditionalData(
            sd_bus*, const char*, const char*, const char*,
            sd_bus_message*, void*, sd_bus_error*);


        static constexpr auto _interface = "xyz.openbmc_project.Logging.Entry";
        static const vtable::vtable_t _vtable[];
        sdbusplus::server::interface::interface
                _xyz_openbmc_project_Logging_Entry_interface;

        uint32_t _id{};
        Level _severity{};
        std::string _message{};
        std::vector<std::string> _additionalData{};

};

/* Specialization of sdbusplus::server::bindings::details::convertForMessage
 * for enum-type Entry::Level.
 *
 * This converts from the enum to a constant c-string representing the enum.
 *
 * @param[in] e - Enum value to convert.
 * @return C-string representing the name for the enum value.
 */
std::string convertForMessage(Entry::Level e);

} // namespace server
} // namespace Logging
} // namespace openbmc_project
} // namespace xyz
} // namespace sdbusplus

