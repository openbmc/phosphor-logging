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
#include "manager.hpp"

#include "additional_data.hpp"
#include "json_utils.hpp"
#include "pel.hpp"
#include "pel_entry.hpp"
#include "service_indicators.hpp"
#include "severity.hpp"

#include <fmt/format.h>
#include <sys/inotify.h>
#include <unistd.h>

#include <xyz/openbmc_project/Common/error.hpp>
#include <xyz/openbmc_project/Logging/Create/server.hpp>

#include <filesystem>
#include <fstream>
#include <locale>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;
namespace fs = std::filesystem;
namespace rg = openpower::pels::message;

namespace common_error = sdbusplus::xyz::openbmc_project::Common::Error;

using Create = sdbusplus::xyz::openbmc_project::Logging::server::Create;

namespace additional_data
{
constexpr auto rawPEL = "RAWPEL";
constexpr auto esel = "ESEL";
constexpr auto error = "ERROR_NAME";
} // namespace additional_data

constexpr auto defaultLogMessage = "xyz.openbmc_project.Logging.Error.Default";

Manager::~Manager()
{
    if (_pelFileDeleteFD != -1)
    {
        if (_pelFileDeleteWatchFD != -1)
        {
            inotify_rm_watch(_pelFileDeleteFD, _pelFileDeleteWatchFD);
        }
        close(_pelFileDeleteFD);
    }
}

void Manager::create(const std::string& message, uint32_t obmcLogID,
                     uint64_t timestamp, Entry::Level severity,
                     const std::vector<std::string>& additionalData,
                     const std::vector<std::string>& associations,
                     const FFDCEntries& ffdc)
{
    AdditionalData ad{additionalData};

    // If a PEL was passed in via a filename or in an ESEL,
    // use that.  Otherwise, create one.
    auto rawPelPath = ad.getValue(additional_data::rawPEL);
    if (rawPelPath)
    {
        addRawPEL(*rawPelPath, obmcLogID);
    }
    else
    {
        auto esel = ad.getValue(additional_data::esel);
        if (esel)
        {
            addESELPEL(*esel, obmcLogID);
        }
        else
        {
            createPEL(message, obmcLogID, timestamp, severity, additionalData,
                      associations, ffdc);
        }
    }

    setEntryPath(obmcLogID);
    setServiceProviderNotifyFlag(obmcLogID);
}

void Manager::addRawPEL(const std::string& rawPelPath, uint32_t obmcLogID)
{
    if (fs::exists(rawPelPath))
    {
        std::ifstream file(rawPelPath, std::ios::in | std::ios::binary);

        auto data = std::vector<uint8_t>(std::istreambuf_iterator<char>(file),
                                         std::istreambuf_iterator<char>());
        if (file.fail())
        {
            log<level::ERR>("Filesystem error reading a raw PEL",
                            entry("PELFILE=%s", rawPelPath.c_str()),
                            entry("OBMCLOGID=%d", obmcLogID));
            // TODO, Decide what to do here. Maybe nothing.
            return;
        }

        file.close();

        addPEL(data, obmcLogID);

        std::error_code ec;
        fs::remove(rawPelPath, ec);
    }
    else
    {
        log<level::ERR>("Raw PEL file from BMC event log does not exist",
                        entry("PELFILE=%s", (rawPelPath).c_str()),
                        entry("OBMCLOGID=%d", obmcLogID));
    }
}

