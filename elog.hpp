#pragma once

#include <tuple>
#include <utility>
#include <systemd/sd-bus.h>
#include <mapper.h>
#include "elog-gen.hpp"

namespace phosphor
{

namespace logging
{

/**
 * @brief Structure used by callers to indicate they want to use the last value
 *        put in the journal for input parameter.
*/
template <typename T>
struct prev_entry
{
    using type = T;
};



namespace details
{
/**
 * @brief Used to return the generated tuple for the error code meta data
 *
 * The prev_entry (above) and deduce_entry_type structures below are used
 * to verify at compile time the required parameters have been passed to
 * the elog interface and then to forward on the appropriate tuple to the
 * log interface.
 */
template <typename T>
struct deduce_entry_type
{

    using type = T;
    auto get() { return value._entry; }

    T value;
};

/**
 * @brief Used to return an empty tuple for prev_entry parameters
 *
 * This is done so we can still call the log() interface with the variable
 * arg parameters to elog.  The log() interface will simply ignore the empty
 * tuples which is what we want for prev_entry parameters.
 */
template <typename T>
struct deduce_entry_type<prev_entry<T>>
{
    using type = T;
    auto get() { return std::make_tuple(); }

    prev_entry<T> value;
};

/**
 * @brief Typedef for above structure usage
 */
template <typename T> using deduce_entry_type_t =
        typename deduce_entry_type<T>::type;

} // namespace details

/**
 * @brief Error log exception base class
 *
 * This allows people to capture all error log exceptions if desired
 */
class elogExceptionBase : public std::exception {};

/**
 * @brief Error log exception class
 *
 * This is for capturing specific error log exceptions
 */
template <typename T> class elogException : public elogExceptionBase
{
public:
    const char* what() const noexcept override { return T::err_code; }
};

/** @fn commit()
 *  @brief Create an error log entry based on journal
 *          entry with a specified MSG_ID
 *  @tparam E - Error log struct
 */
template <typename E>
void commit()
{
    constexpr const auto objName = "/xyz/openbmc_project/Logging";
    constexpr const auto iface = "xyz.openbmc_project.Logging";
    sd_bus_message *reply = nullptr;
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus *bus = nullptr;
    char *busName = nullptr;
    int rc = 0;

    // Open up a connection to the bus
    rc = sd_bus_open_system(&bus);
    if (rc < 0)
    {
        fprintf(stderr, "Failed to connect to system bus: %s\n",
                strerror(-rc));
        goto finish;
    }

    // Get bus name for the Logging object
    rc = mapper_get_service(bus, objName, &busName);
    if (rc < 0)
    {
        fprintf(stderr, "Failed to get bus name, return value: %s.\n",
                strerror(-rc));
        goto finish;
    }

    // Call the Commit method now
    rc = sd_bus_call_method(bus,busName,objName,iface,
                           "Commit",&error, &reply, "s",
                            E::err_msg);
    if (rc < 0)
    {
        fprintf(stderr, "Failed to call Get Method: %s\n", strerror(-rc));
        goto finish;
    }
    // dbus_commit(E.msgid); // call server

    finish:
    return;
}

/** @fn elog()
 *  @brief Create a journal log entry based on predefined
 *          error log information
 *  @tparam T - Error log type
 *  @param[in] i_args - Metadata fields to be added to the journal entry
 */
template <typename T, typename ...Args>
void elog(Args... i_args)
{
    // Validate the caller passed in the required parameters
    static_assert(std::is_same<typename T::metadata_types,
                               std::tuple<
                               details::deduce_entry_type_t<Args>...>>
                               ::value,
                  "You are not passing in required arguments for this error");

    log<T::L>(T::err_msg,
              details::deduce_entry_type<Args>{i_args}.get()...);

    // Now throw an exception for this error
    throw elogException<T>();
}

} // namespace logging

} // namespace phosphor

