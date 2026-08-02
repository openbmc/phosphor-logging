#include "plugin/cper_artifact.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <phosphor-logging/lg2.hpp>

#include <array>
#include <cerrno>
#include <cstring>
#include <filesystem>

namespace phosphor::logging::plugin::cper
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

    int get() const
    {
        return fd;
    }

    explicit operator bool() const
    {
        return fd >= 0;
    }

  private:
    void reset()
    {
        if (fd >= 0)
        {
            ::close(fd);
            fd = -1;
        }
    }

    int fd{-1};
};

bool copyArtifact(int sourceFd, int destinationFd)
{
    std::array<std::byte, artifactCopyBufferSize> buffer{};

    ssize_t bytesRead = 0;

    while ((bytesRead = ::read(sourceFd, buffer.data(), buffer.size())) > 0)
    {
        ssize_t remaining = bytesRead;

        const auto* data = reinterpret_cast<const char*>(buffer.data());

        while (remaining > 0)
        {
            const auto bytesWritten = ::write(destinationFd, data, remaining);

            if (bytesWritten <= 0)
            {
                lg2::error("Failed to write CPER artifact: {ERROR}", "ERROR",
                           std::strerror(errno));
                return false;
            }

            remaining -= bytesWritten;
            data += bytesWritten;
        }
    }

    if (bytesRead < 0)
    {
        lg2::error("Failed to read CPER artifact: {ERROR}", "ERROR",
                   std::strerror(errno));
        return false;
    }

    return true;
}

} // namespace

std::filesystem::path artifactPath(const std::string& objectPath,
                                   const std::filesystem::path& root)
{
    const auto separatorPos = objectPath.find_last_of('/');

    const auto entryId = objectPath.substr(
        separatorPos == std::string::npos ? 0 : separatorPos + 1);

    return root / (entryId + ".bin");
}

std::filesystem::path persistArtifact(const std::string& objectPath, int fd,
                                      const std::filesystem::path& root)
{
    /*
     * CPER artifact is optional.
     */
    if (fd < 0)
    {
        return {};
    }

    const auto path = artifactPath(objectPath, root);

    std::error_code ec;
    std::filesystem::create_directories(path.parent_path(), ec);

    if (ec)
    {
        lg2::error("Failed to create artifact directory {DIR}: {ERROR}", "DIR",
                   path.parent_path().string(), "ERROR", ec.message());
        return {};
    }

    FileDescriptor outputFd(
        ::open(path.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644));

    if (!outputFd)
    {
        lg2::error("Failed to create CPER artifact {PATH}: {ERROR}", "PATH",
                   path.string(), "ERROR", std::strerror(errno));
        return {};
    }

    if (::lseek(fd, 0, SEEK_SET) < 0)
    {
        lg2::error("Failed to rewind CPER artifact fd={FD}: {ERROR}", "FD", fd,
                   "ERROR", std::strerror(errno));

        std::filesystem::remove(path, ec);
        return {};
    }

    if (!copyArtifact(fd, outputFd.get()))
    {
        std::filesystem::remove(path, ec);
        return {};
    }

    return path;
}

} // namespace phosphor::logging::plugin::cper