void Manager::addPEL(std::vector<uint8_t>& pelData, uint32_t obmcLogID)
{
    auto pel = std::make_unique<openpower::pels::PEL>(pelData, obmcLogID);
    if (pel->valid())
    {
        // PELs created by others still need this field set by us.
        pel->setCommitTime();

        // Assign Id other than to Hostbot PEL
        if ((pel->privateHeader()).creatorID() !=
            static_cast<uint8_t>(CreatorID::hostboot))
        {
            pel->assignID();
        }
        else
        {
            const Repository::LogID id{Repository::LogID::Pel(pel->id())};
            auto result = _repo.hasPEL(id);
            if (result)
            {
                log<level::WARNING>(
                    fmt::format("Duplicate HostBoot PEL Id {:#X} found; "
                                "moving it to archive folder",
                                pel->id())
                        .c_str());

                _repo.archivePEL(*pel);

                // No need to keep around the openBMC event log entry
                scheduleObmcLogDelete(obmcLogID);
                return;
            }
        }

        // Update System Info to Extended User Data
        pel->updateSysInfoInExtendedUserDataSection(*_dataIface);

        // Check for severity 0x51 and update boot progress SRC
        updateProgressSRC(pel);

        try
        {
            log<level::DEBUG>(
                fmt::format("Adding external PEL {:#x} (BMC ID {}) to repo",
                            pel->id(), obmcLogID)
                    .c_str());

            _repo.add(pel);

            if (_repo.sizeWarning())
            {
                scheduleRepoPrune();
            }

            // Activate any resulting service indicators if necessary
            auto policy = service_indicators::getPolicy(*_dataIface);
            policy->activate(*pel);
        }
        catch (const std::exception& e)
        {
            // Probably a full or r/o filesystem, not much we can do.
            log<level::ERR>("Unable to add PEL to Repository",
                            entry("PEL_ID=0x%X", pel->id()));
        }

        updateEventId(pel);
        updateResolution(*pel);
        createPELEntry(obmcLogID);

        // Check if firmware should quiesce system due to error
        checkPelAndQuiesce(pel);
    }
    else
    {
        log<level::ERR>("Invalid PEL received from the host",
                        entry("OBMCLOGID=%d", obmcLogID));

        AdditionalData ad;
        ad.add("PLID", getNumberString("0x%08X", pel->plid()));
        ad.add("OBMC_LOG_ID", std::to_string(obmcLogID));
        ad.add("PEL_SIZE", std::to_string(pelData.size()));

        std::string asciiString;
        auto src = pel->primarySRC();
        if (src)
        {
            asciiString = (*src)->asciiString();
        }

        ad.add("SRC", asciiString);

        _eventLogger.log("org.open_power.Logging.Error.BadHostPEL",
                         Entry::Level::Error, ad);

        // Save it to a file for debug in the lab.  Just keep the latest.
        // Not adding it to the PEL because it could already be max size
        // and don't want to truncate an already invalid PEL.
        std::ofstream pelFile{getPELRepoPath() / "badPEL"};
        pelFile.write(reinterpret_cast<const char*>(pelData.data()),
                      pelData.size());

        // No need to keep around the openBMC event log entry
        scheduleObmcLogDelete(obmcLogID);
    }
}

void Manager::addESELPEL(const std::string& esel, uint32_t obmcLogID)
{
    std::vector<uint8_t> data;

    log<level::DEBUG>("Adding PEL from ESEL",
                      entry("OBMC_LOG_ID=%d", obmcLogID));

    try
    {
        data = std::move(eselToRawData(esel));
    }
    catch (const std::exception& e)
    {
        // Try to add it below anyway, so it follows the usual bad data path.
        log<level::ERR>("Problems converting ESEL string to a byte vector");
    }

    addPEL(data, obmcLogID);
}

std::vector<uint8_t> Manager::eselToRawData(const std::string& esel)
{
    std::vector<uint8_t> data;
    std::string byteString;

    // As the eSEL string looks like: "50 48 00 ab ..." there are 3
    // characters per raw byte, and since the actual PEL data starts
    // at the 16th byte, the code will grab the PEL data starting at
    // offset 48 in the string.
    static constexpr size_t pelStart = 16 * 3;

    if (esel.size() <= pelStart)
    {
        log<level::ERR>("ESEL data too short",
                        entry("ESEL_SIZE=%d", esel.size()));

        throw std::length_error("ESEL data too short");
    }

    for (size_t i = pelStart; i < esel.size(); i += 3)
    {
        if (i + 1 < esel.size())
        {
            byteString = esel.substr(i, 2);
            data.push_back(std::stoi(byteString, nullptr, 16));
        }
        else
        {
            log<level::ERR>("ESEL data too short",
                            entry("ESEL_SIZE=%d", esel.size()));
            throw std::length_error("ESEL data too short");
        }
    }

    return data;
}

