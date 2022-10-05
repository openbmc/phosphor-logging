#pragma once

#include <concepts>
#include <type_traits>

namespace lg2::details
{

/** Matches a type T which is anything except one of those in Ss. */
template <typename T, typename... Ss>
concept any_but = (... && !std::convertible_to<T, Ss>);

/** Determine if a type might be a constexpr string: (const char (&)[N]) */
template <typename T>
concept maybe_constexpr_string = std::is_array_v<std::remove_cvref_t<T>> &&
                                 std::same_as<const char*, std::decay_t<T>>;

/** Determine if a type is certainly not a constexpr string. */
template <typename T>
concept not_constexpr_string = (!maybe_constexpr_string<T>);

}; // namespace lg2::details
