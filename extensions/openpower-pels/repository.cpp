// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "repository.hpp"

#include "pel_values.hpp"

#include <fcntl.h>
#include <sys/stat.h>

#include <phosphor-logging/lg2.hpp>
#include <xyz/openbmc_project/Common/File/error.hpp>

#include <fstream>

namespace openpower
{
namespace pels
{

namespace fs = std::filesystem;
namespace file_error = sdbusplus::xyz::openbmc_project::Common::File::Error;

constexpr size_t warningPercentage = 95;

/**
 * @brief Returns the amount of space the file uses on disk.
 *
 * This is different than just the regular size of the file.
 *
 * @param[in] file - The file to get the size of
 *
 * @return size_t The disk space the file uses
 */
size_t getFileDiskSize(const std::filesystem::path& file)
{
    constexpr size_t statBlockSize = 512;
    struct stat statData;
    auto rc = stat(file.c_str(), &statData);
    if (rc != 0)
    {
        auto e = errno;
        lg2::error("Call to stat() failed on {FILE} with errno {ERRNO}", "FILE",
                   file.native(), "ERRNO", e);
        abort();
    }

    return statData.st_blocks * statBlockSize;
}

Repository::Repository(const std::filesystem::path& basePath, size_t repoSize,
                       size_t maxNumPELs) :
    _logPath(basePath / "logs"), _maxRepoSize(repoSize),
    _maxNumPELs(maxNumPELs), _archivePath(basePath / "logs" / "archive")
{
    if (!fs::exists(_logPath))
    {
        fs::create_directories(_logPath);
    }

    if (!fs::exists(_archivePath))
    {
        fs::create_directories(_archivePath);
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
                // If the host hasn't acked it, reset the host state so
                // it will get sent up again.
                if (pel.hostTransmissionState() == TransmissionState::sent)
                {
                    pel.setHostTransmissionState(TransmissionState::newPEL);
                    try
                    {
                        write(pel, dirEntry.path());
                    }
                    catch (const std::exception& e)
                    {
                        lg2::error(
                            "Failed to save PEL after updating host state, PEL ID = {ID}",
                            "ID", lg2::hex, pel.id());
                    }
                }

                PELAttributes attributes{
                    dirEntry.path(),
                    getFileDiskSize(dirEntry.path()),
                    pel.privateHeader().creatorID(),
                    pel.userHeader().subsystem(),
                    pel.userHeader().severity(),
                    pel.userHeader().actionFlags(),
                    pel.hostTransmissionState(),
                    pel.hmcTransmissionState(),
                    pel.plid(),
                    pel.getDeconfigFlag(),
                    pel.getGuardFlag(),
                    getMillisecondsSinceEpoch(
                        pel.privateHeader().createTimestamp())};

                using pelID = LogID::Pel;
                using obmcID = LogID::Obmc;
                _pelAttributes.emplace(
                    LogID(pelID(pel.id()), obmcID(pel.obmcLogID())),
                    attributes);

                updateRepoStats(attributes, true);
            }
            else
            {
                lg2::error(
                    "Found invalid PEL file {FILE} while restoring.  Removing.",
                    "FILE", dirEntry.path());
                fs::remove(dirEntry.path());
            }
        }
        catch (const std::exception& e)
        {
            lg2::error("Hit exception while restoring PEL file {FILE}: {ERROR}",
                       "FILE", dirEntry.path(), "ERROR", e);
        }
    }

