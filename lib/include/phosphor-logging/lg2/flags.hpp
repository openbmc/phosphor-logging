#pragma once

#include <bit>
#include <cstdint>
#include <type_traits>

namespace lg2
{
namespace details
{

/** Type to hold a set of logging flags. */
template <typename... Fs>
struct log_flag
{
    /** Combined bit-set value of the held flags. */
    static constexpr auto value = (0 | ... | Fs::value);
};

/** Constant for the "zero" flag. */
static constexpr auto log_flag_seq_start =
    std::integral_constant<uint64_t, 0>{};

/** Concept to determine if a type is one of the defined flag types. */
template <typename T>
concept log_flags = requires { T::i_am_a_lg2_flag_type; };

/** Operator to combine log_flag sets together. */
template <log_flags... As, log_flags... Bs>
constexpr auto operator|(const log_flag<As...>, const log_flag<Bs...>)
{
    return details::log_flag<As..., Bs...>{};
}

/** Static check to determine if a prohibited flag is found in a flag set. */
template <log_flags... Fs, log_flags F>
constexpr void prohibit(log_flag<Fs...>, log_flag<F>)
{
    static_assert(!(... || std::is_same_v<Fs, F>),
                  "Prohibited flag found for value type.");
}

/** Static check to determine if any conflicting flags are found in a flag set.
 */
template <log_flags... As, log_flags... Bs>
constexpr void one_from_set(log_flag<As...> a, log_flag<Bs...> b)
{
    static_assert(std::popcount(a.value & b.value) < 2,
                  "Conflicting flags found for value type.");
}

} // namespace details

// Macro used to define all of the logging flags as a sequence of bitfields.
//  - Creates a struct-type where the `value` is 1 bit higher than the previous
//    so that it can be combined together with other flags using `log_flag`.
//  - Creates a static instance of the flag in the `lg2` namespace.
#define PHOSPHOR_LOG2_DECLARE_FLAG(flagname, prev)                             \
    namespace details                                                          \
    {                                                                          \
    struct flag_##flagname                                                     \
    {                                                                          \
        static constexpr uint64_t value =                                      \
            prev.value == log_flag_seq_start.value ? 1 : (prev.value << 1);    \
                                                                               \
        static constexpr bool i_am_a_lg2_flag_type = true;                     \
    };                                                                         \
    }                                                                          \
    static constexpr auto flagname =                                           \
        details::log_flag<details::flag_##flagname>()

// Set of supported logging flags.
//      Please keep these sorted!
PHOSPHOR_LOG2_DECLARE_FLAG(bin, log_flag_seq_start);
PHOSPHOR_LOG2_DECLARE_FLAG(dec, bin);
PHOSPHOR_LOG2_DECLARE_FLAG(field8, dec);
PHOSPHOR_LOG2_DECLARE_FLAG(field16, field8);
PHOSPHOR_LOG2_DECLARE_FLAG(field32, field16);
PHOSPHOR_LOG2_DECLARE_FLAG(field64, field32);
PHOSPHOR_LOG2_DECLARE_FLAG(floating, field64);
PHOSPHOR_LOG2_DECLARE_FLAG(hex, floating);
PHOSPHOR_LOG2_DECLARE_FLAG(signed_val, hex);
PHOSPHOR_LOG2_DECLARE_FLAG(str, signed_val);
PHOSPHOR_LOG2_DECLARE_FLAG(unsigned_val, str);

#undef PHOSPHOR_LOG2_DECLARE_FLAG

/** Handy scope-level `using` to get the format flags. */
#define PHOSPHOR_LOG2_USING_FLAGS                                              \
    using lg2::bin;                                                            \
    using lg2::dec;                                                            \
    using lg2::field8;                                                         \
    using lg2::field16;                                                        \
    using lg2::field32;                                                        \
    using lg2::field64;                                                        \
    using lg2::floating;                                                       \
    using lg2::hex;                                                            \
    using lg2::signed_val;                                                     \
    using lg2::str;                                                            \
    using lg2::unsigned_val

} // namespace lg2
