#define SD_JOURNAL_SUPPRESS_LOCATION

#include <systemd/sd-journal.h>
#include <unistd.h>

#include <phosphor-logging/lg2.hpp>

#include <algorithm>
#include <bitset>
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <source_location>
#include <sstream>
#include <utility>
#include <vector>

namespace lg2::details
{
/** Convert unsigned to string using format flags. */
static std::string value_to_string(uint64_t f, uint64_t v)
{
    switch (f & (hex | bin | dec).value)
    {
        // For binary, use bitset<>::to_string.
        // Treat values without a field-length format flag as 64 bit.
        case bin.value:
        {
            switch (f & (field8 | field16 | field32 | field64).value)
            {
                case field8.value:
                {
                    return "0b" + std::bitset<8>(v).to_string();
                }
                case field16.value:
                {
                    return "0b" + std::bitset<16>(v).to_string();
                }
                case field32.value:
                {
                    return "0b" + std::bitset<32>(v).to_string();
                }
                case field64.value:
                default:
                {
                    return "0b" + std::bitset<64>(v).to_string();
                }
            }
        }

        // For hex, use the appropriate sprintf.
        case hex.value:
        {
            char value[19];
            const char* format = nullptr;

            switch (f & (field8 | field16 | field32 | field64).value)
            {
                case field8.value:
                {
                    format = "0x%02" PRIx64;
                    break;
                }

                case field16.value:
                {
                    format = "0x%04" PRIx64;
                    break;
                }

                case field32.value:
                {
                    format = "0x%08" PRIx64;
                    break;
                }

                case field64.value:
                {
                    format = "0x%016" PRIx64;
                    break;
                }

                default:
                {
                    format = "0x%" PRIx64;
                    break;
                }
            }

            snprintf(value, sizeof(value), format, v);
            return value;
        }

        // For dec, use the simple to_string.
        case dec.value:
        default:
        {
            return std::to_string(v);
        }
    }
}

/** Convert signed to string using format flags. */
static std::string value_to_string(uint64_t f, int64_t v)
{
    // If hex or bin was requested just use the unsigned formatting
    // rules. (What should a negative binary number look like otherwise?)
    if (f & (hex | bin).value)
    {
        return value_to_string(f, static_cast<uint64_t>(v));
    }
    return std::to_string(v);
}

/** Convert float to string using format flags. */
static std::string value_to_string(uint64_t, double v)
{
    // No format flags supported for floats.
    return std::to_string(v);
}

// Positions of various strings in an iovec.
static constexpr size_t pos_msg = 0;
static constexpr size_t pos_fmtmsg = 1;
static constexpr size_t pos_prio = 2;
static constexpr size_t pos_file = 3;
static constexpr size_t pos_line = 4;
static constexpr size_t pos_func = 5;
static constexpr size_t static_locs = pos_func + 1;

/** No-op output of a message. */
static void noop_extra_output(level, const std::source_location&,
                              const std::string&)
{}

/** std::cerr output of a message. */
static void cerr_extra_output(level l, const std::source_location& s,
                              const std::string& m)
{
    static const int maxLogLevel = []() {
        const char* logLevel = getenv("LG2_LOG_LEVEL");
        if (logLevel == nullptr)
        {
            // Default to logging all levels if not set
            return std::to_underlying(level::debug);
        }
        try
        {
            int level = std::stoi(logLevel);
            return std::clamp(level, std::to_underlying(level::emergency),
                              std::to_underlying(level::debug));
        }
        catch (const std::exception&)
        {
            return std::to_underlying(level::debug);
        }
    }();

    if (std::to_underlying(l) > maxLogLevel)
    {
        return;
    }

    static const char* const defaultFormat = []() {
        const char* f = getenv("LG2_FORMAT");
        if (nullptr == f)
        {
            f = "<%l> %m";
        }
        return f;
    }();

    const char* format = defaultFormat;

    std::stringstream stream;

    while (*format)
    {
        if (*format != '%')
        {
            stream << *format;
            ++format;
            continue;
        }

        ++format;
        switch (*format)
        {
            case '%':
            case '\0':
                stream << '%';
                break;

            case 'f':
                stream << s.function_name();
                break;

            case 'F':
                stream << s.file_name();
                break;

            case 'l':
                stream << static_cast<uint64_t>(l);
                break;

            case 'L':
                stream << s.line();
                break;

            case 'm':
                stream << m;
                break;

            default:
                stream << '%' << *format;
                break;
        }

        if (*format != '\0')
        {
            ++format;
        }
    }

    static std::mutex mutex;

    // Prevent multiple threads from clobbering the stderr lines of each other
    std::scoped_lock lock(mutex);

    // Ensure this line makes it out before releasing mutex for next line
    std::cerr << stream.str() << std::endl;
}

// Use the cerr output method if we are on a TTY or if explicitly set via
// environment variable.
static auto extra_output_method =
    (isatty(fileno(stderr)) || nullptr != getenv("LG2_FORCE_STDERR"))
        ? cerr_extra_output
        : noop_extra_output;

// Skip sending debug messages to journald if "DEBUG_INVOCATION" is not set
// per systemd.exec manpage.
static auto send_debug_to_journal = nullptr != getenv("DEBUG_INVOCATION");

// Do_log implementation.
void do_log(level l, const std::source_location& s, const char* m, ...)
{
    using namespace std::string_literals;

    std::vector<std::string> strings{static_locs};

    std::string message{m};

    // Assign all the static fields.
    strings[pos_fmtmsg] = "LOG2_FMTMSG="s + m;
    strings[pos_prio] = "PRIORITY="s + std::to_string(static_cast<uint64_t>(l));
    strings[pos_file] = "CODE_FILE="s + s.file_name();
    strings[pos_line] = "CODE_LINE="s + std::to_string(s.line());
    strings[pos_func] = "CODE_FUNC="s + s.function_name();

    // Handle all the va_list args.
    std::va_list args;
    va_start(args, m);
    while (true)
    {
        // Get the header out.
        auto h_ptr = va_arg(args, const char*);
        if (h_ptr == nullptr)
        {
            break;
        }
        std::string h{h_ptr};

        // Get the format flag.
        auto f = va_arg(args, uint64_t);

        // Handle the value depending on which type format flag it has.
        std::string value = {};
        switch (f & (signed_val | unsigned_val | str | floating).value)
        {
            case signed_val.value:
            {
                auto v = va_arg(args, int64_t);
                value = value_to_string(f, v);
                break;
            }

            case unsigned_val.value:
            {
                auto v = va_arg(args, uint64_t);
                value = value_to_string(f, v);
                break;
            }

            case str.value:
            {
                value = va_arg(args, const char*);
                break;
            }

            case floating.value:
            {
                auto v = va_arg(args, double);
                value = value_to_string(f, v);
                break;
            }
        }

        // Create the field for this value.
        strings.emplace_back(h + '=' + value);

        // Check for {HEADER} in the message and replace with value.
        auto h_brace = '{' + h + '}';
        if (auto start = message.find(h_brace); start != std::string::npos)
        {
            message.replace(start, h_brace.size(), value);
        }
    }
    va_end(args);

    // Add the final message into the strings array.
    strings[pos_msg] = "MESSAGE="s + message.data();

    // Transform strings -> iovec.
    std::vector<iovec> iov{};
    std::ranges::transform(strings, std::back_inserter(iov), [](auto& s) {
        return iovec{s.data(), s.length()};
    });

    // Output the iovec.
    if (send_debug_to_journal || l != level::debug)
    {
        sd_journal_sendv(iov.data(), strings.size());
    }
    extra_output_method(l, s, message);
}

} // namespace lg2::details