    // Get size of archive folder
    for (auto& dirEntry : fs::directory_iterator(_archivePath))
    {
        _archiveSize += getFileDiskSize(dirEntry);
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
    pel->setHostTransmissionState(TransmissionState::newPEL);
    pel->setHMCTransmissionState(TransmissionState::newPEL);

    auto path = _logPath / getPELFilename(pel->id(), pel->commitTime());

    write(*(pel.get()), path);

    PELAttributes attributes{
        path,
        getFileDiskSize(path),
        pel->privateHeader().creatorID(),
        pel->userHeader().subsystem(),
        pel->userHeader().severity(),
        pel->userHeader().actionFlags(),
        pel->hostTransmissionState(),
        pel->hmcTransmissionState(),
        pel->plid(),
        pel->getDeconfigFlag(),
        pel->getGuardFlag(),
        getMillisecondsSinceEpoch(pel->privateHeader().createTimestamp())};

    using pelID = LogID::Pel;
    using obmcID = LogID::Obmc;
    _pelAttributes.emplace(LogID(pelID(pel->id()), obmcID(pel->obmcLogID())),
                           attributes);

    _lastPelID = pel->id();

    updateRepoStats(attributes, true);

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
        lg2::error(
            "Unable to open PEL file {FILE} for writing, errno = {ERRNO}",
            "FILE", path, "ERRNO", e);
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
        lg2::error("Unable to write PEL file {FILE}, errno = {ERRNO}", "FILE",
                   path, "ERRNO", e);
        throw file_error::Write();
    }
}

std::optional<Repository::LogID> Repository::remove(const LogID& id)
{
    auto pel = findPEL(id);
    if (pel == _pelAttributes.end())
    {
        return std::nullopt;
    }

    LogID actualID = pel->first;
    updateRepoStats(pel->second, false);

    lg2::debug(
        "Removing PEL from repository, PEL ID = {PEL_ID}, BMC log ID = {BMC_ID}",
        "PEL_ID", lg2::hex, actualID.pelID.id, "BMC_ID", actualID.obmcID.id);

    if (fs::exists(pel->second.path))
    {
        // Check for existense of new archive folder
        if (!fs::exists(_archivePath))
        {
            fs::create_directories(_archivePath);
        }

        // Move log file to archive folder
        auto fileName = _archivePath / pel->second.path.filename();
        fs::rename(pel->second.path, fileName);

        // Update size of file
        _archiveSize += getFileDiskSize(fileName);
    }

    _pelAttributes.erase(pel);

    processDeleteCallbacks(actualID.pelID.id);

    return actualID;
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
            lg2::error("Unable to open PEL file {FILE}, errno = {ERRNO}",
                       "FILE", pel->second.path, "ERRNO", e);
            throw file_error::Open();
        }

        std::vector<uint8_t> data{std::istreambuf_iterator<char>(file),
                                  std::istreambuf_iterator<char>()};
        return data;
    }

    return std::nullopt;
}

