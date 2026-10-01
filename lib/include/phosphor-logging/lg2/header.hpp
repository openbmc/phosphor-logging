#pragma once

#include <phosphor-logging/lg2/concepts.hpp>
#include <phosphor-logging/lg2/flags.hpp>

#include <algorithm>
#include <array>
#include <string_view>
#include <tuple>
#include <utility>

namespace lg2::details
{

/** A type to handle compile-time validation of header strings. */
struct header_str
{
    // Hold the header string value.
    std::string_view value;

    /** Constructor which performs validation. */
    template <typename T>
    consteval header_str(const T& s) : value(s)
    {
        if (value.size() == 0)
        {
            report_error(
                "journald requires headers must have non-zero length.");
        }
        if (value[0] == '_')
        {
            report_error("journald requires header do not start with "
                         "underscore (_)");
        }

        if (value.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789") !=
            std::string_view::npos)
        {
            report_error(
                "journald requires header may only contain underscore, "
                "uppercase letters, or numbers ([_A-Z0-9]).");
        }

        constexpr std::array reserved{
            "CODE_FILE",   "CODE_FUNC", "CODE_LINE",
            "LOG2_FMTMSG", "MESSAGE",   "PRIORITY",
        };
        if (std::ranges::find(reserved, value) != std::end(reserved))
        {
            report_error("Header name is reserved.");
        }
    }

    /** Cast conversion back to (const char*). */
    operator const char*() const
    {
        return value.data();
    }

    const char* data() const
    {
        return value.data();
    }

  private:
    // This does nothing, but is useful for creating nice compile errors in
    // a constexpr context.
    static void report_error(const char*);
};

/** A helper type for constexpr conversion into header_str, if
 *  'maybe_constexpr_string'.  For non-constexpr string, this does nothing.
 */
template <typename T>
struct header_str_conversion
{
    using type = T;
};

/** Specialization for maybe_constexpr_string. */
template <maybe_constexpr_string T>
struct header_str_conversion<T>
{
    using type = const header_str&;
};

/** std-style _t alias for header_str_conversion. */
template <typename T>
using header_str_conversion_t = typename header_str_conversion<T>::type;

/** Recursive pack converter for lg2 arguments to convert headers into
 *  header_str while preserving flags and values.
 */
template <typename... Ts>
struct convert_args;

template <>
struct convert_args<>
{
    using type = std::tuple<>;
};

// Case 1: Header, Flag, Value, Rest...
template <typename H, typename F, typename V, typename... Rest>
    requires is_log_flag_v<F>
struct convert_args<H, F, V, Rest...>
{
    using type = decltype(std::tuple_cat(
        std::declval<std::tuple<header_str_conversion_t<H>, F, V>>(),
        std::declval<typename convert_args<Rest...>::type>()));
};

// Case 2: Header, Value, Rest... (when second argument is not a flag)
template <typename H, typename V, typename... Rest>
    requires(!is_log_flag_v<V>)
struct convert_args<H, V, Rest...>
{
    using type = decltype(std::tuple_cat(
        std::declval<std::tuple<header_str_conversion_t<H>, V>>(),
        std::declval<typename convert_args<Rest...>::type>()));
};

// Case 3: Single trailing argument (expected to be a header missing data)
template <typename H>
struct convert_args<H>
{
    using type = std::tuple<header_str_conversion_t<H>>;
};

// Fallback: preserve remaining types
template <typename T, typename... Rest>
struct convert_args<T, Rest...>
{
    using type = decltype(std::tuple_cat(
        std::declval<std::tuple<T>>(),
        std::declval<typename convert_args<Rest...>::type>()));
};

template <typename... Ts>
using convert_args_t = typename convert_args<Ts...>::type;

} // namespace lg2::details
