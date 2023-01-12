#include "elog_entry.hpp"

#include "elog_serialize.hpp"
#include "log_manager.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <xyz/openbmc_project/Common/File/error.hpp>

namespace phosphor
{
namespace logging
{

// TODO Add interfaces to handle the error log id numbering

void Entry::delete_()
{
    parent.erase(id());
}

bool Entry::resolved(bool value)
{
    auto current =
        sdbusplus::xyz::openbmc_project::Logging::server::Entry::resolved();
    if (value != current)
    {
        value ? associations({}) : associations(assocs);
        current =
            sdbusplus::xyz::openbmc_project::Logging::server::Entry::resolved(
                value);

        uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count();
        updateTimestamp(ms);

        serialize(*this);
    }

    return current;
}

std::string Entry::eventId(std::string value)
{
    auto current =
        sdbusplus::xyz::openbmc_project::Logging::server::Entry::eventId();
    if (value != current)
    {
        current =
            sdbusplus::xyz::openbmc_project::Logging::server::Entry::eventId(
                value);
        serialize(*this);
    }

    return current;
}

std::string Entry::resolution(std::string value)
{
    auto current =
        sdbusplus::xyz::openbmc_project::Logging::server::Entry::resolution();
    if (value != current)
    {
        current =
            sdbusplus::xyz::openbmc_project::Logging::server::Entry::resolution(
                value);
        serialize(*this);
    }

    return current;
}

sdbusplus::message::unix_fd Entry::getEntry()
{
    int fd = open(path().c_str(), O_RDONLY | O_NONBLOCK);
    if (fd == -1)
    {
        auto e = errno;
        log<level::ERR>("Failed to open Entry File", entry("ERRNO=%d", e),
                        entry("PATH=%s", path().c_str()));
        throw sdbusplus::xyz::openbmc_project::Common::File::Error::Open();
    }

    // Schedule the fd to be closed by sdbusplus when it sends it back over
    // D-Bus.
    sdeventplus::Event event = sdeventplus::Event::get_default();
    fdCloseEventSource = std::make_unique<sdeventplus::source::Defer>(
        event, std::bind(std::mem_fn(&Entry::closeFD), this, fd,
                         std::placeholders::_1));

    return fd;
}

void Entry::closeFD(int fd, sdeventplus::source::EventBase& /*source*/)
{
    close(fd);
    fdCloseEventSource.reset();
}

} // namespace logging
} // namespace phosphor
