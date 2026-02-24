#pragma once
#include "xyz/openbmc_project/Logging/Entry/server.hpp"

#include <phosphor-logging/log.hpp>
#include <sdbusplus/exception.hpp>

#include <tuple>
#include <utility>

namespace phosphor
{

namespace logging
{

using namespace sdbusplus::server::xyz::openbmc_project::logging;

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
    auto get()
    {
        return value._entry;
    }

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
    auto get()
    {
        return std::make_tuple();
    }

    prev_entry<T> value;
};

/**
 * @brief Typedef for above structure usage
 */
template <typename T>
using deduce_entry_type_t = typename deduce_entry_type<T>::type;

/**
 * @brief Used to map an sdbusplus error to a phosphor-logging error type
 *
 * Users log errors via the sdbusplus error name, and the exception that's
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
template <typename T>
using map_exception_type_t = typename map_exception_type<T>::type;

/** @fn commit()
 *  @brief Create an error log entry based on journal
 *          entry with a specified exception name
 *  @param[in] name - name of the error exception
 *
 *  @return The entry ID
 */
uint32_t commit(const char* name);

/** @fn commit() - override that accepts error level
 *
 *  @return The entry ID
 */
uint32_t commit(const char* name, Entry::Level level);

} // namespace details

/** @fn commit()
 *  \deprecated use commit<T>()
 *  @brief Create an error log entry based on journal
 *          entry with a specified MSG_ID
 *  @param[in] name - name of the error exception
 *
 *  @return The entry ID
 */
uint32_t commit(std::string&& name);

/** @fn commit()
 *  @brief Create an error log entry based on journal
 *          entry with a specified MSG_ID
 *
 *  @return The entry ID
 */
template <typename T>
uint32_t commit()
{
    // Validate if the exception is derived from sdbusplus::exception.
    static_assert(std::is_base_of_v<sdbusplus::exception_t, T>,
                  "T must be a descendant of sdbusplus::exception_t");
    static_assert(
        !std::is_base_of_v<sdbusplus::exception::generated_event<T>, T>,
        "T must NOT be an sdbusplus::generated_event");

    return details::commit(T::errName);
}

/** @fn commit()
 *  @brief Create an error log entry based on journal
 *         entry with a specified MSG_ID. This override accepts error level.
 *  @param[in] level - level of the error
 *
 *  @return The entry ID
 */
template <typename T>
uint32_t commit(Entry::Level level)
{
    // Validate if the exception is derived from sdbusplus::exception.
    static_assert(std::is_base_of_v<sdbusplus::exception_t, T>,
                  "T must be a descendant of sdbusplus::exception_t");
    static_assert(
        !std::is_base_of_v<sdbusplus::exception::generated_event<T>, T>,
        "T must NOT be an sdbusplus::generated_event");

    return details::commit(T::errName, level);
}

/** @fn elog()
 *  @brief Create a journal log entry based on predefined
 *          error log information
 *  @tparam T - Error log type
 *  @param[in] i_args - Metadata fields to be added to the journal entry
 */
template <typename T, typename... Args>
[[noreturn]] void elog(Args... i_args)
{
    // Validate if the exception is derived from sdbusplus::exception.
    static_assert(std::is_base_of_v<sdbusplus::exception_t, T>,
                  "T must be a descendant of sdbusplus::exception_t");
    static_assert(
        !std::is_base_of_v<sdbusplus::exception::generated_event<T>, T>,
        "T must NOT be an sdbusplus::generated_event");

    // Validate the caller passed in the required parameters
    static_assert(std::is_same_v<
                      typename details::map_exception_type_t<T>::metadata_types,
                      std::tuple<details::deduce_entry_type_t<Args>...>>,
                  "You are not passing in required arguments for this error");

    log<details::map_exception_type_t<T>::L>(
        T::errDesc, details::deduce_entry_type<Args>{i_args}.get()...);

    // Now throw an exception for this error
    throw T();
}

/** @fn report()
 *  @brief Create a journal log entry based on predefined
 *         error log information and commit the error
 *  @tparam T - exception
 *  @param[in] i_args - Metadata fields to be added to the journal entry
 *
 *  @return The entry ID
 */
template <typename T, typename... Args>
uint32_t report(Args... i_args)
{
    // validate if the exception is derived from sdbusplus::exception.
    static_assert(std::is_base_of_v<sdbusplus::exception_t, T>,
                  "T must be a descendant of sdbusplus::exception_t");
    static_assert(
        !std::is_base_of_v<sdbusplus::exception::generated_event<T>, T>,
        "T must NOT be an sdbusplus::generated_event");

    // Validate the caller passed in the required parameters
    static_assert(std::is_same_v<
                      typename details::map_exception_type_t<T>::metadata_types,
                      std::tuple<details::deduce_entry_type_t<Args>...>>,
                  "You are not passing in required arguments for this error");

    log<details::map_exception_type_t<T>::L>(
        T::errDesc, details::deduce_entry_type<Args>{i_args}.get()...);

    return commit<T>();
}

/** @fn report()
 *  @brief Create a journal log entry based on predefined
 *         error log information and commit the error. Accepts error
 *         level.
 *  @tparam T - exception
 *  @param[in] level - level of the error
 *  @param[in] i_args - Metadata fields to be added to the journal entry
 *
 *  @return The entry ID
 */
template <typename T, typename... Args>
uint32_t report(Entry::Level level, Args... i_args)
{
    // validate if the exception is derived from sdbusplus::exception.
    static_assert(std::is_base_of_v<sdbusplus::exception_t, T>,
                  "T must be a descendant of sdbusplus::exception_t");
    static_assert(
        !std::is_base_of_v<sdbusplus::exception::generated_event<T>, T>,
        "T must NOT be an sdbusplus::generated_event");

    // Validate the caller passed in the required parameters
    static_assert(std::is_same_v<
                      typename details::map_exception_type_t<T>::metadata_types,
                      std::tuple<details::deduce_entry_type_t<Args>...>>,
                  "You are not passing in required arguments for this error");

    log<details::map_exception_type_t<T>::L>(
        T::errDesc, details::deduce_entry_type<Args>{i_args}.get()...);

    return commit<T>(level);
}

} // namespace logging

} // namespace phosphor
