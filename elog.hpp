#pragma once

#include <tuple>
#include <utility>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/transaction.hpp>
#include "config.h"
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
private:
    // Append the transaction id to the err_code, separate them with a period.
    uint64_t id = sdbusplus::server::transaction::get_id();
    std::string exceptionStr = std::string(T::err_code) + '.' +
                                           std::to_string(id);
public:
    const char* what() const noexcept override { return exceptionStr.c_str(); }
};

/** @fn commit()
 *  @brief Create an error log entry based on journal
 *          entry with a specified transaction id.
 *  @tparam E - Error log struct
 *  @param[in] exceptionStr - String with the err_code and transaction id.
 */
template <typename E>
void commit(std::string exceptionStr)
{
    // Transaction id is located at the end of the string separated by a period.
    uint64_t id = 0;
    auto idPos = exceptionStr.rfind(".");
    if (idPos != std::string::npos)
    {
        // Remove the period and convert the string to integer.
        id = std::stoul(exceptionStr.substr(idPos+1), nullptr, 0);
    }

    // Create error log by calling the dbus Commit method, which takes the
    // transaction id and the err_code.
    auto b = sdbusplus::bus::new_default();
    auto m = b.new_method_call(BUSNAME_LOGGING,
                               OBJ_INTERNAL,
                               IFACE_INTERNAL,
                               "Commit");
    m.append(id, exceptionStr.substr(0, idPos));
    b.call_noreply(m);
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

