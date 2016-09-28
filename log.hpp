/**
 * Copyright © 2016 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
 *  @details Usage: log<level::XX>(msg(const char*), entry(*format), entry()...);
 *  @example log<level::DEBUG>(
 *                 msg("Simple Example"));
 *           log<level::DEBUG>(
 *                 msg("File not found"),
 *                 entry("MY_METADATA=%s_%x, name, number));
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

/** @fn helper_log()
 *  @brief Helper function for details::log(). Log request to journal.
 *  @tparam T - Type of tuple
 *  @tparam I - std::integer_sequence of indexes (0..N) for each tuple element
 *  @param[in] e - Tuple containing the data to be logged
 *  @param[unnamed] - std::integer_sequence of tuple's index values
 */
template <typename T, size_t ...I>
void helper_log(T&& e, std::integer_sequence<size_t, I...>)
{
    sd_journal_send(std::get<I>(std::forward<T>(e))..., NULL);
}

/** @fn details::log()
 *  @brief Implementation of logging::log() function.
 *         Send request msg and size to helper funct to log it to the journal.
 *  @tparam T - Type of tuple
 *  @param[in] e - Tuple containing the data to be logged
 */
template <typename T>
void log(T&& e)
{
    constexpr auto e_size = std::tuple_size<typename std::decay<T>::type>::value;
    helper_log(std::forward<T>(e), std::make_index_sequence<e_size>{});
}

} // namespace details

template <level L, typename Msg, typename ...Entry>
void log(Msg msg, Entry... entry)
{
    auto log_tuple = std::tuple_cat(details::prio<L>(),
                                    std::forward<Msg>(msg),
                                    std::forward<Entry>(entry)...);
    details::log(log_tuple);
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

