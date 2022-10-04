#include "temporary_file.hpp"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdexcept>

namespace openpower
{
namespace pels
{
namespace util
{

TemporaryFile::TemporaryFile(const char* data, const uint32_t len)
{
    // Build template path required by mkstemp()
    std::string templatePath = fs::temp_directory_path() /
                               "phosphor-logging-XXXXXX";

    // Generate unique file name, create file, and open it.  The XXXXXX
    // characters are replaced by mkstemp() to make the file name unique.
    fd = mkostemp(templatePath.data(), O_RDWR);
    if (fd == -1)
    {
        throw std::runtime_error{
            std::string{"Unable to create temporary file: "} + strerror(errno)};
    }

    // Update file with input Buffer data
    auto rc = write(fd, data, len);
    if (rc == -1)
    {
        // Delete temporary file.  The destructor won't be called because the
        // exception below causes this constructor to exit without completing.
        remove();
        throw std::runtime_error{std::string{"Unable to update file: "} +
                                 strerror(errno)};
    }

    // Store path to temporary file
    path = templatePath;
}

TemporaryFile& TemporaryFile::operator=(TemporaryFile&& file)
{
    // Verify not assigning object to itself (a = std::move(a))
    if (this != &file)
    {
        // Delete temporary file owned by this object
        remove();

        // Move temporary file path from other object, transferring ownership
        path = std::move(file.path);

        // Clear path in other object; after move path is in unspecified state
        file.path.clear();
    }
    return *this;
}

void TemporaryFile::remove()
{
    if (!path.empty())
    {
        // Delete temporary file from file system
        fs::remove(path);

        // Clear path to indicate file has been deleted
        path.clear();
    }
}

} // namespace util
} // namespace pels
} // namespace openpower