std::optional<sdbusplus::message::unix_fd> Repository::getPELFD(const LogID& id)
{
    auto pel = findPEL(id);
    if (pel != _pelAttributes.end())
    {
        int fd = open(pel->second.path.c_str(), O_RDONLY | O_NONBLOCK);
        if (fd == -1)
        {
            auto e = errno;
            lg2::error("Unable to open PEL file {FILE}, errno = {ERRNO}",
                       "FILE", pel->second.path, "ERRNO", e);
            throw file_error::Open();
        }

        // Must leave the file open here.  It will be closed by sdbusplus
        // when it sends it back over D-Bus.
        return fd;
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
            lg2::error(
                "Repository::for_each: Unable to open PEL file {FILE}, errno = {ERRNO}",
                "FILE", attributes.path, "ERRNO", e);
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
        catch (const std::exception& e)
        {
            lg2::error("Repository::for_each function exception: {ERROR}",
                       "ERROR", e);
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
        catch (const std::exception& e)
        {
            lg2::error(
                "PEL Repository add callback exception. Name = {NAME}, Error = {ERROR}",
                "NAME", name, "ERROR", e);
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
        catch (const std::exception& e)
        {
            lg2::error(
                "PEL Repository delete callback exception. Name = {NAME}, Error = {ERROR}",
                "NAME", name, "ERROR", e);
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

void Repository::setPELHostTransState(uint32_t pelID, TransmissionState state)
{
    LogID id{LogID::Pel{pelID}};
    auto attr = std::find_if(_pelAttributes.begin(), _pelAttributes.end(),
                             [&id](const auto& a) { return a.first == id; });

    if ((attr != _pelAttributes.end()) && (attr->second.hostState != state))
    {
        PELUpdateFunc func = [state](PEL& pel) {
            pel.setHostTransmissionState(state);
            return true;
        };

        try
        {
            updatePEL(attr->second.path, func);
        }
        catch (const std::exception& e)
        {
            lg2::error(
                "Unable to update PEL host transmission state. Path = {PATH}, Error = {ERROR}",
                "PATH", attr->second.path, "ERROR", e);
        }
    }
}

void Repository::setPELHMCTransState(uint32_t pelID, TransmissionState state)
{
    LogID id{LogID::Pel{pelID}};
    auto attr = std::find_if(_pelAttributes.begin(), _pelAttributes.end(),
                             [&id](const auto& a) { return a.first == id; });

    if ((attr != _pelAttributes.end()) && (attr->second.hmcState != state))
    {
        PELUpdateFunc func = [state](PEL& pel) {
            pel.setHMCTransmissionState(state);
            return true;
        };

        try
        {
            updatePEL(attr->second.path, func);
        }
        catch (const std::exception& e)
        {
            lg2::error(
                "Unable to update PEL HMC transmission state. Path = {PATH}, Error = {ERROR}",
                "PATH", attr->second.path, "ERROR", e);
        }
    }
}

bool Repository::updatePEL(const fs::path& path, PELUpdateFunc updateFunc)
{
    std::ifstream file{path};
    std::vector<uint8_t> data{std::istreambuf_iterator<char>(file),
                              std::istreambuf_iterator<char>()};
    file.close();

    PEL pel{data};

    if (pel.valid())
    {
        if (updateFunc(pel))
        {
            // Three attribute fields can change post creation from
            // an updatePEL call:
            //  - hmcTransmissionState - When HMC acks a PEL
            //  - hostTransmissionState - When host acks a PEL
            //  - deconfig flag - Can be cleared for PELs that call out
            //                    hotplugged FRUs.
            // Make sure they're up to date.
            LogID id{LogID::Pel(pel.id())};
            auto attr =
                std::find_if(_pelAttributes.begin(), _pelAttributes.end(),
                             [&id](const auto& a) { return a.first == id; });
            if (attr != _pelAttributes.end())
            {
                attr->second.hmcState = pel.hmcTransmissionState();
                attr->second.hostState = pel.hostTransmissionState();
                attr->second.deconfig = pel.getDeconfigFlag();
            }

            write(pel, path);
            return true;
        }
    }
    else
    {
        throw std::runtime_error(
            "Unable to read a valid PEL when trying to update it");
    }
    return false;
}

bool Repository::isServiceableSev(const PELAttributes& pel)
{
    auto sevType = static_cast<SeverityType>(pel.severity & 0xF0);
    auto sevPVEntry =
        pel_values::findByValue(pel.severity, pel_values::severityValues);
    std::string sevName = std::get<pel_values::registryNamePos>(*sevPVEntry);

    bool check1 = (sevType == SeverityType::predictive) ||
                  (sevType == SeverityType::unrecoverable) ||
                  (sevType == SeverityType::critical);

    bool check2 = ((sevType == SeverityType::recovered) ||
                   (sevName == "symptom_recovered")) &&
                  !pel.actionFlags.test(hiddenFlagBit);

    bool check3 = (sevName == "symptom_predictive") ||
                  (sevName == "symptom_unrecoverable") ||
                  (sevName == "symptom_critical");

    return check1 || check2 || check3;
}

void Repository::updateRepoStats(const PELAttributes& pel, bool pelAdded)
{
    auto isServiceable = Repository::isServiceableSev(pel);
    auto bmcPEL = CreatorID::openBMC == static_cast<CreatorID>(pel.creator);

    auto adjustSize = [pelAdded, &pel](auto& runningSize) {
        if (pelAdded)
        {
            runningSize += pel.sizeOnDisk;
        }
        else
        {
            runningSize = std::max(static_cast<int64_t>(runningSize) -
                                       static_cast<int64_t>(pel.sizeOnDisk),
                                   static_cast<int64_t>(0));
        }
    };

    adjustSize(_sizes.total);

    if (bmcPEL)
    {
        adjustSize(_sizes.bmc);
        if (isServiceable)
        {
            adjustSize(_sizes.bmcServiceable);
        }
        else
        {
            adjustSize(_sizes.bmcInfo);
        }
    }
    else
    {
        adjustSize(_sizes.nonBMC);
        if (isServiceable)
        {
            adjustSize(_sizes.nonBMCServiceable);
        }
        else
        {
            adjustSize(_sizes.nonBMCInfo);
        }
    }
}

bool Repository::sizeWarning()
{
    std::error_code ec;

    if ((_archiveSize > 0) && ((_sizes.total + _archiveSize) >
                               ((_maxRepoSize * warningPercentage) / 100)))
    {
        lg2::info(
            "Repository::sizeWarning function:Deleting the files in archive");

        for (const auto& dirEntry : fs::directory_iterator(_archivePath))
        {
            fs::remove(dirEntry.path(), ec);
            if (ec)
            {
                lg2::info("Repository::sizeWarning: Could not delete "
                          "file {FILE} in PEL archive",
                          "FILE", dirEntry.path());
            }
        }

        _archiveSize = 0;
    }

    return (_sizes.total > (_maxRepoSize * warningPercentage / 100)) ||
           (_pelAttributes.size() > _maxNumPELs);
}

std::vector<Repository::AttributesReference> Repository::getAllPELAttributes(
    SortOrder order) const
{
    std::vector<Repository::AttributesReference> attributes;

    std::for_each(_pelAttributes.begin(), _pelAttributes.end(),
                  [&attributes](auto& pelEntry) {
                      attributes.push_back(pelEntry);
                  });

    std::sort(attributes.begin(), attributes.end(),
              [order](const auto& left, const auto& right) {
                  if (order == SortOrder::ascending)
                  {
                      return left.get().second.path < right.get().second.path;
                  }
                  return left.get().second.path > right.get().second.path;
              });

    return attributes;
}

std::vector<uint32_t> Repository::prune(
    const std::vector<uint32_t>& idsWithHwIsoEntry)
{
    std::vector<uint32_t> obmcLogIDs;
    lg2::info("Pruning PEL repository that takes up {TOTAL} bytes and has "
              "{NUM_PELS} PELs",
              "TOTAL", _sizes.total, "NUM_PELS", _pelAttributes.size());

    // Set up the 5 functions to check if the PEL category
    // is still over its limits.

    // BMC informational PELs should only take up 15%
    IsOverLimitFunc overBMCInfoLimit = [this]() {
        return _sizes.bmcInfo > _maxRepoSize * 15 / 100;
    };

    // BMC non informational PELs should only take up 30%
    IsOverLimitFunc overBMCNonInfoLimit = [this]() {
        return _sizes.bmcServiceable > _maxRepoSize * 30 / 100;
    };

    // Non BMC informational PELs should only take up 15%
    IsOverLimitFunc overNonBMCInfoLimit = [this]() {
        return _sizes.nonBMCInfo > _maxRepoSize * 15 / 100;
    };

    // Non BMC non informational PELs should only take up 15%
    IsOverLimitFunc overNonBMCNonInfoLimit = [this]() {
        return _sizes.nonBMCServiceable > _maxRepoSize * 30 / 100;
    };

    // Bring the total number of PELs down to 80% of the max
    IsOverLimitFunc tooManyPELsLimit = [this]() {
        return _pelAttributes.size() > _maxNumPELs * 80 / 100;
    };

    // Set up the functions to determine which category a PEL is in.
    // TODO: Return false in these functions if a PEL caused a guard record.

    // A BMC informational PEL
    IsPELTypeFunc isBMCInfo = [](const PELAttributes& pel) {
        return (CreatorID::openBMC == static_cast<CreatorID>(pel.creator)) &&
               !Repository::isServiceableSev(pel);
    };

    // A BMC non informational PEL
    IsPELTypeFunc isBMCNonInfo = [](const PELAttributes& pel) {
        return (CreatorID::openBMC == static_cast<CreatorID>(pel.creator)) &&
               Repository::isServiceableSev(pel);
    };

    // A non BMC informational PEL
    IsPELTypeFunc isNonBMCInfo = [](const PELAttributes& pel) {
        return (CreatorID::openBMC != static_cast<CreatorID>(pel.creator)) &&
               !Repository::isServiceableSev(pel);
    };

    // A non BMC non informational PEL
    IsPELTypeFunc isNonBMCNonInfo = [](const PELAttributes& pel) {
        return (CreatorID::openBMC != static_cast<CreatorID>(pel.creator)) &&
               Repository::isServiceableSev(pel);
    };

    // When counting PELs, count every PEL
    IsPELTypeFunc isAnyPEL = [](const PELAttributes& /*pel*/) { return true; };

    // Check all 4 categories, which will result in at most 90%
    // usage (15 + 30 + 15 + 30).
    removePELs(overBMCInfoLimit, isBMCInfo, idsWithHwIsoEntry, obmcLogIDs);
    removePELs(overBMCNonInfoLimit, isBMCNonInfo, idsWithHwIsoEntry,
               obmcLogIDs);
    removePELs(overNonBMCInfoLimit, isNonBMCInfo, idsWithHwIsoEntry,
               obmcLogIDs);
    removePELs(overNonBMCNonInfoLimit, isNonBMCNonInfo, idsWithHwIsoEntry,
               obmcLogIDs);

    // After the above pruning check if there are still too many PELs,
    // which can happen depending on PEL sizes.
    if (_pelAttributes.size() > _maxNumPELs)
    {
        removePELs(tooManyPELsLimit, isAnyPEL, idsWithHwIsoEntry, obmcLogIDs);
    }

    if (!obmcLogIDs.empty())
    {
        lg2::info("Number of PELs removed to save space: {NUM_PELS}",
                  "NUM_PELS", obmcLogIDs.size());
    }

    return obmcLogIDs;
}

void Repository::removePELs(const IsOverLimitFunc& isOverLimit,
                            const IsPELTypeFunc& isPELType,
                            const std::vector<uint32_t>& idsWithHwIsoEntry,
                            std::vector<uint32_t>& removedBMCLogIDs)
{
    if (!isOverLimit())
    {
        return;
    }

    auto attributes = getAllPELAttributes(SortOrder::ascending);

    // Make 4 passes on the PELs, stopping as soon as isOverLimit
    // returns false.
    //   Pass 1: only delete HMC acked PELs
    //   Pass 2: only delete OS acked PELs
    //   Pass 3: only delete PHYP sent PELs
    //   Pass 4: delete all PELs
    static const std::vector<std::function<bool(const PELAttributes& pel)>>
        stateChecks{[](const auto& pel) {
                        return pel.hmcState == TransmissionState::acked;
                    },

                    [](const auto& pel) {
                        return pel.hostState == TransmissionState::acked;
                    },

                    [](const auto& pel) {
                        return pel.hostState == TransmissionState::sent;
                    },

                    [](const auto& /*pel*/) { return true; }};

    for (const auto& stateCheck : stateChecks)
    {
        for (auto it = attributes.begin(); it != attributes.end();)
        {
            const auto& pel = it->get();
            if (isPELType(pel.second) && stateCheck(pel.second))
            {
                auto removedID = pel.first.obmcID.id;

                auto idFound = std::find(idsWithHwIsoEntry.begin(),
                                         idsWithHwIsoEntry.end(), removedID);
                if (idFound != idsWithHwIsoEntry.end())
                {
                    ++it;
                    continue;
                }

                remove(pel.first);

                removedBMCLogIDs.push_back(removedID);

                attributes.erase(it);

                if (!isOverLimit())
                {
                    break;
                }
            }
            else
            {
                ++it;
            }
        }

        if (!isOverLimit())
        {
            break;
        }
    }
}

void Repository::archivePEL(const PEL& pel)
{
    if (pel.valid())
    {
        auto path = _archivePath / getPELFilename(pel.id(), pel.commitTime());

        write(pel, path);

        _archiveSize += getFileDiskSize(path);
    }
}

} // namespace pels
} // namespace openpower