void Manager::erase(uint32_t obmcLogID)
{
    Repository::LogID id{Repository::LogID::Obmc(obmcLogID)};

    auto path = std::string(OBJ_ENTRY) + '/' + std::to_string(obmcLogID);
    _pelEntries.erase(path);
    _repo.remove(id);
}

bool Manager::isDeleteProhibited(uint32_t /*obmcLogID*/)
{
    return false;
}

PelFFDC Manager::convertToPelFFDC(const FFDCEntries& ffdc)
{
    PelFFDC pelFFDC;

    std::for_each(ffdc.begin(), ffdc.end(), [&pelFFDC](const auto& f) {
        PelFFDCfile pf;
        pf.subType = std::get<ffdcSubtypePos>(f);
        pf.version = std::get<ffdcVersionPos>(f);
        pf.fd = std::get<ffdcFDPos>(f);

        switch (std::get<ffdcFormatPos>(f))
        {
            case Create::FFDCFormat::JSON:
                pf.format = UserDataFormat::json;
                break;
            case Create::FFDCFormat::CBOR:
                pf.format = UserDataFormat::cbor;
                break;
            case Create::FFDCFormat::Text:
                pf.format = UserDataFormat::text;
                break;
            case Create::FFDCFormat::Custom:
                pf.format = UserDataFormat::custom;
                break;
        }

        pelFFDC.push_back(pf);
    });

    return pelFFDC;
}

void Manager::createPEL(const std::string& message, uint32_t obmcLogID,
                        uint64_t timestamp,
                        phosphor::logging::Entry::Level severity,
                        const std::vector<std::string>& additionalData,
                        const std::vector<std::string>& /*associations*/,
                        const FFDCEntries& ffdc)
{
    auto entry = _registry.lookup(message, rg::LookupType::name);
    auto pelFFDC = convertToPelFFDC(ffdc);
    AdditionalData ad{additionalData};
    std::string msg;

    if (!entry)
    {
        // Instead, get the default entry that means there is no
        // other matching entry.  This error will still use the
        // AdditionalData values of the original error, and this
        // code will add the error message value that wasn't found
        // to this AD.  This way, there will at least be a PEL,
        // possibly with callouts, to allow users to debug the
        // issue that caused the error even without its own PEL.
        msg = "Event not found in PEL message registry: " + message;
        log<level::INFO>(msg.c_str());

        entry = _registry.lookup(defaultLogMessage, rg::LookupType::name);
        if (!entry)
        {
            log<level::ERR>("Default event not found in PEL message registry");
            return;
        }

        ad.add(additional_data::error, message);
    }

    auto pel = std::make_unique<openpower::pels::PEL>(
        *entry, obmcLogID, timestamp, severity, ad, pelFFDC, *_dataIface,
        *_journal);

    _repo.add(pel);

    if (_repo.sizeWarning())
    {
        scheduleRepoPrune();
    }

    auto src = pel->primarySRC();
    if (src)
    {
        auto m = fmt::format("Created PEL {:#x} (BMC ID {}) with SRC {}",
                             pel->id(), pel->obmcLogID(),
                             (*src)->asciiString());
        while (m.back() == ' ')
        {
            m.pop_back();
        }
        log<level::INFO>(m.c_str());
    }

    // Check for severity 0x51 and update boot progress SRC
    updateProgressSRC(pel);

    // Activate any resulting service indicators if necessary
    auto policy = service_indicators::getPolicy(*_dataIface);
    policy->activate(*pel);

    updateDBusSeverity(*pel);
    updateEventId(pel);
    updateResolution(*pel);
    createPELEntry(obmcLogID);

    // Check if firmware should quiesce system due to error
    checkPelAndQuiesce(pel);
}

