#pragma once

#include <phosphor-logging/lg2/flags.hpp>
#include <phosphor-logging/lg2/header.hpp>
#include <phosphor-logging/lg2/level.hpp>
#include <phosphor-logging/lg2/logger.hpp>
#include <phosphor-logging/lg2/source_location.hpp>
#include <sdbusplus/message/native_types.hpp>

#include <concepts>
#include <cstddef>
#include <filesystem>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace lg2::details
{

/** Concept to determine if an item acts like a string.
 *
 *  Something acts like a string if we can construct a string_view from it.
 *  This covers std::string and C-strings.  But, there is subtlety in that
 *  nullptr_t can be used to construct a string_view until C++23, so we need
 *  to exempt out nullptr_t's (otherwise `nullptr` ends up acting like a
 *  string).
 */
template <typename T>
concept string_like_type =
    (std::constructible_from<std::string_view, T> ||
     std::same_as<std::filesystem::path,
                  std::decay_t<T>>)&&!std::same_as<std::nullptr_t, T>;

/** Concept to determine if an item acts like a pointer.
 *
 *  Any pointer, which doesn't act like a string, should be treated as a raw
 *  pointer.
 */
template <typename T>
concept pointer_type = (std::is_pointer_v<T> ||
                        std::same_as<std::nullptr_t, T>)&&!string_like_type<T>;

/** Concept to determine if an item acts like an unsigned_integral.
 *
 *  Except bool because we want bool to be handled special to create nice
 *  `True` and `False` strings.
 */
template <typename T>
concept unsigned_integral_except_bool = !std::same_as<T, bool> &&
                                        std::unsigned_integral<T>;

template <typename T>
concept sdbusplus_enum = sdbusplus::message::has_convert_from_string_v<T>;

template <typename T>
concept exception_type = std::derived_from<std::decay_t<T>, std::exception>;

template <typename T>
concept sdbusplus_object_path =
    std::derived_from<std::decay_t<T>, sdbusplus::message::object_path>;

/** Concept listing all of the types we know how to convert into a format
 *  for logging.
 */
template <typename T>
concept unsupported_log_convert_types =
    !(unsigned_integral_except_bool<T> || std::signed_integral<T> ||
      std::same_as<bool, T> || std::floating_point<T> || string_like_type<T> ||
      pointer_type<T> || sdbusplus_enum<T> || exception_type<T> ||
      sdbusplus_object_path<T>);

/** Any type we do not know how to convert for logging gives a nicer
 *  static_assert message. */
template <log_flags... Fs, unsupported_log_convert_types T>
static auto log_convert(const char*, log_flag<Fs...>, T)
{
    static_assert(!std::is_same_v<T, T>, "Unsupported type for logging value.");
    // Having this return of an empty tuple reduces the error messages.
    return std::tuple<>{};
}

/** Logging conversion for unsigned. */
template <log_flags... Fs, unsigned_integral_except_bool V>
static auto log_convert(const char* h, log_flag<Fs...> f, V v)
{
    // Compile-time checks for valid formatting flags.
    prohibit(f, floating);
    prohibit(f, signed_val);
    prohibit(f, str);

    one_from_set(f, dec | hex | bin);
    one_from_set(f, field8 | field16 | field32 | field64);

    // Add 'unsigned' flag, force to uint64_t for variadic passing.
    return std::make_tuple(h, (f | unsigned_val).value,
                           static_cast<uint64_t>(v));
}

/** Logging conversion for signed. */
template <log_flags... Fs, std::signed_integral V>
static auto log_convert(const char* h, log_flag<Fs...> f, V v)
{
    // Compile-time checks for valid formatting flags.
    prohibit(f, floating);
    prohibit(f, str);
    prohibit(f, unsigned_val);

    one_from_set(f, dec | hex | bin);
    one_from_set(f, field8 | field16 | field32 | field64);

    // Add 'signed' flag, force to int64_t for variadic passing.
    return std::make_tuple(h, (f | signed_val).value, static_cast<int64_t>(v));
}

/** Logging conversion for bool. */
template <log_flags... Fs, std::same_as<bool> V>
static auto log_convert(const char* h, log_flag<Fs...> f, V v)
{
    // Compile-time checks for valid formatting flags.
    prohibit(f, bin);
    prohibit(f, dec);
    prohibit(f, field16);
    prohibit(f, field32);
    prohibit(f, field64);
    prohibit(f, field8);
    prohibit(f, floating);
    prohibit(f, hex);
    prohibit(f, signed_val);
    prohibit(f, unsigned_val);

    // Cast bools to a "True" or "False" string.
    return std::make_tuple(h, (f | str).value, v ? "True" : "False");
}

/** Logging conversion for floating points. */
template <log_flags... Fs, std::floating_point V>
static auto log_convert(const char* h, log_flag<Fs...> f, V v)
{
    // Compile-time checks for valid formatting flags.
    prohibit(f, bin);
    prohibit(f, dec);
    prohibit(f, field16);
    prohibit(f, field32);
    prohibit(f, field64);
    prohibit(f, field8);
    prohibit(f, hex);
    prohibit(f, signed_val);
    prohibit(f, str);
    prohibit(f, unsigned_val);

    // Add 'floating' flag, force to double for variadic passing.
    return std::make_tuple(h, (f | floating).value, static_cast<double>(v));
}

/** Logging conversion for sdbusplus enums. */
template <log_flags... Fs, sdbusplus_enum V>
static auto log_convert(const char* h, log_flag<Fs...> f, V v)
{
    // Compile-time checks for valid formatting flags.
    prohibit(f, bin);
    prohibit(f, dec);
    prohibit(f, field16);
    prohibit(f, field32);
    prohibit(f, field64);
    prohibit(f, field8);
    prohibit(f, floating);
    prohibit(f, hex);
    prohibit(f, signed_val);
    prohibit(f, unsigned_val);

    return std::make_tuple(h, (f | str).value,
                           sdbusplus::message::convert_to_string(v));
}

/** Logging conversion for string-likes. */
template <log_flags... Fs, string_like_type V>
static auto log_convert(const char* h, log_flag<Fs...> f, const V& v)
{
    // Compile-time checks for valid formatting flags.
    prohibit(f, bin);
    prohibit(f, dec);
    prohibit(f, field16);
    prohibit(f, field32);
    prohibit(f, field64);
    prohibit(f, field8);
    prohibit(f, floating);
    prohibit(f, hex);
    prohibit(f, signed_val);
    prohibit(f, unsigned_val);

    // Utiilty to handle conversion to a 'const char*' depending on V:
    //  - 'const char*' and similar use static cast.
    //  - 'std::filesystem::path' use c_str() function.
    //  - 'std::string' and 'std::string_view' use data() function.
    auto str_data = [](const V& v) {
        if constexpr (std::is_same_v<const char*, std::decay_t<V>> ||
                      std::is_same_v<char*, std::decay_t<V>>)
        {
            return static_cast<const char*>(v);
        }
        else if constexpr (std::is_same_v<std::filesystem::path,
                                          std::decay_t<V>>)
        {
            return v.c_str();
        }
        else
        {
            return v.data();
        }
    };

    // Add 'str' flag, force to 'const char*' for variadic passing.
    return std::make_tuple(h, (f | str).value, str_data(v));
}

/** Logging conversion for pointer-types. */
template <log_flags... Fs, pointer_type V>
static auto log_convert(const char* h, log_flag<Fs...> f, V v)
{
    // Compile-time checks for valid formatting flags.
    prohibit(f, bin);
    prohibit(f, dec);
    prohibit(f, field16);
    prohibit(f, field32);
    prohibit(f, field64);
    prohibit(f, field8);
    prohibit(f, floating);
    prohibit(f, signed_val);
    prohibit(f, str);

    // Cast (void*) to a hex-formatted uint64 using the target's pointer-size
    // to determine field-width.
    constexpr static auto new_f = sizeof(void*) == 4 ? field32.value
                                                     : field64.value;

    return std::make_tuple(h, new_f | (hex | unsigned_val).value,
                           reinterpret_cast<uint64_t>(v));
}

/* Logging conversion for exceptions. */
template <log_flags... Fs, exception_type V>
static auto log_convert(const char* h, log_flag<Fs...> f, V&& v)
{
    // Compile-time checks for valid formatting flags.
    prohibit(f, bin);
    prohibit(f, dec);
    prohibit(f, field16);
    prohibit(f, field32);
    prohibit(f, field64);
    prohibit(f, field8);
    prohibit(f, floating);
    prohibit(f, hex);
    prohibit(f, signed_val);
    prohibit(f, unsigned_val);

    // Treat like a string, but get the 'what' from the exception.
    return std::make_tuple(h, (f | str).value, v.what());
}

/* Logging conversion for object path. */
template <log_flags... Fs, sdbusplus_object_path V>
static auto log_convert(const char* h, log_flag<Fs...> f, V&& v)
{
    // Compile-time checks for valid formatting flags.
    prohibit(f, bin);
    prohibit(f, dec);
    prohibit(f, field16);
    prohibit(f, field32);
    prohibit(f, field64);
    prohibit(f, field8);
    prohibit(f, floating);
    prohibit(f, hex);
    prohibit(f, signed_val);
    prohibit(f, unsigned_val);

    // Treat like a string, but get the 'str' from the object path.
    return std::make_tuple(h, (f | str).value, v.str);
}

/** Class to facilitate walking through the arguments of the `lg2::log` function
 *  and ensuring correct parameter types and conversion operations.
 */
class log_conversion
{
  private:
    /** Conversion and validation is complete.  Pass along to the final
     *  do_log variadic function. */
    template <typename... Ts>
    static void done(level l, const lg2::source_location& s, const char* m,
                     Ts&&... ts)
    {
        do_log(l, s, m, ts..., nullptr);
    }

    /** Apply the tuple from the end of 'step' into done.
     *
     *  There are some cases where the tuple must hold a `std::string` in
     *  order to avoid losing data in a temporary (see sdbusplus-enum
     *  conversion), but the `do_log` function wants a `const char*` as
     *  the variadic type.  Run the tuple through a lambda to pull out
     *  the `const char*`'s without losing the temporary held by the tuple.
     */
    static void apply_done(const auto& args_tuple)
    {
        auto squash_string = [](auto& arg) -> decltype(auto) {
            if constexpr (std::is_same_v<const std::string&, decltype(arg)>)
            {
                return arg.data();
            }
            else
            {
                return arg;
            }
        };

        std::apply([squash_string](
                       const auto&... args) { done(squash_string(args)...); },
                   args_tuple);
    }

    /** Handle conversion of a { Header, Flags, Value } argument set. */
    template <typename... Ts, log_flags... Fs, typename V, typename... Ss>
    static void step(std::tuple<Ts...>&& ts, const header_str& h,
                     log_flag<Fs...> f, V&& v, Ss&&... ss)
    {
        static_assert(!std::is_same_v<header_str, std::decay_t<V>>,
                      "Found header_str as value; suggest using std::string to "
                      "avoid unintended conversion.");

        // These two if conditions are similar, except that one calls 'done'
        // since Ss is empty and the other calls the next 'step'.

        // 1. Call `log_convert` on {h, f, v} for proper conversion.
        // 2. Append the results of `log_convert` into the already handled
        //    arguments (in ts).
        // 3. Call the next step in the chain to handle the remainder Ss.

        if constexpr (sizeof...(Ss) == 0)
        {
            apply_done(
                std::tuple_cat(std::move(ts), log_convert(h.data(), f, v)));
        }
        else
        {
            step(std::tuple_cat(std::move(ts), log_convert(h.data(), f, v)),
                 ss...);
        }
    }

    /** Handle conversion of a { Header, Value } argument set. */
    template <typename... Ts, typename V, typename... Ss>
    static void step(std::tuple<Ts...>&& ts, const header_str& h, V&& v,
                     Ss&&... ss)
    {
        static_assert(!std::is_same_v<header_str, std::decay_t<V>>,
                      "Found header_str as value; suggest using std::string to "
                      "avoid unintended conversion.");
        // These two if conditions are similar, except that one calls 'done'
        // since Ss is empty and the other calls the next 'step'.

        // 1. Call `log_convert` on {h, <empty flags>, v} for proper conversion.
        // 2. Append the results of `log_convert` into the already handled
        //    arguments (in ts).
        // 3. Call the next step in the chain to handle the remainder Ss.

        if constexpr (sizeof...(Ss) == 0)
        {
            apply_done(std::tuple_cat(std::move(ts),
                                      log_convert(h.data(), log_flag<>{}, v)));
        }
        else
        {
            step(std::tuple_cat(std::move(ts),
                                log_convert(h.data(), log_flag<>{}, v)),
                 ss...);
        }
    }

    /** Finding a non-string as the first argument of a 2 or 3 argument set
     *  is an error (missing HEADER field). */
    template <typename... Ts, not_constexpr_string H, typename... Ss>
    static void step(std::tuple<Ts...>&&, H, Ss&&...)
    {
        static_assert(std::is_same_v<header_str, H>,
                      "Found value without expected header field.");
    }

    /** Finding a free string at the end is an error (found HEADER but no data).
     */
    template <typename... Ts>
    static void step(std::tuple<Ts...>&&, header_str)
    {
        static_assert(std::is_same_v<std::tuple<Ts...>, header_str>,
                      "Found header field without expected data.");
    }

  public:
    /** Start processing a sequence of arguments to `lg2::log` using `step` or
     * `done`. */
    template <typename... Ts>
    static void start(level l, const lg2::source_location& s, const char* msg,
                      Ts&&... ts)
    {
        // If there are no arguments (ie. just a message), then skip processing
        // and call `done` directly.
        if constexpr (sizeof...(Ts) == 0)
        {
            done(l, s, msg);
        }
        // Handle all the Ts by recursively calling 'step'.
        else
        {
            step(std::forward_as_tuple(l, s, msg), ts...);
        }
    }
};

} // namespace lg2::details
