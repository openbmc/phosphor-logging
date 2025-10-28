// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "log_id.hpp"

#include "paths.hpp"

#include <phosphor-logging/lg2.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>

namespace openpower
{
namespace pels
{

namespace fs = std::filesystem;

constexpr uint32_t startingLogID = 1;
constexpr uint32_t bmcLogIDPrefix = 0x50000000;

namespace detail
{

uint32_t addLogIDPrefix(uint32_t id)
{
    // If redundant BMCs are ever a thing, may need a different prefix.
    return (id & 0x00FFFFFF) | bmcLogIDPrefix;
}

uint32_t getTimeBasedLogID()
{
    using namespace std::chrono;

    // Use 3 bytes of the nanosecond count since the epoch.
    uint32_t id =
        duration_cast<nanoseconds>(system_clock::now().time_since_epoch())
            .count();

    return addLogIDPrefix(id);
}

} // namespace detail

uint32_t generatePELID()
{
    // Note: there isn't a need to be thread safe.

    static std::string idFilename;
    if (idFilename.empty())
    {
        idFilename = getPELIDFile();
        checkFileForZeroData(idFilename);
    }

    uint32_t id = 0;

    if (!fs::exists(idFilename))
    {
        auto path = fs::path(idFilename).parent_path();
        if (!fs::exists(path))
        {
            fs::create_directories(path);
        }

        id = startingLogID;
    }
    else
    {
        std::ifstream idFile{idFilename};
        idFile >> id;
        if (idFile.fail())
        {
            // Just make up an ID
            lg2::error("Unable to read PEL ID File!");
            return detail::getTimeBasedLogID();
        }
    }

    // Wrapping shouldn't be a problem, but check anyway
    if (id == 0x00FFFFFF)
    {
        id = startingLogID;
    }

    std::ofstream idFile{idFilename};
    idFile << (id + 1);
    if (idFile.fail())
    {
        // Just make up an ID so we don't reuse one next time
        lg2::error("Unable to write PEL ID File!");
        return detail::getTimeBasedLogID();
    }

    return detail::addLogIDPrefix(id);
}

void checkFileForZeroData(const std::string& filename)
{
    if (fs::exists(filename))
    {
        char ch;

        std::ifstream rf{filename, std::ios::binary};
        rf.read(&ch, sizeof(ch));
        while (ch == '\0')
        {
            if (rf.eof())
            {
                fs::remove(filename);
                lg2::warning("PEL ID file seems corrupted. Deleting it.");
                break;
            }
            rf.read(&ch, sizeof(ch));
        }
    }
}
} // namespace pels
} // namespace openpower