sdbusplus::message::unix_fd Manager::getPEL(uint32_t pelID)
{
    Repository::LogID id{Repository::LogID::Pel(pelID)};
    std::optional<int> fd;

    log<level::DEBUG>("getPEL", entry("PEL_ID=0x%X", pelID));

    try
    {
        fd = _repo.getPELFD(id);
    }
    catch (const std::exception& e)
    {
        throw common_error::InternalFailure();
    }

    if (!fd)
    {
        throw common_error::InvalidArgument();
    }

    scheduleFDClose(*fd);

    return *fd;
}

void Manager::scheduleFDClose(int fd)
{
    _fdCloserEventSource = std::make_unique<sdeventplus::source::Defer>(
        _event, std::bind(std::mem_fn(&Manager::closeFD), this, fd,
                          std::placeholders::_1));
}

void Manager::closeFD(int fd, sdeventplus::source::EventBase& /*source*/)
{
    close(fd);
    _fdCloserEventSource.reset();
}

std::vector<uint8_t> Manager::getPELFromOBMCID(uint32_t obmcLogID)
{
    Repository::LogID id{Repository::LogID::Obmc(obmcLogID)};
    std::optional<std::vector<uint8_t>> data;

    log<level::DEBUG>("getPELFromOBMCID", entry("OBMC_LOG_ID=%d", obmcLogID));

    try
    {
        data = _repo.getPELData(id);
    }
    catch (const std::exception& e)
    {
        throw common_error::InternalFailure();
    }

    if (!data)
    {
        throw common_error::InvalidArgument();
    }

    return *data;
}

void Manager::hostAck(uint32_t pelID)
{
    Repository::LogID id{Repository::LogID::Pel(pelID)};

    log<level::DEBUG>("HostAck", entry("PEL_ID=0x%X", pelID));

    if (!_repo.hasPEL(id))
    {
        throw common_error::InvalidArgument();
    }

    if (_hostNotifier)
    {
        _hostNotifier->ackPEL(pelID);
    }
}

void Manager::hostReject(uint32_t pelID, RejectionReason reason)
{
    Repository::LogID id{Repository::LogID::Pel(pelID)};

    log<level::DEBUG>("HostReject", entry("PEL_ID=0x%X", pelID),
                      entry("REASON=%d", static_cast<int>(reason)));

    if (!_repo.hasPEL(id))
    {
        throw common_error::InvalidArgument();
    }

    if (reason == RejectionReason::BadPEL)
    {
        AdditionalData data;
        data.add("BAD_ID", getNumberString("0x%08X", pelID));
        _eventLogger.log("org.open_power.Logging.Error.SentBadPELToHost",
                         Entry::Level::Informational, data);
        if (_hostNotifier)
        {
            _hostNotifier->setBadPEL(pelID);
        }
    }
    else if ((reason == RejectionReason::HostFull) && _hostNotifier)
    {
        _hostNotifier->setHostFull(pelID);
    }
}

void Manager::scheduleRepoPrune()
{
    _repoPrunerEventSource = std::make_unique<sdeventplus::source::Defer>(
        _event, std::bind(std::mem_fn(&Manager::pruneRepo), this,
                          std::placeholders::_1));
}

void Manager::pruneRepo(sdeventplus::source::EventBase& /*source*/)
{
    auto idsWithHwIsoEntry = _dataIface->getLogIDWithHwIsolation();

    auto idsToDelete = _repo.prune(idsWithHwIsoEntry);

    // Remove the OpenBMC event logs for the PELs that were just removed.
    std::for_each(idsToDelete.begin(), idsToDelete.end(),
                  [this](auto id) { this->_logManager.erase(id); });

    _repoPrunerEventSource.reset();
}

