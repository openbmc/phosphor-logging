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

#include <systemd/sd-journal.h>

#include <sdbusplus/server/transaction.hpp>
#include <tuple>
#include <type_traits>

namespace phosphor
{

namespace logging
{

/** @enum level
 *  @brief Enum for priority level
 */
enum class level
{
    EMERG = LOG_EMERG,
    ALERT = LOG_ALERT,
    CRIT = LOG_CRIT,
    ERR = LOG_ERR,
    WARNING = LOG_WARNING,
    NOTICE = LOG_NOTICE,
    INFO = LOG_INFO,
    DEBUG = LOG_DEBUG,
};

/** @fn log()
 *  @brief Log message to systemd journal
 *  @tparam L - Priority level
 *  @param[in] msg - Message to be logged in C-string format
 *  @param[in] entry - Metadata fields to be added to the message
 *  @details Usage: log<level::XX>(const char*, entry(*format), entry()...);
 *  @example log<level::DEBUG>(
 *                 "Simple Example");
 *           char msg_str[] = "File not found";
 *           log<level::DEBUG>(
 *                 msg_str,
 *                 entry("MY_METADATA=%s_%x, name, number));
 */
template <level L, typename Msg, typename... Entry>
void log(Msg msg, Entry... entry);

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
template <typename Arg, typename... Args>
constexpr auto entry(Arg&& arg, Args&&... args);

namespace details
{

/** @fn prio()
 *  @brief Prepend PRIORITY= to the input priority string.
 *    This is required by sd_journal_send().
 *  @tparam L - Priority level
 */
template <level L>
constexpr auto prio()
{
    constexpr const char* prio_str = "PRIORITY=%d";
    constexpr const auto prio_tuple = std::make_tuple(prio_str, L);
    return prio_tuple;
}

/** @fn helper_log()
 *  @brief Helper function for details::log(). Log request to journal.
 *  @tparam T - Type of tuple
 *  @tparam I - std::integer_sequence of indexes (0..N) for each tuple element
 *  @param[in] e - Tuple containing the data to be logged
 *  @param[unnamed] - std::integer_sequence of tuple's index values
 */
template <typename T, size_t... I>
void helper_log(T&& e, std::integer_sequence<size_t, I...>)
{
    // https://www.freedesktop.org/software/systemd/man/sd_journal_print.html
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
    constexpr auto e_size = std::tuple_size<std::decay_t<T>>::value;
    helper_log(std::forward<T>(e), std::make_index_sequence<e_size>{});
}

} // namespace details

template <level L, typename Msg, typename... Entry>
void log(Msg msg, Entry... e)
{
    static_assert((std::is_same<const char*, std::decay_t<Msg>>::value ||
                   std::is_same<char*, std::decay_t<Msg>>::value),
                  "First parameter must be a C-string.");

    constexpr const char* msg_str = "MESSAGE=%s";
    const auto msg_tuple = std::make_tuple(msg_str, std::forward<Msg>(msg));

    constexpr auto transactionStr = "TRANSACTION_ID=%lld";
    auto transactionId = sdbusplus::server::transaction::get_id();

    auto log_tuple = std::tuple_cat(details::prio<L>(), msg_tuple,
                                    entry(transactionStr, transactionId),
                                    std::forward<Entry>(e)...);
    details::log(log_tuple);
}

enum class SensorAssertion : bool
{
    asserted = true,
    deasserted = false,
};

// The selTuple collects information needed to add a new IPMI SEL record to
// the journal.  The selTuple consists of the following objects which are
// used to populate various bytes of data required in an IPMI SEL record:
//     std::string: The path of the IPMI sensor
//     std::vector<uint8_t>: Up to three bytes of event data
//     SensorAssertion: Indicator of the direction of the event
using selTuple = std::tuple<std::string, std::vector<uint8_t>, SensorAssertion>;

template <level L, typename Msg>
void log(Msg msg, selTuple&& selInfo)
{
    static_assert((std::is_same<const char*, std::decay_t<Msg>>::value ||
                   std::is_same<char*, std::decay_t<Msg>>::value),
                  "First parameter must be a C-string.");

    static constexpr char const* ipmiSELObject =
        "xyz.openbmc_project.Logging.IPMI";
    static constexpr char const* ipmiSELPath =
        "/xyz/openbmc_project/Logging/IPMI";
    static constexpr char const* ipmiSELAddInterface =
        "xyz.openbmc_project.Logging.IPMI";

    sdbusplus::bus::bus dbus = sdbusplus::bus::new_system();

    std::string sensorPath;
    std::vector<uint8_t> selData;
    SensorAssertion sensorAssertion;
    std::tie(sensorPath, selData, sensorAssertion) = selInfo;

    sdbusplus::message::message writeSEL = dbus.new_method_call(
        ipmiSELObject, ipmiSELPath, ipmiSELAddInterface, "IpmiSelAdd");
    writeSEL.append(std::string(msg), sensorPath, selData,
                    static_cast<bool>(sensorAssertion),
                    static_cast<uint16_t>(0x20));
    try
    {
        sdbusplus::message::message writeSELResp = dbus.call(writeSEL);
    }
    catch (sdbusplus::exception_t& e)
    {
        log<level::ERR>(e.what());
        return;
    }
}

template <class T>
struct is_printf_argtype
    : std::integral_constant<
          bool,
          (std::is_integral<typename std::remove_reference<T>::type>::value ||
           std::is_enum<typename std::remove_reference<T>::type>::value ||
           std::is_floating_point<
               typename std::remove_reference<T>::type>::value ||
           std::is_pointer<typename std::decay<T>::type>::value)>
{
};

template <class T>
struct is_char_ptr_argtype
    : std::integral_constant<
          bool,
          (std::is_pointer<typename std::decay<T>::type>::value &&
           std::is_same<typename std::remove_cv<typename std::remove_pointer<
                            typename std::decay<T>::type>::type>::type,
                        char>::value)>
{
};

template <bool...>
struct bool_pack;

template <bool... bs>
using all_true = std::is_same<bool_pack<bs..., true>, bool_pack<true, bs...>>;

template <typename Arg, typename... Args>
constexpr auto entry(Arg&& arg, Args&&... args)
{
    static_assert(is_char_ptr_argtype<Arg>::value,
                  "bad argument type: use char*");
    static_assert(all_true<is_printf_argtype<Args>::value...>::value,
                  "bad argument type: use string.c_str() if needed");
    const auto entry_tuple =
        std::make_tuple(std::forward<Arg>(arg), std::forward<Args>(args)...);
    return entry_tuple;
}

template <typename Path, typename SelData>
constexpr auto ipmiSelEntry(Path&& path, SelData&& selData,
                            SensorAssertion&& sensorAssertion)
{
    static_assert(is_char_ptr_argtype<Path>::value,
                  "bad argument type: use char*");
    static_assert(
        std::is_same<std::vector<uint8_t>, std::decay_t<SelData>>::value,
        "bad argument type: use std::vector<uint8_t>");
    selTuple selInfo = std::make_tuple(
        std::forward<Path>(path), std::forward<SelData>(selData),
        std::forward<SensorAssertion>(sensorAssertion));
    return selInfo;
}

} // namespace logging

} // namespace phosphor
