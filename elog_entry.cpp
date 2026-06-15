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
namespace
{

using CperType =
    sdbusplus::xyz::openbmc_project::Logging::Diagnostic::server::CPER::Type;

CperType toCperType(const std::string& type)
{
    using Type = CperType;

    // CPER Section
    if (type == "CPERSection" ||
        type == "xyz.openbmc_project.Logging.Diagnostic.CPER.Type.CPERSection")
    {
        return Type::CPERSection;
    }

    // CPER Full Record
    if (type == "CPER" ||
        type == "xyz.openbmc_project.Logging.Diagnostic.CPER.Type.CPER")
    {
        return Type::CPER;
    }

    // OEM / Vendor-specific
    if (type == "OEM" ||
        type == "xyz.openbmc_project.Logging.Diagnostic.CPER.Type.OEM")
    {
        return Type::OEM;
    }

    // Unknown → safer fallback
    // Prefer OEM instead of CPER to avoid misinterpretation
    return Type::OEM;
}

} // anonymous namespace

void Entry::persist()
{
    serialize(*this);
    serializeJSON(*this);
}

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
                func(id(), prohibited);

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

        persist();
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
        persist();
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
        persist();
    }

    return current;
}

sdbusplus::message::unix_fd Entry::getEntry()
{
    std::string jsonPath = path() + ".json";
    int fd = open(jsonPath.c_str(), O_RDONLY | O_NONBLOCK);
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
        event, std::bind_front(&Entry::closeFD, this, fd));

    return fd;
}

void Entry::closeFD(int fd, sdeventplus::source::EventBase& /*source*/)
{
    close(fd);
    fdCloseEventSource.reset();
}

Entry& Entry::operator=(const Entry& source)
{
    if (resolved() != source.resolved())
    {
        resolved(source.resolved());
    }

    if (resolution() != source.resolution())
    {
        resolution(source.resolution());
    }

    if (eventId() != source.eventId())
    {
        eventId(source.eventId());
    }

    if (severity() != source.severity())
    {
        severity(source.severity());
    }

    if (message() != source.message())
    {
        message(source.message());
    }

    if (additionalData() != source.additionalData())
    {
        additionalData(source.additionalData());
    }

    if (serviceProviderNotify() != source.serviceProviderNotify())
    {
        serviceProviderNotify(source.serviceProviderNotify());
    }

    return *this;
}

void Entry::createCperInterface(const std::string& type,
                                const std::string& diagInfo,
                                const std::string& dataObj)
{
    if (cperIface)
    {
        return;
    }

    cperIface = std::make_unique<CperIface>(busRef, objPathStr.c_str());
    cperIface->type(toCperType(type));

    if (!diagInfo.empty())
    {
        cperIface->diagnosticInfo(diagInfo);
    }

    // Optional additional data reference (for large payloads)
    if (!dataObj.empty())
    {
        cperIface->additionalDataObject(dataObj);
    }
}

} // namespace logging
} // namespace phosphor