void Manager::setupPELDeleteWatch()
{
    _pelFileDeleteFD = inotify_init1(IN_NONBLOCK);
    if (-1 == _pelFileDeleteFD)
    {
        auto e = errno;
        std::string msg = "inotify_init1 failed with errno " +
                          std::to_string(e);
        log<level::ERR>(msg.c_str());
        abort();
    }

    _pelFileDeleteWatchFD = inotify_add_watch(
        _pelFileDeleteFD, _repo.repoPath().c_str(), IN_DELETE);
    if (-1 == _pelFileDeleteWatchFD)
    {
        auto e = errno;
        std::string msg = "inotify_add_watch failed with error " +
                          std::to_string(e);
        log<level::ERR>(msg.c_str());
        abort();
    }

    _pelFileDeleteEventSource = std::make_unique<sdeventplus::source::IO>(
        _event, _pelFileDeleteFD, EPOLLIN,
        std::bind(std::mem_fn(&Manager::pelFileDeleted), this,
                  std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3));
}

void Manager::pelFileDeleted(sdeventplus::source::IO& /*io*/, int /*fd*/,
                             uint32_t revents)
{
    if (!(revents & EPOLLIN))
    {
        return;
    }

    // An event for 1 PEL uses 48B. When all PELs are deleted at once,
    // as many events as there is room for can be handled in one callback.
    // A size of 2000 will allow 41 to be processed, with additional
    // callbacks being needed to process the remaining ones.
    std::array<uint8_t, 2000> data{};
    auto bytesRead = read(_pelFileDeleteFD, data.data(), data.size());
    if (bytesRead < 0)
    {
        auto e = errno;
        std::string msg = "Failed reading data from inotify event, errno = " +
                          std::to_string(e);
        log<level::ERR>(msg.c_str());
        abort();
    }

    auto offset = 0;
    while (offset < bytesRead)
    {
        auto event = reinterpret_cast<inotify_event*>(&data[offset]);
        if (event->mask & IN_DELETE)
        {
            std::string filename{event->name};

            // Get the PEL ID from the filename and tell the
            // repo it's been removed, and then delete the BMC
            // event log if it's there.
            auto pos = filename.find_first_of('_');
            if (pos != std::string::npos)
            {
                try
                {
                    auto idString = filename.substr(pos + 1);
                    auto pelID = std::stoul(idString, nullptr, 16);

                    Repository::LogID id{Repository::LogID::Pel(pelID)};
                    auto removedLogID = _repo.remove(id);
                    if (removedLogID)
                    {
                        _logManager.erase(removedLogID->obmcID.id);
                    }
                }
                catch (const std::exception& e)
                {
                    log<level::INFO>("Could not find PEL ID from its filename",
                                     entry("FILENAME=%s", filename.c_str()));
                }
            }
        }

        offset += offsetof(inotify_event, name) + event->len;
    }
}

std::tuple<uint32_t, uint32_t> Manager::createPELWithFFDCFiles(
    std::string message, Entry::Level severity,
    std::map<std::string, std::string> additionalData,
    std::vector<std::tuple<
        sdbusplus::xyz::openbmc_project::Logging::server::Create::FFDCFormat,
        uint8_t, uint8_t, sdbusplus::message::unix_fd>>
        fFDC)
{
    _logManager.createWithFFDC(message, severity, additionalData, fFDC);

    return {_logManager.lastEntryID(), _repo.lastPelID()};
}

