#include "elog_entry.hpp"

#include "elog_serialize.hpp"
#include "extensions.hpp"
#include "log_manager.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <phosphor-logging/lg2.hpp>
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
        sdbusplus::server::xyz::openbmc_project::logging::Entry::resolved();
    if (value != current)
    {
        // Resolve operation will be prohibited if delete operation is
        // prohibited.
        for (auto& func : Extensions::getDeleteProhibitedFunctions())
        {
            try
            {
                bool prohibited = false;
                uint32_t entryId = id();
                func(entryId, prohibited);

                if (prohibited)
                {
                    throw sdbusplus::xyz::openbmc_project::Common::Error::
                        Unavailable();
                }
            }
            catch (const sdbusplus::xyz::openbmc_project::Common::Error::
                       Unavailable& e)
            {
                throw;
            }
            catch (const std::exception& e)
            {
                lg2::error("An extension's deleteProhibited function threw an "
                           "exception: {ERROR}",
                           "ERROR", e);
            }
        }
        value ? associations({}) : associations(assocs);
        current =
            sdbusplus::server::xyz::openbmc_project::logging::Entry::resolved(
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
        sdbusplus::server::xyz::openbmc_project::logging::Entry::eventId();
    if (value != current)
    {
        current =
            sdbusplus::server::xyz::openbmc_project::logging::Entry::eventId(
                value);
        serialize(*this);
    }

    return current;
}

std::string Entry::resolution(std::string value)
{
    auto current =
        sdbusplus::server::xyz::openbmc_project::logging::Entry::resolution();
    if (value != current)
    {
        current =
            sdbusplus::server::xyz::openbmc_project::logging::Entry::resolution(
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
        lg2::error("Failed to open Entry File ERRNO={ERRNO}, PATH={PATH}",
                   "ERRNO", e, "PATH", path());
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
