#pragma once

#include <tuple>
#include <utility>
#include <phosphor-logging/log.hpp>

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

/**
 * @brief Used to map an sdbusplus error to a phosphor-logging error type
 *
 * Users log errors via the sdbusplus error name, and the execption that's
 * thrown is the corresponding sdbusplus exception. However, there's a need
 * to map the sdbusplus error name to the phosphor-logging error name, in order
 * to verify the error metadata at compile-time.
 */
template <typename T>
struct map_exception_type
{
    using type = T;
};

/**
 * @brief Typedef for above structure usage
 */
template <typename T> using map_exception_type_t =
    typename map_exception_type<T>::type;

} // namespace details

/** @fn commit()
 *  @brief Create an error log entry based on journal
 *          entry with a specified MSG_ID
 *  @param[in] - Exception name
 */
void commit(std::string&& name);

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
    static_assert(std::is_same<typename details::
                               map_exception_type_t<T>::metadata_types,
                               std::tuple<
                               details::deduce_entry_type_t<Args>...>>
                               ::value,
                  "You are not passing in required arguments for this error");

    log<details::map_exception_type_t<T>::L>(
        details::map_exception_type_t<T>::err_msg,
        details::deduce_entry_type<Args>{i_args}.get()...);

    // Now throw an exception for this error
    throw T();
}

} // namespace logging

} // namespace phosphor