std::string Manager::getPELJSON(uint32_t obmcLogID)
{
    // Throws InvalidArgument if not found
    auto pelID = getPELIdFromBMCLogId(obmcLogID);

    auto cmd = fmt::format("/usr/bin/peltool -i {:#x}", pelID);

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
    {
        log<level::ERR>(fmt::format("Error running {}", cmd).c_str());
        throw common_error::InternalFailure();
    }

    std::string output;
    std::array<char, 1024> buffer;
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
    {
        output.append(buffer.data());
    }

    int rc = pclose(pipe);
    if (WEXITSTATUS(rc) != 0)
    {
        log<level::ERR>(
            fmt::format("Error running {}, rc = {}", cmd, rc).c_str());
        throw common_error::InternalFailure();
    }

    return output;
}

void Manager::checkPelAndQuiesce(std::unique_ptr<openpower::pels::PEL>& pel)
{
    if ((pel->userHeader().severity() ==
         static_cast<uint8_t>(SeverityType::nonError)) ||
        (pel->userHeader().severity() ==
         static_cast<uint8_t>(SeverityType::recovered)))
    {
        log<level::DEBUG>(
            "PEL severity informational or recovered. no quiesce needed");
        return;
    }
    if (!_logManager.isQuiesceOnErrorEnabled())
    {
        log<level::DEBUG>("QuiesceOnHwError not enabled, no quiesce needed");
        return;
    }

    CreatorID creatorID{pel->privateHeader().creatorID()};

    if ((creatorID != CreatorID::openBMC) &&
        (creatorID != CreatorID::hostboot) &&
        (creatorID != CreatorID::ioDrawer) && (creatorID != CreatorID::occ) &&
        (creatorID != CreatorID::phyp))
    {
        return;
    }

    // Now check if it has any type of callout
    if (pel->isHwCalloutPresent())
    {
        log<level::INFO>(
            "QuiesceOnHwError enabled, PEL severity not nonError or recovered, "
            "and callout is present");

        _logManager.quiesceOnError(pel->obmcLogID());
    }
}

std::string Manager::getEventId(const openpower::pels::PEL& pel) const
{
    std::string str;
    auto src = pel.primarySRC();
    if (src)
    {
        const auto& hexwords = (*src)->hexwordData();

        std::string refcode = (*src)->asciiString();
        size_t pos = refcode.find_last_not_of(0x20);
        if (pos != std::string::npos)
        {
            refcode.erase(pos + 1);
        }
        str = refcode;

        for (auto& value : hexwords)
        {
            str += " ";
            str += getNumberString("%08X", value);
        }
    }
    return sanitizeFieldForDBus(str);
}

void Manager::updateEventId(std::unique_ptr<openpower::pels::PEL>& pel)
{
    std::string eventIdStr = getEventId(*pel);

    auto entryN = _logManager.entries.find(pel->obmcLogID());
    if (entryN != _logManager.entries.end())
    {
        entryN->second->eventId(eventIdStr);
    }
}

std::string Manager::sanitizeFieldForDBus(std::string field)
{
    std::for_each(field.begin(), field.end(), [](char& ch) {
        if (((ch < ' ') || (ch > '~')) && (ch != '\n') && (ch != '\t'))
        {
            ch = ' ';
        }
    });
    return field;
}

