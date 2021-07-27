#define SD_JOURNAL_SUPPRESS_LOCATION

#include <systemd/sd-journal.h>
#include <unistd.h>

#include <algorithm>
#include <bitset>
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <phosphor-logging/lg2.hpp>
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
static constexpr size_t pos_prio = 1;
static constexpr size_t pos_file = 2;
static constexpr size_t pos_line = 3;
static constexpr size_t pos_func = 4;
static constexpr size_t static_locs = pos_func + 1;

/** No-op output of an iovec. */
static void noop_iov_output(const iovec*, size_t)
{
}

/** std::cerr output of an iovec. */
static void cerr_iov_output(const iovec* iovec, size_t count)
{
    std::cerr << "---------------------------------------------------------\n";
    std::cerr << static_cast<const char*>(iovec[pos_prio].iov_base) << ' '
              << static_cast<const char*>(iovec[pos_msg].iov_base) << '\n';
    std::cerr << static_cast<const char*>(iovec[pos_file].iov_base) << ' '
              << static_cast<const char*>(iovec[pos_line].iov_base) << ' '
              << static_cast<const char*>(iovec[pos_func].iov_base) << '\n';

    for (size_t i = static_locs; i < count; i++)
    {
        std::cerr << static_cast<const char*>(iovec[i].iov_base) << "\n";
    }
    std::cerr << "---------------------------------------------------------"
              << std::endl;
}

// Use the cerr output method if we are on a TTY.
static auto extra_iov_output =
    isatty(fileno(stderr)) ? cerr_iov_output : noop_iov_output;

// Do_log implementation.
void do_log(level l, const std::source_location& s, const std::string_view& m,
            size_t count, ...)
{
    using namespace std::string_literals;

    const size_t entries = count + static_locs;
    std::vector<std::string> strings{entries};

    // Assign all the static fields.
    strings[pos_msg] = "MESSAGE="s + m.data();
    strings[pos_prio] = "PRIORITY="s + std::to_string(static_cast<uint64_t>(l));
    strings[pos_file] = "CODE_FILE="s + s.file_name();
    strings[pos_line] = "CODE_LINE="s + std::to_string(s.line());
    strings[pos_func] = "CODE_FUNC="s + s.function_name();

    // Handle all the va_list args.
    std::va_list args;
    va_start(args, count);
    for (size_t i = static_locs; i < entries; ++i)
    {
        // Get the header out.
        auto h = va_arg(args, const char*);
        strings[i] = std::string(h) + '=';

        // Get the format flag.
        auto f = va_arg(args, uint64_t);

        // Handle the value depending on which type format flag it has.
        switch (f & (signed_val | unsigned_val | str | floating).value)
        {
            case signed_val.value:
            {
                auto v = va_arg(args, int64_t);
                strings[i] += value_to_string(f, v);
                break;
            }

            case unsigned_val.value:
            {
                auto v = va_arg(args, uint64_t);
                strings[i] += value_to_string(f, v);
                break;
            }

            case str.value:
            {
                auto v = va_arg(args, const char*);
                strings[i] += v;
                break;
            }

            case floating.value:
            {
                auto v = va_arg(args, double);
                strings[i] += value_to_string(f, v);
                break;
            }
        }

        // Primary flags need to be added to the message also.
        if (f & primary.value)
        {
            strings[pos_msg] += ' ' + strings[i];
        }
    }
    va_end(args);

    // Trasform strings -> iovec.
    std::vector<iovec> iov{};
    std::ranges::transform(strings, std::back_inserter(iov), [](auto& s) {
        return iovec{s.data(), s.length()};
    });

    // Output the iovec.
    sd_journal_sendv(iov.data(), entries);
    extra_iov_output(iov.data(), entries);
}

} // namespace lg2::details
