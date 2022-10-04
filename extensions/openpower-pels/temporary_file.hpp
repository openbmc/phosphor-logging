#pragma once

#include <filesystem>
#include <utility>

namespace openpower
{
namespace pels
{
namespace util
{

namespace fs = std::filesystem;

/**
 * @class TemporaryFile
 *
 * A temporary file in the file system.
 *
 * The temporary file is created by the constructor.  The absolute path to the
 * file can be obtained using getPath().
 *
 * Note: Callers responsibility to delete the file after usage.
 *       The temporary file can be deleted by calling remove().
 *
 * TemporaryFile objects cannot be copied, but they can be moved.  This enables
 * them to be stored in containers like std::vector.
 */
class TemporaryFile
{
  public:
    // Specify which compiler-generated methods we want
    TemporaryFile(const TemporaryFile&) = delete;
    TemporaryFile& operator=(const TemporaryFile&) = delete;

    /**
     * Constructor.
     *
     * Creates a temporary file in the temporary directory (normally /tmp).
     *
     * Throws an exception if the file cannot be created.
     *
     *  @param data - data buffer
     *  @param len - length of the data buffer
     */
    TemporaryFile(const char* data, const uint32_t len);

    /**
     * Move constructor.
     *
     * Transfers ownership of a temporary file.
     *
     * @param file TemporaryFile object being moved
     */
    TemporaryFile(TemporaryFile&& file) : path{std::move(file.path)}
    {
        // Clear path in other object; after move path is in unspecified state
        file.path.clear();
    }

    /**
     * Move assignment operator.
     *
     * Deletes the temporary file owned by this object.  Then transfers
     * ownership of the temporary file owned by the other object.
     *
     * Throws an exception if an error occurs during the deletion.
     *
     * @param file TemporaryFile object being moved
     */
    TemporaryFile& operator=(TemporaryFile&& file);

    /**
     * Destructor.
     */
    ~TemporaryFile() {}

    /**
     * Deletes the temporary file.
     *
     * Does nothing if the file has already been deleted.
     *
     * Throws an exception if an error occurs during the deletion.
     */
    void remove();

    /**
     * Returns the absolute path to the temporary file.
     *
     * Returns an empty path if the file has been deleted.
     *
     * @return temporary file path
     */
    const fs::path& getPath() const
    {
        return path;
    }

    int getFd() const
    {
        return fd;
    }

  private:
    /**
     * Absolute path to the temporary file.
     */
    fs::path path{};

    /**
     * File descriptor of the temporary file.
     */
    int fd;
};

} // namespace util
} // namespace pels
} // namespace openpower
