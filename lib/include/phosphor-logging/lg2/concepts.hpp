#pragma once

#include <concepts>

namespace lg2::details
{

/** Matches a type T which is anything except one of those in Ss. */
template <typename T, typename... Ss>
concept any_but = (... && !std::convertible_to<T, Ss>);

}; // namespace lg2::details
