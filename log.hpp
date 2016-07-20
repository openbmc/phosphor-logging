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

/** @fn entry()
 *  @brief Create tuple from args
 *  @tparam Arg - Types of first argument
 *  @tparam Args - Types of remaining arguments
 *  @param[in] arg - First metadata string of form VAR=value where
 *    VAR is the variable name in uppercase and
 *    value is of any size and format
 *  @param[in] args - Remaining metadata strings
 */
template <typename Arg, typename ...Args>
std::tuple<Arg, Args...> entry(Arg&& arg, Args&&... args);

namespace details
{

/** @fn prio()
 *  @brief Create tuple from priority string and args
 *  @tparam Arg - Type of arguments
 *  @param[in] arg - Priority level
 */
template <typename Arg>
std::tuple<const char*, Arg> prio(Arg&& arg);

/** @fn send_log()
 *  @brief Log message to systemd journal
 *  @tparam T - Types of tuples
 *  @tparam I - std::integer_sequence of indexes (0..N) for each tuple element
 *  @param[in] e - Tuple containing the data to be logged
 *  @param[unnamed] - std::integer_sequence of tuple's index values
 */
template <typename ...T, size_t ...I>
void send_log(std::tuple<T...>&& e, std::integer_sequence<size_t, I...>);

/** @fn send_tuple_log()
 *  @brief Log tuple message to systemd journal
 *  @tparam T - Types of tuples
 *  @tparam I - std::integer_sequence of indexes (0..N) for each tuple element
 *  @param[in] entry - Tuple containing the data to be logged
 */
template <typename ...T, typename I = std::make_index_sequence<sizeof...(T)>>
void send_tuple_log(std::tuple<T...>&& entry);

/** @fn send_entry_log()
 *  @brief Log entry message to systemd journal
 *  @tparam Entry - Type of first argument
 *  @tparam Rest - Type of remaining arguments
 *  @param[in] e - First argument containing the data to be logged
 *  @param[in] r - Remaining arguments containing the data to be logged
 */
template <typename Entry, typename ...Rest>
void send_entry_log(Entry&& e, Rest&&... r);

template <typename Arg>
std::tuple<const char*, Arg> prio(Arg&& arg)
{
    return std::tuple<const char*, Arg>("PRIORITY=%d", arg);
}

template <typename ...T, size_t ...I>
void send_log(std::tuple<T...>&& e, std::integer_sequence<size_t, I...>)
{
    sd_journal_send(std::get<I>(e)..., NULL);
}

template <typename ...T, typename I = std::make_index_sequence<sizeof...(T)>>
void send_tuple_log(std::tuple<T...>&& entry)
{
    send_log(std::move(entry), I());
}

template <typename Entry, typename ...Rest>
void send_entry_log(Entry&& e, Rest&&... r)
{
    send_tuple_log(std::move(e));
}

} // namespace details

template <level L, typename Msg, typename ...Entry>
void log(Msg msg, Entry... entry)
{
    details::send_entry_log(std::tuple_cat(details::prio(L), msg, entry...));
}

template <typename Arg, typename ...Args>
std::tuple<Arg, Args...> entry(Arg&& arg, Args&&... args)
{
    return std::tuple<Arg, Args...>(arg, args...);
}

} // namespace logging

} // namespace phosphor

