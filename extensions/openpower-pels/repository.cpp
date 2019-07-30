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

            PEL pel(std::move(data));
            if (pel.valid())
            {
                using pelID = LogID::Pel;
                using obmcID = LogID::Obmc;
                _idsToPELs.emplace(
                    LogID(pelID(pel.id()), obmcID(pel.obmcLogID())),
                    dirEntry.path());
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
    std::ofstream file{path, std::ios::binary};

    if (!file.good())
    {
        // If this fails, the filesystem is probably full so it isn't like
        // we could successfully create yet another error log here.
        auto e = errno;
        log<level::ERR>("Failed creating PEL file",
                        entry("FILE=%s", path.c_str()));
        fs::remove(path);
        log<level::ERR>("Unable to open PEL file for writing",
                        entry("ERRNO=%d", e), entry("PATH=%s", path.c_str()));
        throw file_error::Open();
    }

    auto data = pel->data();
    file.write(reinterpret_cast<const char*>(data.data()), data.size());

    if (file.fail())
    {
        // Same note as above about not being able to create an error log
        // for this case even if we wanted.
        auto e = errno;
        log<level::ERR>("Failed writing PEL file",
                        entry("FILE=%s", path.c_str()));
        file.close();
        fs::remove(path);
        log<level::ERR>("Unable to write PEL file", entry("ERRNO=%d", e),
                        entry("PATH=%s", path.c_str()));
        throw file_error::Write();
    }

    using pelID = LogID::Pel;
    using obmcID = LogID::Obmc;
    _idsToPELs.emplace(LogID(pelID(pel->id()), obmcID(pel->obmcLogID())), path);
}

void Repository::remove(const LogID& id)
{
    auto pel = findPEL(id);
    if (pel != _idsToPELs.end())
    {
        fs::remove(pel->second);
        _idsToPELs.erase(pel);
    }
}

std::optional<std::vector<uint8_t>> Repository::getPELData(const LogID& id)
{
    auto pel = findPEL(id);
    if (pel != _idsToPELs.end())
    {
        std::ifstream file{pel->second.c_str()};
        if (!file.good())
        {
            auto e = errno;
            log<level::ERR>("Unable to open PEL file", entry("ERRNO=%d", e),
                            entry("PATH=%s", pel->second.c_str()));
            throw file_error::Open();
        }

        std::vector<uint8_t> data{std::istreambuf_iterator<char>(file),
                                  std::istreambuf_iterator<char>()};
        return data;
    }

    return std::nullopt;
}

} // namespace pels
} // namespace openpower
