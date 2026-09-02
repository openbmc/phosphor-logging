#include "event_extensions/utils/artifact.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <phosphor-logging/lg2.hpp>

#include <cerrno>
#include <cstring>
#include <filesystem>
#include <string>
#include <utility>

namespace phosphor::logging::event_extensions::utils
{

namespace
{

class FileDescriptor
{
  public:
    explicit FileDescriptor(int fd) : fd(fd) {}

    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor& operator=(const FileDescriptor&) = delete;

    FileDescriptor(FileDescriptor&& other) noexcept :
        fd(std::exchange(other.fd, -1))
    {}

    FileDescriptor& operator=(FileDescriptor&& other) noexcept
    {
        if (this != &other)
        {
            reset();
            fd = std::exchange(other.fd, -1);
        }

        return *this;
    }

    ~FileDescriptor()
    {
        reset();
    }

    int get() const noexcept
    {
        return fd;
    }

    explicit operator bool() const noexcept
    {
        return fd >= 0;
    }

  private:
    void reset() noexcept
    {
        if (fd >= 0)
        {
            ::close(fd);
            fd = -1;
        }
    }

    int fd{-1};
};

} // namespace

ArtifactRef persistArtifact(std::span<const uint8_t> data,
                            std::string_view prefix,
                            const std::filesystem::path& root)
{
    try
    {
        if (data.empty())
        {
            return {};
        }

        if (data.size() > maxArtifactPayloadSize)
        {
            lg2::error("Artifact payload exceeds maximum size. "
                       "SIZE={SIZE} LIMIT={LIMIT}",
                       "SIZE", data.size(), "LIMIT", maxArtifactPayloadSize);
            return {};
        }

        std::error_code ec;
        std::filesystem::create_directories(root, ec);

        if (ec)
        {
            lg2::error("Failed to create artifact directory {DIR}: {ERROR}",
                       "DIR", root.string(), "ERROR", ec.message());
            return {};
        }

        auto templatePath = (root / (std::string(prefix) + "-XXXXXX")).string();
        templatePath.push_back('\0');
        FileDescriptor fd(::mkstemp(templatePath.data()));
        if (!fd)
        {
            lg2::error("Failed to create artifact: {ERROR}", "ERROR",
                       std::strerror(errno));
            return {};
        }

        const auto* current = reinterpret_cast<const char*>(data.data());
        auto remaining = static_cast<ssize_t>(data.size());

        while (remaining > 0)
        {
            const auto rc = ::write(fd.get(), current, remaining);
            if (rc < 0)
            {
                if (errno == EINTR)
                {
                    continue;
                }

                lg2::error("Failed to write artifact: {ERROR}", "ERROR",
                           std::strerror(errno));
                std::filesystem::remove(
                    std::filesystem::path(templatePath.data()), ec);

                return {};
            }

            current += rc;
            remaining -= rc;
        }

        while (::fsync(fd.get()) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            lg2::error("Failed to sync artifact: {ERROR}", "ERROR",
                       std::strerror(errno));
            std::filesystem::remove(std::filesystem::path(templatePath.data()),
                                    ec);

            return {};
        }

        return ArtifactRef{.path = std::filesystem::path(templatePath.data())};
    }
    catch (const std::exception& e)
    {
        lg2::error("Failed to persist artifact: {ERROR}", "ERROR", e.what());
    }

    return {};
}

int openArtifact(const ArtifactRef& artifact)
{
    if (!artifact)
    {
        return -1;
    }

    const auto fd = ::open(artifact.path.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0)
    {
        lg2::error("Failed to open artifact {PATH}: {ERROR}", "PATH",
                   artifact.path.string(), "ERROR", std::strerror(errno));
    }

    return fd;
}

void removeArtifact(const ArtifactRef& artifact)
{
    if (!artifact)
    {
        return;
    }

    try
    {
        std::error_code ec;
        std::filesystem::remove(artifact.path, ec);
        if (ec)
        {
            lg2::error("Failed to remove artifact {PATH}: {ERROR}", "PATH",
                       artifact.path.string(), "ERROR", ec.message());
        }
    }
    catch (const std::exception& e)
    {
        lg2::error("Failed to remove artifact {PATH}: {ERROR}", "PATH",
                   artifact.path.string(), "ERROR", e.what());
    }
}

} // namespace phosphor::logging::event_extensions::utils