std::string Manager::getResolution(const openpower::pels::PEL& pel) const
{
    std::string str;
    std::string resolution;
    auto src = pel.primarySRC();
    if (src)
    {
        // First extract the callout pointer and then go through
        const auto& callouts = (*src)->callouts();
        namespace pv = openpower::pels::pel_values;
        // All PELs dont have callout, check before parsing callout data
        if (callouts)
        {
            const auto& entries = callouts->callouts();
            // Entry starts with index 1
            uint8_t index = 1;
            for (auto& entry : entries)
            {
                resolution += std::to_string(index) + ". ";
                // Adding Location code to resolution
                if (!entry->locationCode().empty())
                    resolution += "Location Code: " + entry->locationCode() +
                                  ", ";
                if (entry->fruIdentity())
                {
                    // Get priority and set the resolution string
                    str = pv::getValue(entry->priority(),
                                       pel_values::calloutPriorityValues,
                                       pel_values::registryNamePos);
                    str[0] = toupper(str[0]);
                    resolution += "Priority: " + str + ", ";
                    if (entry->fruIdentity()->getPN().has_value())
                    {
                        resolution +=
                            "PN: " + entry->fruIdentity()->getPN().value() +
                            ", ";
                    }
                    if (entry->fruIdentity()->getSN().has_value())
                    {
                        resolution +=
                            "SN: " + entry->fruIdentity()->getSN().value() +
                            ", ";
                    }
                    if (entry->fruIdentity()->getCCIN().has_value())
                    {
                        resolution +=
                            "CCIN: " + entry->fruIdentity()->getCCIN().value() +
                            ", ";
                    }
                    // Add the maintenance procedure
                    if (entry->fruIdentity()->getMaintProc().has_value())
                    {
                        resolution +=
                            "Procedure: " +
                            entry->fruIdentity()->getMaintProc().value() + ", ";
                    }
                }
                resolution.resize(resolution.size() - 2);
                resolution += "\n";
                index++;
            }
        }
    }
    return sanitizeFieldForDBus(resolution);
}

bool Manager::updateResolution(const openpower::pels::PEL& pel)
{
    std::string callouts = getResolution(pel);
    auto entryN = _logManager.entries.find(pel.obmcLogID());
    if (entryN != _logManager.entries.end())
    {
        entryN->second->resolution(callouts, true);
    }

    return false;
}

void Manager::updateDBusSeverity(const openpower::pels::PEL& pel)
{
    // The final severity of the PEL may not agree with the
    // original severity of the D-Bus event log.  Update the
    // D-Bus property to match in some cases.  This is to
    // ensure there isn't a Critical or Warning Redfish event
    // log for an informational or recovered PEL (or vice versa).
    // This doesn't make an explicit call to serialize the new
    // event log property value because updateEventId() is called
    // right after this and will do it.
    auto sevType =
        static_cast<SeverityType>(pel.userHeader().severity() & 0xF0);

    auto entryN = _logManager.entries.find(pel.obmcLogID());
    if (entryN != _logManager.entries.end())
    {
        auto newSeverity = fixupLogSeverity(entryN->second->severity(),
                                            sevType);
        if (newSeverity)
        {
            log<level::INFO>(
                fmt::format(
                    "Changing event log {} severity from {} "
                    "to {} to match PEL",
                    entryN->second->id(),
                    Entry::convertLevelToString(entryN->second->severity()),
                    Entry::convertLevelToString(*newSeverity))
                    .c_str());

            entryN->second->severity(*newSeverity, true);
        }
    }
}

void Manager::setEntryPath(uint32_t obmcLogID)
{
    Repository::LogID id{Repository::LogID::Obmc(obmcLogID)};
    if (auto attributes = _repo.getPELAttributes(id); attributes)
    {
        auto& attr = attributes.value().get();
        auto entry = _logManager.entries.find(obmcLogID);
        if (entry != _logManager.entries.end())
        {
            entry->second->path(attr.path, true);
        }
    }
}

void Manager::setServiceProviderNotifyFlag(uint32_t obmcLogID)
{
    Repository::LogID id{Repository::LogID::Obmc(obmcLogID)};
    if (auto attributes = _repo.getPELAttributes(id); attributes)
    {
        auto& attr = attributes.value().get();
        auto entry = _logManager.entries.find(obmcLogID);
        if (entry != _logManager.entries.end())
        {
            if (attr.actionFlags.test(callHomeFlagBit))
            {
                entry->second->serviceProviderNotify(Entry::Notify::Notify);
            }
            else
            {
                entry->second->serviceProviderNotify(Entry::Notify::Inhibit);
            }
        }
    }
}

