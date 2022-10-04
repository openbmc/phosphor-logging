#pragma once

#include <phosphor-logging/lg2/concepts.hpp>

#include <algorithm>
#include <array>
#include <string_view>

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

} // namespace lg2::details
