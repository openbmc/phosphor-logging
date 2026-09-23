#include "processor.hpp"

#include <unistd.h>

#include <phosphor-logging/lg2.hpp>

#include <array>
#include <cerrno>
#include <cstdint>
#include <ranges>
#include <utility>
#include <vector>

namespace phosphor::logging::cper
{
PHOSPHOR_LOG2_USING;

namespace
{
auto readCPERBinary(int fd) -> std::vector<uint8_t>;
} // namespace

void Processor::method_call(process_t, sdbusplus::object_path source,
                            ContentType type, sdbusplus::message::unix_fd data)
{
    auto raw = readCPERBinary(data);

    info("CPER Process request: source={SOURCE} type={TYPE} size={SIZE}",
         "SOURCE", source.str, "TYPE", type, "SIZE", raw.size());

    decoder.queue(std::move(source), type, std::move(raw));
}

namespace
{
auto readCPERBinary(int fd) -> std::vector<uint8_t>
{
    // The sender may leave the offset at EOF, so rewind when possible.
    // Pipes/sockets fail with ESPIPE; just read from the current offset.
    if (lseek(fd, 0, SEEK_SET) == -1 && errno != ESPIPE)
    {
        auto e = errno;
        warning("Could not seek on CPER data: {ERRNO}", "ERRNO", e);
    }

    std::vector<uint8_t> data;
    std::array<uint8_t, 64 * 1024> chunk{};
    while (true)
    {
        auto n = read(fd, chunk.data(), chunk.size());
        if (n == -1)
        {
            auto e = errno;
            if (e == EINTR)
            {
                continue;
            }
            error("Could not read CPER data: {ERRNO}", "ERRNO", e);
            break;
        }
        if (n == 0)
        {
            break;
        }
        data.append_range(chunk | std::views::take(n));
    }

    return data;
}
} // namespace

} // namespace phosphor::logging::cper
