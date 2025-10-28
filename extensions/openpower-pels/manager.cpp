// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "manager.hpp"

#include "additional_data.hpp"
#include "elog_serialize.hpp"
#include "json_utils.hpp"
#include "pel.hpp"
#include "pel_entry.hpp"
#include "pel_values.hpp"
#include "service_indicators.hpp"
#include "severity.hpp"

#include <sys/inotify.h>
#include <unistd.h>

#include <phosphor-logging/lg2.hpp>
#include <xyz/openbmc_project/Common/error.hpp>
#include <xyz/openbmc_project/Logging/Create/server.hpp>

#include <filesystem>
#include <format>
#include <fstream>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;
namespace fs = std::filesystem;
namespace rg = openpower::pels::message;

namespace common_error = sdbusplus::xyz::openbmc_project::Common::Error;

using Create = sdbusplus::server::xyz::openbmc_project::logging::Create;

namespace additional_data
{
constexpr auto rawPEL = "RAWPEL";
constexpr auto esel = "ESEL";
constexpr auto error = "ERROR_NAME";
} // namespace additional_data

constexpr auto defaultLogMessage = "xyz.openbmc_project.Logging.Error.Default";
constexpr uint32_t bmcThermalCompID = 0x2700;
constexpr uint32_t bmcFansCompID = 0x2800;

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
                     const std::map<std::string, std::string>& additionalData,
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
            lg2::error(
                "Filesystem error reading a raw PEL. File = {FILE}, obmcLogID = {LOGID}",
                "FILE", rawPelPath, "LOGID", obmcLogID);
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
        lg2::error(
            "Raw PEL file from BMC event log does not exit. File = {FILE}, obmcLogID = {LOGID}",
            "FILE", rawPelPath, "LOGID", obmcLogID);
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
                lg2::warning(
                    "Duplicate HostBoot PEL ID {ID} found, moving it to archive folder",
                    "ID", lg2::hex, pel->id());

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
            lg2::debug("Adding external PEL {ID} (BMC ID {BMCID}) to repo",
                       "ID", lg2::hex, pel->id(), "BMCID", obmcLogID);
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
            lg2::error("Unable to add PEL {ID} to Repository", "ID", lg2::hex,
                       pel->id());
        }

        updateEventId(pel);
        updateResolution(*pel);
        serializeLogEntry(obmcLogID);
        createPELEntry(obmcLogID);

        // Check if firmware should quiesce system due to error
        checkPelAndQuiesce(pel);
    }
    else
    {
        lg2::error("Invalid PEL received from the host. BMC ID = {ID}", "ID",
                   obmcLogID);

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

    lg2::debug("Adding PEL from ESEL. BMC ID = {ID}", "ID", obmcLogID);

    try
    {
        data = eselToRawData(esel);
    }
    catch (const std::exception& e)
    {
        // Try to add it below anyway, so it follows the usual bad data path.
        lg2::error("Problems converting ESEL string to a byte vector");
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
        lg2::error("ESEL data too short, length = {LEN}", "LEN", esel.size());
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
            lg2::error("ESEL data too short, length = {LEN}", "LEN",
                       esel.size());
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

void Manager::getLogIDWithHwIsolation(std::vector<uint32_t>& idsWithHwIsoEntry)
{
    idsWithHwIsoEntry = _dataIface->getLogIDWithHwIsolation();
}

bool Manager::isDeleteProhibited(uint32_t obmcLogID)
{
    Repository::LogID id{Repository::LogID::Obmc(obmcLogID)};
    if (!_repo.hasPEL(id))
    {
        return false;
    }
    auto entryPath{std::string(OBJ_ENTRY) + '/' + std::to_string(obmcLogID)};
    auto entry = _pelEntries.find(entryPath);
    if (entry != _pelEntries.end())
    {
        if (entry->second->guard())
        {
            auto hwIsolationAssocPaths = _dataIface->getAssociatedPaths(
                entryPath += "/isolated_hw_entry", "/", 0,
                {"xyz.openbmc_project.HardwareIsolation.Entry"});
            if (!hwIsolationAssocPaths.empty())
            {
                return true;
            }
        }
    }
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

void Manager::createPEL(
    const std::string& message, uint32_t obmcLogID, uint64_t timestamp,
    phosphor::logging::Entry::Level severity,
    const std::map<std::string, std::string>& additionalData,
    const std::vector<std::string>& /*associations*/, const FFDCEntries& ffdc)
{
    auto start = std::chrono::steady_clock::now();
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
        lg2::error("Event not found in PEL message registry: {MSG}", "MSG",
                   message);

        entry = _registry.lookup(defaultLogMessage, rg::LookupType::name);
        if (!entry)
        {
            lg2::error("Default event not found in PEL message registry");
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

    // Check for severity 0x51 and update boot progress SRC
    updateProgressSRC(pel);

    // Activate any resulting service indicators if necessary
    auto policy = service_indicators::getPolicy(*_dataIface);
    policy->activate(*pel);

    updateDBusSeverity(*pel);
    updateEventId(pel);
    updateResolution(*pel);
    serializeLogEntry(obmcLogID);
    createPELEntry(obmcLogID);

    auto src = pel->primarySRC();
    if (src)
    {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start);

        auto asciiString = (*src)->asciiString();
        while (asciiString.back() == ' ')
        {
            asciiString.pop_back();
        }
        lg2::info("Created PEL {ID} (BMC ID {BMCID}) with SRC {SRC}", "ID",
                  lg2::hex, pel->id(), "BMCID", pel->obmcLogID(), "SRC",
                  asciiString, "PEL_CREATE_DURATION", duration.count());
    }

    // Check if firmware should quiesce system due to error
    checkPelAndQuiesce(pel);
}

sdbusplus::message::unix_fd Manager::getPEL(uint32_t pelID)
{
    Repository::LogID id{Repository::LogID::Pel(pelID)};
    std::optional<int> fd;

    lg2::debug("getPEL {ID}", "ID", lg2::hex, pelID);

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

    lg2::debug("getPELFromOBMCID  {BMCID}", "BMCID", obmcLogID);

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

    lg2::debug("HostHack {ID}", "ID", lg2::hex, pelID);

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

    lg2::debug("HostReject {ID}, reason = {REASON}", "ID", lg2::hex, pelID,
               "REASON", reason);

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
        lg2::error("inotify_init1 failed with errno {ERRNO}", "ERRNO", e);
        abort();
    }

    _pelFileDeleteWatchFD = inotify_add_watch(
        _pelFileDeleteFD, _repo.repoPath().c_str(), IN_DELETE);
    if (-1 == _pelFileDeleteWatchFD)
    {
        auto e = errno;
        lg2::error("inotify_add_watch failed with errno {ERRNO}", "ERRNO", e);
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
        lg2::error("Failed reading data from inotify event, errno = {ERRNO}",
                   "ERRNO", e);
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
                    lg2::info("Could not find PEL ID from its filename {NAME}",
                              "NAME", filename);
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
        sdbusplus::server::xyz::openbmc_project::logging::Create::FFDCFormat,
        uint8_t, uint8_t, sdbusplus::message::unix_fd>>
        fFDC)
{
    _logManager.create(message, severity, additionalData, fFDC);

    return {_logManager.lastEntryID(), _repo.lastPelID()};
}

std::string Manager::getPELJSON(uint32_t obmcLogID)
{
    // Throws InvalidArgument if not found
    auto pelID = getPELIdFromBMCLogId(obmcLogID);

    auto cmd = std::format("/usr/bin/peltool -i {:#x}", pelID);

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
    {
        lg2::error("Error running cmd: {CMD}", "CMD", cmd);
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
        lg2::error("Error running cmd: {CMD}, rc = {RC}", "CMD", cmd, "RC", rc);
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
        lg2::debug(
            "PEL severity informational or recovered. no quiesce needed");
        return;
    }
    if (!_logManager.isQuiesceOnErrorEnabled())
    {
        lg2::debug("QuiesceOnHwError not enabled, no quiesce needed");
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
        lg2::info(
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
        entryN->second->eventId(eventIdStr, true);
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

void Manager::serializeLogEntry(uint32_t obmcLogID)
{
    auto entryN = _logManager.entries.find(obmcLogID);
    if (entryN != _logManager.entries.end())
    {
        serialize(*entryN->second);
    }
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
        auto newSeverity =
            fixupLogSeverity(entryN->second->severity(), sevType);
        if (newSeverity)
        {
            lg2::info("Changing event log {ID} severity from {OLD} "
                      "to {NEW} to match PEL",
                      "ID", lg2::hex, entryN->second->id(), "OLD",
                      Entry::convertLevelToString(entryN->second->severity()),
                      "NEW", Entry::convertLevelToString(*newSeverity));

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
                entry->second->serviceProviderNotify(Entry::Notify::Notify,
                                                     true);
            }
            else
            {
                entry->second->serviceProviderNotify(Entry::Notify::Inhibit,
                                                     true);
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

        varData.emplace("PlatformLogID", attr.plid);
        varData.emplace("Deconfig", attr.deconfig);
        varData.emplace("Guard", attr.guard);
        varData.emplace("Timestamp", attr.creationTime);

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
    const size_t refcodeBegin = 40;
    const size_t refcodeSize = 8;

    // Check for pel severity of type - 0x51 = critical error, system
    // termination
    if (pel->userHeader().severity() == 0x51)
    {
        auto src = pel->primarySRC();
        if (src)
        {
            std::vector<uint8_t> asciiSRC = (*src)->getSrcStruct();

            if (asciiSRC.size() < (refcodeBegin + refcodeSize))
            {
                lg2::error(
                    "SRC struct is too short to get progress code ({SIZE})",
                    "SIZE", asciiSRC.size());
                return;
            }

            // Pull the ASCII SRC from offset [40-47] e.g. BD8D1001
            std::vector<uint8_t> srcRefCode(
                asciiSRC.begin() + refcodeBegin,
                asciiSRC.begin() + refcodeBegin + refcodeSize);

            try
            {
                _dataIface->createProgressSRC(srcRefCode, asciiSRC);
            }
            catch (const std::exception&)
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
    lg2::info("Removing event log with no PEL: {BMCID}", "BMCID", obmcLogID);
    _logManager.erase(obmcLogID);
    _obmcLogDeleteEventSource.reset();
}

bool Manager::clearPowerThermalDeconfigFlag(const std::string& locationCode,
                                            openpower::pels::PEL& pel)
{
    // The requirements state that only power-thermal or
    // fan PELs need their deconfig flag cleared.
    static const std::vector<uint32_t> compIDs{bmcThermalCompID, bmcFansCompID};

    if (std::find(compIDs.begin(), compIDs.end(),
                  pel.privateHeader().header().componentID) == compIDs.end())
    {
        return false;
    }

    auto src = pel.primarySRC();
    const auto& callouts = (*src)->callouts();
    if (!callouts)
    {
        return false;
    }

    for (const auto& callout : callouts->callouts())
    {
        // Look for the passed in location code in a callout that
        // is either a normal HW callout or a symbolic FRU with
        // a trusted location code callout.
        if ((callout->locationCode() != locationCode) ||
            !callout->fruIdentity())
        {
            continue;
        }

        if ((callout->fruIdentity()->failingComponentType() !=
             src::FRUIdentity::hardwareFRU) &&
            (callout->fruIdentity()->failingComponentType() !=
             src::FRUIdentity::symbolicFRUTrustedLocCode))
        {
            continue;
        }

        lg2::info(
            "Clearing deconfig flag in PEL {ID} with SRC {SRC} because {LOC} was replaced",
            "ID", lg2::hex, pel.id(), "SRC", (*src)->asciiString().substr(0, 8),
            "LOC", locationCode);
        (*src)->clearErrorStatusFlag(SRC::ErrorStatusFlags::deconfigured);
        return true;
    }
    return false;
}

void Manager::hardwarePresent(const std::string& locationCode)
{
    Repository::PELUpdateFunc handlePowerThermalHardwarePresent =
        [locationCode](openpower::pels::PEL& pel) {
            return Manager::clearPowerThermalDeconfigFlag(locationCode, pel);
        };

    // If the PEL was created by the BMC and has the deconfig flag set,
    // it's a candidate to have the deconfig flag cleared.
    for (const auto& [id, attributes] : _repo.getAttributesMap())
    {
        if ((attributes.creator == static_cast<uint8_t>(CreatorID::openBMC)) &&
            attributes.deconfig)
        {
            auto updated = _repo.updatePEL(attributes.path,
                                           handlePowerThermalHardwarePresent);

            if (updated)
            {
                // Also update the property on D-Bus
                auto objPath = std::string(OBJ_ENTRY) + '/' +
                               std::to_string(id.obmcID.id);
                auto entryN = _pelEntries.find(objPath);
                if (entryN != _pelEntries.end())
                {
                    entryN->second->deconfig(false);
                }
                else
                {
                    lg2::error(
                        "Could not find PEL Entry D-Bus object for {PATH}",
                        "PATH", objPath);
                }
            }
        }
    }
}

} // namespace pels
} // namespace openpower
