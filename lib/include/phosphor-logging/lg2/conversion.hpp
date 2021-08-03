#pragma once

#include <concepts>
#include <cstddef>
#include <phosphor-logging/lg2/flags.hpp>
#include <phosphor-logging/lg2/level.hpp>
#include <phosphor-logging/lg2/logger.hpp>
#include <source_location>
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
    std::constructible_from<std::string_view, T> && !std::same_as<nullptr_t, T>;

/** Concept to determine if an item acts like a pointer.
 *
 *  Any pointer, which doesn't act like a string, should be treated as a raw
 *  pointer.
 */
template <typename T>
concept pointer_type = (std::is_pointer_v<T> ||
                        std::same_as<nullptr_t, T>)&&!string_like_type<T>;

/** Concept to determine if an item acts like an unsigned_integral.
 *
 *  Except bool because we want bool to be handled special to create nice
 *  `True` and `False` strings.
 */
template <typename T>
concept unsigned_integral_except_bool =
    !std::same_as<T, bool> && std::unsigned_integral<T>;

/** Concept listing all of the types we know how to convert into a format
 *  for logging.
 */
template <typename T>
concept unsupported_log_convert_types =
    !(unsigned_integral_except_bool<T> || std::signed_integral<T> ||
      std::same_as<bool, T> || std::floating_point<T> || string_like_type<T> ||
      pointer_type<T>);

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

/** Logging conversion for string-likes. */
template <log_flags... Fs, string_like_type V>
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

    // Add 'str' flag, force to 'const char*' for variadic passing.
    //
    // It may appear unsafe that we are using a temporary string_view object
    // for conversion, but it is a safe and consise way to do this conversion.
    //  - (const char*) goes through string_view, but data() returns the
    //    original pointer.
    //  - (string_view) makes a duplicate temporary pointing to the same
    //    original data pointer.
    //  - (string) uses a temporary string_view to return a pointer to the
    //    original std::string v.data().
    // Without this pass-through-string_view, we would need some additional
    // template magic to differentiate between types requiring direct pointer
    // passing vs v.data() to obtain the underlying pointer.
    return std::make_tuple(h, (f | str).value, std::string_view{v}.data());
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
    constexpr static auto new_f =
        sizeof(void*) == 4 ? field32.value : field64.value;

    return std::make_tuple(h, new_f | (hex | unsigned_val).value,
                           reinterpret_cast<uint64_t>(v));
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
    static void done(level l, const std::source_location& s,
                     const std::string_view& m, Ts&&... ts)
    {
        do_log(l, s, m, sizeof...(Ts) / 3, std::forward<Ts>(ts)...);
    }

    /** Handle conversion of a { Header, Flags, Value } argument set. */
    template <typename... Ts, std::convertible_to<const char*> H,
              log_flags... Fs, typename V, typename... Ss>
    static void step(std::tuple<Ts...> ts, H&& h, log_flag<Fs...> f, V&& v,
                     Ss&&... ss)
    {
        // These two if conditions are similar, except that one calls 'done'
        // since Ss is empty and the other calls the next 'step'.

        // 1. Call `log_convert` on {h, f, v} for proper conversion.
        // 2. Append the results of `log_convert` into the already handled
        //    arguments (in ts).
        // 3. Call the next step in the chain to handle the remainder Ss.

        if constexpr (sizeof...(Ss) == 0)
        {
            std::apply(
                [](auto... args) { done(args...); },
                std::tuple_cat(ts, log_convert(std::forward<H>(h), f, v)));
        }
        else
        {
            step(std::tuple_cat(ts, log_convert(std::forward<H>(h), f, v)),
                 std::forward<Ss>(ss)...);
        }
    }

    /** Handle conversion of a { Header, Value } argument set. */
    template <typename... Ts, std::convertible_to<const char*> H, typename V,
              typename... Ss>
    static void step(std::tuple<Ts...> ts, H&& h, V&& v, Ss&&... ss)
    {
        // These two if conditions are similar, except that one calls 'done'
        // since Ss is empty and the other calls the next 'step'.

        // 1. Call `log_convert` on {h, <empty flags>, v} for proper conversion.
        // 2. Append the results of `log_convert` into the already handled
        //    arguments (in ts).
        // 3. Call the next step in the chain to handle the remainder Ss.

        if constexpr (sizeof...(Ss) == 0)
        {
            std::apply([](auto... args) { done(args...); },
                       std::tuple_cat(ts, log_convert(std::forward<H>(h),
                                                      log_flag<>{}, v)));
        }
        else
        {
            step(std::tuple_cat(
                     ts, log_convert(std::forward<H>(h), log_flag<>{}, v)),
                 std::forward<Ss>(ss)...);
        }
    }

    /** Finding a non-string as the first argument of a 2 or 3 argument set
     *  is an error (missing HEADER field). */
    template <typename... Ts, any_but<const char*> H, typename... Ss>
    static void step(const std::tuple<Ts...>&, H, Ss&&...)
    {
        static_assert(std::is_same_v<const char*, H>,
                      "Found value without expected header field.");
    }

    /** Finding a free string at the end is an error (found HEADER but no data).
     */
    template <typename... Ts, std::convertible_to<const char*> H>
    static void step(const std::tuple<Ts...>&, H)
    {
        static_assert(!std::is_convertible_v<const char*, H>,
                      "Found header field without expected data.");
    }

  public:
    /** Start processing a sequence of arguments to `lg2::log` using `step` or
     * `done`. */
    template <typename... Ts>
    static void start(level l, const std::source_location& s,
                      const std::string_view& msg, Ts&&... ts)
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
            step(std::forward_as_tuple(l, s, msg), std::forward<Ts>(ts)...);
        }
    }
};

} // namespace lg2::details