void Manager::createPELEntry(uint32_t obmcLogID, bool skipIaSignal)
{
    std::map<std::string, PropertiesVariant> varData;
    Repository::LogID id{Repository::LogID::Obmc(obmcLogID)};
    if (auto attributes = _repo.getPELAttributes(id); attributes)
    {
        namespace pv = openpower::pels::pel_values;
        auto& attr = attributes.value().get();

        // get the hidden flag values
        auto sevType = static_cast<SeverityType>(attr.severity & 0xF0);
        auto isHidden = true;
        if (((sevType != SeverityType::nonError) &&
             attr.actionFlags.test(reportFlagBit) &&
             !attr.actionFlags.test(hiddenFlagBit)) ||
            ((sevType == SeverityType::nonError) &&
             attr.actionFlags.test(serviceActionFlagBit)))
        {
            isHidden = false;
        }
        varData.emplace(std::string("Hidden"), isHidden);
        varData.emplace(
            std::string("Subsystem"),
            pv::getValue(attr.subsystem, pel_values::subsystemValues));

        varData.emplace(
            std::string("ManagementSystemAck"),
            (attr.hmcState == TransmissionState::acked ? true : false));

        // Path to create PELEntry Interface is same as PEL
        auto path = std::string(OBJ_ENTRY) + '/' + std::to_string(obmcLogID);
        // Create Interface for PELEntry and set properties
        auto pelEntry = std::make_unique<PELEntry>(_logManager.getBus(), path,
                                                   varData, obmcLogID, &_repo);
        if (!skipIaSignal)
        {
            pelEntry->emit_added();
        }
        _pelEntries.emplace(std::move(path), std::move(pelEntry));
    }
}

uint32_t Manager::getPELIdFromBMCLogId(uint32_t bmcLogId)
{
    Repository::LogID id{Repository::LogID::Obmc(bmcLogId)};
    if (auto logId = _repo.getLogID(id); !logId.has_value())
    {
        throw common_error::InvalidArgument();
    }
    else
    {
        return logId->pelID.id;
    }
}

uint32_t Manager::getBMCLogIdFromPELId(uint32_t pelId)
{
    Repository::LogID id{Repository::LogID::Pel(pelId)};
    if (auto logId = _repo.getLogID(id); !logId.has_value())
    {
        throw common_error::InvalidArgument();
    }
    else
    {
        return logId->obmcID.id;
    }
}

void Manager::updateProgressSRC(
    std::unique_ptr<openpower::pels::PEL>& pel) const
{
    // Check for pel severity of type - 0x51 = critical error, system
    // termination
    if (pel->userHeader().severity() == 0x51)
    {
        auto src = pel->primarySRC();
        if (src)
        {
            std::vector<uint8_t> asciiSRC = (*src)->getSrcStruct();
            uint64_t srcRefCode = 0;

            // Read bytes from offset [40-47] e.g. BD8D1001
            for (int i = 0; i < 8; i++)
            {
                srcRefCode |= (static_cast<uint64_t>(asciiSRC[40 + i])
                               << (8 * i));
            }

            try
            {
                _dataIface->createProgressSRC(srcRefCode, asciiSRC);
            }
            catch (std::exception& e)
            {
                // Exception - may be no boot progress interface on dbus
            }
        }
    }
}

void Manager::scheduleObmcLogDelete(uint32_t obmcLogID)
{
    _obmcLogDeleteEventSource = std::make_unique<sdeventplus::source::Defer>(
        _event, std::bind(std::mem_fn(&Manager::deleteObmcLog), this,
                          std::placeholders::_1, obmcLogID));
}

void Manager::deleteObmcLog(sdeventplus::source::EventBase&, uint32_t obmcLogID)
{
    log<level::INFO>(
        fmt::format("Removing event log with no PEL: {}", obmcLogID).c_str());
    _logManager.erase(obmcLogID);
    _obmcLogDeleteEventSource.reset();
}

} // namespace pels
} // namespace openpower
