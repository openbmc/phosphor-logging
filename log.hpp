#pragma once

#include <tuple>
#include <systemd/sd-journal.h>

namespace phosphor
{

namespace logging
{

/** @enum level
 *  @brief Enum for priority level
 */
enum class level
{
    EMERG   = LOG_EMERG,
    ALERT   = LOG_ALERT,
    CRIT    = LOG_CRIT,
    ERR     = LOG_ERR,
    WARNING = LOG_WARNING,
    NOTICE  = LOG_NOTICE,
    INFO    = LOG_INFO,
    DEBUG   = LOG_DEBUG,
};

/** @fn log()
 *  @brief Log message to systemd journal
 *  @tparam L - Priority level
 *  @param[in] msg - Message to be logged
 *  @param[in] entry - Metadata fields to be added to the message
 */
template <level L, typename Msg, typename ...Entry>
void log(Msg msg, Entry... entry);

/** @fn msg()
 *  @brief Prepend MESSAGE= to the input msg format string instead of
 *    requiring the user to remember to prepend themselves.
 *    This is required by sd_journal_send().
 *  @tparam Arg - Type of arguments
 *  @param[in] arg - Message string
 */
template <typename Arg>
auto msg(Arg&& arg);

/** @fn entry()
 *  @brief Pack each format string entry as a tuple to be able to validate
 *    the string and parameters when multiple entries are passed to be logged.
 *  @tparam Arg - Types of first argument
 *  @tparam Args - Types of remaining arguments
 *  @param[in] arg - First metadata string of form VAR=value where
 *    VAR is the variable name in uppercase and
 *    value is of any size and format
 *  @param[in] args - Remaining metadata strings
 */
template <typename Arg, typename ...Args>
auto entry(Arg&& arg, Args&&... args);

namespace details
{

/** @fn prio()
 *  @brief Prepend PRIORITY= to the input priority string.
 *    This is required by sd_journal_send().
 *  @tparam L - Priority level
 */
template <level L>
auto prio()
{
    auto prio_str = "PRIORITY=%d";
    auto prio_tuple = std::make_tuple(prio_str, (int)L);
    return prio_tuple;
}

/** @fn send_log()
 *  @brief Log message to systemd journal
 *  @tparam T - Type of tuple
 *  @tparam I - std::integer_sequence of indexes (0..N) for each tuple element
 *  @param[in] e - Tuple containing the data to be logged
 *  @param[unnamed] - std::integer_sequence of tuple's index values
 */
template <typename T, size_t ...I>
void send_log(T&& e, std::integer_sequence<size_t, I...>)
{
    sd_journal_send(std::get<I>(std::forward<T>(e))..., NULL);
}

/** @fn send_entry_log()
 *  @brief Log entry message to systemd journal
 *  @tparam T - Type of tuple
 *  @param[in] e - Tuple containing the data to be logged
 */
template <typename T>
void send_entry_log(T&& e)
{
    constexpr auto e_size = std::tuple_size<typename std::decay<T>::type>::value;
    send_log(std::forward<T>(e), std::make_index_sequence<e_size>{});
}

} // namespace details

template <level L, typename Msg, typename ...Entry>
void log(Msg msg, Entry... entry)
{
    auto log_tuple = std::tuple_cat(details::prio<L>(),
                                    std::forward<Msg>(msg),
                                    std::forward<Entry>(entry)...);
    details::send_entry_log(log_tuple);
}

template <typename Arg>
auto msg(Arg&& arg)
{
    auto msg_str = "MESSAGE=%s";
    auto msg_tuple = std::make_tuple(msg_str, std::forward<Arg>(arg));
    return msg_tuple;
}

template <typename Arg, typename ...Args>
auto entry(Arg&& arg, Args&&... args)
{
    auto entry_tuple = std::make_tuple(std::forward<Arg>(arg),
                                       std::forward<Args>(args)...);
    return entry_tuple;
}

} // namespace logging

} // namespace phosphor

