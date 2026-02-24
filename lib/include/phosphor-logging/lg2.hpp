#pragma once

#if __cplusplus < 202002L
#error "phosphor-logging lg2 requires C++20"
#else

#include <phosphor-logging/lg2/concepts.hpp>
#include <phosphor-logging/lg2/conversion.hpp>
#include <phosphor-logging/lg2/flags.hpp>
#include <phosphor-logging/lg2/header.hpp>
#include <phosphor-logging/lg2/level.hpp>

#include <source_location>

namespace lg2
{
/** Implementation of the structured logging `lg2::log` interface. */
template <level S = level::debug, details::any_but<std::source_location>... Ts>
struct log
{
    /** log with a custom source_location.
     *
     *  @param[in] s - The custom source location.
     *  @param[in] msg - The message to log.
     *  @param[in] ts - The rest of the arguments.
     */
    explicit log(const std::source_location& s, const char* msg,
                 details::header_str_conversion_t<Ts&&>... ts)
    {
        details::log_conversion::start(
            S, s, msg,
            std::forward<details::header_str_conversion_t<Ts&&>>(ts)...);
    }

    /** default log (source_location is determined by calling location).
     *
     *  @param[in] msg - The message to log.
     *  @param[in] ts - The rest of the arguments.
     *  @param[in] s - The derived source_location.
     */
    explicit log(
        const char* msg, details::header_str_conversion_t<Ts&&>... ts,
        const std::source_location& s = std::source_location::current()) :
        log(s, msg, std::forward<details::header_str_conversion_t<Ts&&>>(ts)...)
    {}

    // Give a nicer compile error if someone tries to log without a message.
    log() = delete;
};

/** Macro to define aliases for lg2::level(...) -> lg2::log<level>(...)
 *
 *  Creates a simple inherited structure and corresponding deduction guides.
 */
#define PHOSPHOR_LOG2_DECLARE_LEVEL(levelval)                                  \
    template <typename... Ts>                                                  \
    struct levelval : public log<level::levelval, Ts...>                       \
    {                                                                          \
        using log<level::levelval, Ts...>::log;                                \
    };                                                                         \
                                                                               \
    template <typename... Ts>                                                  \
    explicit levelval(const char*, Ts&&...) -> levelval<Ts...>;                \
                                                                               \
    template <typename... Ts>                                                  \
    explicit levelval(const std::source_location&, const char*, Ts&&...)       \
        ->levelval<Ts...>

// Enumerate the aliases for each log level.
PHOSPHOR_LOG2_DECLARE_LEVEL(emergency);
PHOSPHOR_LOG2_DECLARE_LEVEL(alert);
PHOSPHOR_LOG2_DECLARE_LEVEL(critical);
PHOSPHOR_LOG2_DECLARE_LEVEL(error);
PHOSPHOR_LOG2_DECLARE_LEVEL(warning);
PHOSPHOR_LOG2_DECLARE_LEVEL(notice);
PHOSPHOR_LOG2_DECLARE_LEVEL(info);
PHOSPHOR_LOG2_DECLARE_LEVEL(debug);

#undef PHOSPHOR_LOG2_DECLARE_LEVEL

/** Handy scope-level `using` to get the necessary bits of lg2. */
#define PHOSPHOR_LOG2_USING                                                    \
    using lg2::emergency;                                                      \
    using lg2::alert;                                                          \
    using lg2::critical;                                                       \
    using lg2::error;                                                          \
    using lg2::warning;                                                        \
    using lg2::notice;                                                         \
    using lg2::info;                                                           \
    using lg2::debug
// We purposefully left out `using lg2::log` above to avoid collisions with
// the math function `log`.  There is little use for direct calls to `lg2::log`,
// when the level-aliases are available, since it is just a more awkward syntax.

/** Scope-level `using` to get the everything, including format flags. */
#define PHOSPHOR_LOG2_USING_WITH_FLAGS                                         \
    PHOSPHOR_LOG2_USING;                                                       \
    PHOSPHOR_LOG2_USING_FLAGS

} // namespace lg2

#endif
