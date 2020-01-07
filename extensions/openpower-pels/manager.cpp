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
#include "pel.hpp"

#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <xyz/openbmc_project/Common/error.hpp>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;
namespace fs = std::filesystem;

namespace common_error = sdbusplus::xyz::openbmc_project::Common::Error;

namespace additional_data
{
constexpr auto rawPEL = "RAWPEL";
}

void Manager::create(const std::string& message, uint32_t obmcLogID,
                     uint64_t timestamp, Entry::Level severity,
                     const std::vector<std::string>& additionalData,
                     const std::vector<std::string>& associations)
{
    AdditionalData ad{additionalData};

    // If a PEL was passed in, use that.  Otherwise, create one.
    auto rawPelPath = ad.getValue(additional_data::rawPEL);
    if (rawPelPath)
    {
        addRawPEL(*rawPelPath, obmcLogID);
    }
    else
    {
        createPEL(message, obmcLogID, timestamp, severity, additionalData,
                  associations);
    }
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

        auto pel = std::make_unique<openpower::pels::PEL>(data, obmcLogID);
        if (pel->valid())
        {
            // PELs created by others still need these fields set by us.
            pel->assignID();
            pel->setCommitTime();

            try
            {
                _repo.add(pel);
            }
            catch (std::exception& e)
            {
                // Probably a full or r/o filesystem, not much we can do.
                log<level::ERR>("Unable to add PEL to Repository",
                                entry("PEL_ID=0x%X", pel->id()));
            }
        }
        else
        {
            log<level::ERR>("Invalid PEL found",
                            entry("PELFILE=%s", rawPelPath.c_str()),
                            entry("OBMCLOGID=%d", obmcLogID));
            // TODO, make a whole new OpenBMC event log + PEL
        }
    }
    else
    {
        log<level::ERR>("Raw PEL file from BMC event log does not exist",
                        entry("PELFILE=%s", (rawPelPath).c_str()),
                        entry("OBMCLOGID=%d", obmcLogID));
    }
}

void Manager::erase(uint32_t obmcLogID)
{
    Repository::LogID id{Repository::LogID::Obmc(obmcLogID)};

    _repo.remove(id);
}

bool Manager::isDeleteProhibited(uint32_t obmcLogID)
{
    return false;
}

void Manager::createPEL(const std::string& message, uint32_t obmcLogID,
                        uint64_t timestamp,
                        phosphor::logging::Entry::Level severity,
                        const std::vector<std::string>& additionalData,
                        const std::vector<std::string>& associations)
{
    auto entry = _registry.lookup(message);
    std::string msg;

    if (entry)
    {
        AdditionalData ad{additionalData};

        auto pel = std::make_unique<openpower::pels::PEL>(
            *entry, obmcLogID, timestamp, severity, ad, *_dataIface);

        _repo.add(pel);

        auto src = pel->primarySRC();
        if (src)
        {
            using namespace std::literals::string_literals;
            char id[11];
            sprintf(id, "0x%08X", pel->id());
            msg = "Created PEL "s + id + " with SRC "s + (*src)->asciiString();
            while (msg.back() == ' ')
            {
                msg.pop_back();
            }
            log<level::INFO>(msg.c_str());
        }
    }
    else
    {
        // TODO ibm-openbmc/dev/1151: Create a new PEL for this case.
        // For now, just trace it.
        msg = "Event not found in PEL message registry: " + message;
        log<level::INFO>(msg.c_str());
    }
}

sdbusplus::message::unix_fd Manager::getPEL(uint32_t pelID)
{
    Repository::LogID id{Repository::LogID::Pel(pelID)};
    std::optional<int> fd;

    char msg[50];
    sprintf(msg, ">>getPEL 0x%X", pelID);
    log<level::INFO>(msg);

    try
    {
        fd = _repo.getPELFD(id);
    }
    catch (std::exception& e)
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
        _logManager.getBus().get_event(),
        std::bind(std::mem_fn(&Manager::closeFD), this, fd,
                  std::placeholders::_1));
}

void Manager::closeFD(int fd, sdeventplus::source::EventBase& source)
{
    close(fd);
    _fdCloserEventSource.reset();
}

std::vector<uint8_t> Manager::getPELFromOBMCID(uint32_t obmcLogID)
{
    Repository::LogID id{Repository::LogID::Obmc(obmcLogID)};
    std::optional<std::vector<uint8_t>> data;

    try
    {
        data = _repo.getPELData(id);
    }
    catch (std::exception& e)
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

    if (!_repo.hasPEL(id))
    {
        throw common_error::InvalidArgument();
    }

    if (_hostNotifier)
    {
        if (reason == RejectionReason::BadPEL)
        {
            _hostNotifier->setBadPEL(pelID);
        }
        else if (reason == RejectionReason::HostFull)
        {
            _hostNotifier->setHostFull(pelID);
        }
    }
}

} // namespace pels
} // namespace openpower
