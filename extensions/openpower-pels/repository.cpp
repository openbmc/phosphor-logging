/**
 * Copyright © 2019 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "repository.hpp"

#include <fstream>
#include <phosphor-logging/log.hpp>
#include <xyz/openbmc_project/Common/File/error.hpp>

namespace openpower
{
namespace pels
{

namespace fs = std::filesystem;
using namespace phosphor::logging;
namespace file_error = sdbusplus::xyz::openbmc_project::Common::File::Error;

Repository::Repository(const std::filesystem::path& basePath) :
    _logPath(basePath / "logs")
{
    if (!fs::exists(_logPath))
    {
        fs::create_directories(_logPath);
    }

    restore();
}

void Repository::restore()
{
    for (auto& dirEntry : fs::directory_iterator(_logPath))
    {
        try
        {
            if (!fs::is_regular_file(dirEntry.path()))
            {
                continue;
            }

            std::ifstream file{dirEntry.path()};
            std::vector<uint8_t> data{std::istreambuf_iterator<char>(file),
                                      std::istreambuf_iterator<char>()};
            file.close();

            PEL pel{data};
            if (pel.valid())
            {
                PELAttributes attributes{dirEntry.path(),
                                         pel.userHeader().actionFlags()};

                using pelID = LogID::Pel;
                using obmcID = LogID::Obmc;
                _pelAttributes.emplace(
                    LogID(pelID(pel.id()), obmcID(pel.obmcLogID())),
                    attributes);
            }
            else
            {
                log<level::ERR>(
                    "Found invalid PEL file while restoring.  Removing.",
                    entry("FILENAME=%s", dirEntry.path().c_str()));
                fs::remove(dirEntry.path());
            }
        }
        catch (std::exception& e)
        {
            log<level::ERR>("Hit exception while restoring PEL File",
                            entry("FILENAME=%s", dirEntry.path().c_str()),
                            entry("ERROR=%s", e.what()));
        }
    }
}

std::string Repository::getPELFilename(uint32_t pelID, const BCDTime& time)
{
    char name[50];
    sprintf(name, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X_%.8X", time.yearMSB,
            time.yearLSB, time.month, time.day, time.hour, time.minutes,
            time.seconds, time.hundredths, pelID);
    return std::string{name};
}

void Repository::add(std::unique_ptr<PEL>& pel)
{
    auto path = _logPath / getPELFilename(pel->id(), pel->commitTime());

    write(*(pel.get()), path);

    PELAttributes attributes{path, pel->userHeader().actionFlags()};

    using pelID = LogID::Pel;
    using obmcID = LogID::Obmc;
    _pelAttributes.emplace(LogID(pelID(pel->id()), obmcID(pel->obmcLogID())),
                           attributes);

    processAddCallbacks(*pel);
}

void Repository::write(const PEL& pel, const fs::path& path)
{
    std::ofstream file{path, std::ios::binary};

    if (!file.good())
    {
        // If this fails, the filesystem is probably full so it isn't like
        // we could successfully create yet another error log here.
        auto e = errno;
        fs::remove(path);
        log<level::ERR>("Unable to open PEL file for writing",
                        entry("ERRNO=%d", e), entry("PATH=%s", path.c_str()));
        throw file_error::Open();
    }

    auto data = pel.data();
    file.write(reinterpret_cast<const char*>(data.data()), data.size());

    if (file.fail())
    {
        // Same note as above about not being able to create an error log
        // for this case even if we wanted.
        auto e = errno;
        file.close();
        fs::remove(path);
        log<level::ERR>("Unable to write PEL file", entry("ERRNO=%d", e),
                        entry("PATH=%s", path.c_str()));
        throw file_error::Write();
    }
}

void Repository::remove(const LogID& id)
{
    auto pel = findPEL(id);
    if (pel != _pelAttributes.end())
    {
        fs::remove(pel->second.path);
        _pelAttributes.erase(pel);

        processDeleteCallbacks(id.pelID.id);
    }
}

std::optional<std::vector<uint8_t>> Repository::getPELData(const LogID& id)
{
    auto pel = findPEL(id);
    if (pel != _pelAttributes.end())
    {
        std::ifstream file{pel->second.path.c_str()};
        if (!file.good())
        {
            auto e = errno;
            log<level::ERR>("Unable to open PEL file", entry("ERRNO=%d", e),
                            entry("PATH=%s", pel->second.path.c_str()));
            throw file_error::Open();
        }

        std::vector<uint8_t> data{std::istreambuf_iterator<char>(file),
                                  std::istreambuf_iterator<char>()};
        return data;
    }

    return std::nullopt;
}

void Repository::for_each(ForEachFunc func) const
{
    for (const auto& [id, attributes] : _pelAttributes)
    {
        std::ifstream file{attributes.path};

        if (!file.good())
        {
            auto e = errno;
            log<level::ERR>("Repository::for_each: Unable to open PEL file",
                            entry("ERRNO=%d", e),
                            entry("PATH=%s", attributes.path.c_str()));
            continue;
        }

        std::vector<uint8_t> data{std::istreambuf_iterator<char>(file),
                                  std::istreambuf_iterator<char>()};
        file.close();

        PEL pel{data};

        try
        {
            if (func(pel))
            {
                break;
            }
        }
        catch (std::exception& e)
        {
            log<level::ERR>("Repository::for_each function exception",
                            entry("ERROR=%s", e.what()));
        }
    }
}

void Repository::processAddCallbacks(const PEL& pel) const
{
    for (auto& [name, func] : _addSubscriptions)
    {
        try
        {
            func(pel);
        }
        catch (std::exception& e)
        {
            log<level::ERR>("PEL Repository add callback exception",
                            entry("NAME=%s", name.c_str()),
                            entry("ERROR=%s", e.what()));
        }
    }
}

void Repository::processDeleteCallbacks(uint32_t id) const
{
    for (auto& [name, func] : _deleteSubscriptions)
    {
        try
        {
            func(id);
        }
        catch (std::exception& e)
        {
            log<level::ERR>("PEL Repository delete callback exception",
                            entry("NAME=%s", name.c_str()),
                            entry("ERROR=%s", e.what()));
        }
    }
}

std::optional<std::reference_wrapper<const Repository::PELAttributes>>
    Repository::getPELAttributes(const LogID& id) const
{
    auto pel = findPEL(id);
    if (pel != _pelAttributes.end())
    {
        return pel->second;
    }

    return std::nullopt;
}

} // namespace pels
} // namespace openpower
