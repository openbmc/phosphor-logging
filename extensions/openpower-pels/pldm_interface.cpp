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
#include "pldm_interface.hpp"

#include <libpldm/base.h>
#include <libpldm/file_io.h>
#include <unistd.h>

#include <fstream>
#include <phosphor-logging/log.hpp>

namespace openpower::pels
{

using namespace phosphor::logging;
using namespace sdeventplus;
using namespace sdeventplus::source;

constexpr auto eidPath = "/usr/share/pldm/host_eid";
constexpr mctp_eid_t defaultEIDValue = 9;

constexpr uint16_t pelFileType = 0;

PLDMInterface::~PLDMInterface()
{
    closeFD();
}

void PLDMInterface::closeFD()
{
    if (_fd >= 0)
    {
        close(_fd);
        _fd = -1;
    }
}

void PLDMInterface::readEID()
{
    _eid = defaultEIDValue;

    std::ifstream eidFile{eidPath};
    if (!eidFile.good())
    {
        log<level::ERR>("Could not open host EID file");
    }
    else
    {
        std::string eid;
        eidFile >> eid;
        if (!eid.empty())
        {
            _eid = atoi(eid.c_str());
        }
        else
        {
            log<level::ERR>("EID file was empty");
        }
    }
}

void PLDMInterface::open()
{
    _fd = pldm_open();
    if (_fd < 0)
    {
        auto e = errno;
        log<level::ERR>("pldm_open failed", entry("ERRNO=%d", e),
                        entry("RC=%d\n", _fd));
        throw std::exception{};
    }
}

CmdStatus PLDMInterface::sendNewLogCmd(uint32_t id, uint32_t size)
{
    try
    {
        closeFD();

        open();

        readInstanceID();

        registerReceiveCallback();

        doSend(id, size);
    }
    catch (const std::exception& e)
    {
        closeFD();

        _inProgress = false;
        _source.reset();
        return CmdStatus::failure;
    }

    _inProgress = true;
    _receiveTimer.restartOnce(_receiveTimeout);
    return CmdStatus::success;
}

void PLDMInterface::registerReceiveCallback()
{
    _source = std::make_unique<IO>(
        _event, _fd, EPOLLIN,
        std::bind(std::mem_fn(&PLDMInterface::receive), this,
                  std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3));
}

void PLDMInterface::readInstanceID()
{
    try
    {
        _instanceID = _dataIface.getPLDMInstanceID(_eid);
    }
    catch (const std::exception& e)
    {
        log<level::ERR>(
            "Failed to get instance ID from PLDM Requester D-Bus daemon",
            entry("ERROR=%s", e.what()));
        throw;
    }
}

void PLDMInterface::doSend(uint32_t id, uint32_t size)
{
    std::array<uint8_t, sizeof(pldm_msg_hdr) + sizeof(pelFileType) +
                            sizeof(id) + sizeof(uint64_t)>
        requestMsg;

    auto request = reinterpret_cast<pldm_msg*>(requestMsg.data());

    auto rc = encode_new_file_req(_instanceID, pelFileType, id, size, request);
    if (rc != PLDM_SUCCESS)
    {
        log<level::ERR>("encode_new_file_req failed", entry("RC=%d", rc));
        throw std::exception{};
    }

    rc = pldm_send(_eid, _fd, requestMsg.data(), requestMsg.size());
    if (rc < 0)
    {
        auto e = errno;
        log<level::ERR>("pldm_send failed", entry("RC=%d", rc),
                        entry("ERRNO=%d", e));

        throw std::exception{};
    }
}

void PLDMInterface::receive(IO& io, int fd, uint32_t revents)
{
    if (!(revents & EPOLLIN))
    {
        return;
    }

    uint8_t* responseMsg = nullptr;
    size_t responseSize = 0;
    ResponseStatus status = ResponseStatus::success;

    auto rc = pldm_recv(_eid, fd, _instanceID, &responseMsg, &responseSize);
    if (rc < 0)
    {
        if (rc == PLDM_REQUESTER_INSTANCE_ID_MISMATCH)
        {
            // We got a response to someone else's message. Ignore it.
            return;
        }
        else if (rc == PLDM_REQUESTER_NOT_RESP_MSG)
        {
            // Due to the MCTP loopback, we may get notified of the message
            // we just sent.
            return;
        }

        auto e = errno;
        log<level::ERR>("pldm_recv failed", entry("RC=%d", rc),
                        entry("ERRNO=%d", e));
        status = ResponseStatus::failure;

        responseMsg = nullptr;
    }

    _inProgress = false;
    _receiveTimer.setEnabled(false);
    closeFD();
    _source.reset();

    if (status == ResponseStatus::success)
    {
        uint8_t completionCode = 0;
        auto response = reinterpret_cast<pldm_msg*>(responseMsg);

        auto decodeRC = decode_new_file_resp(response, PLDM_NEW_FILE_RESP_BYTES,
                                             &completionCode);
        if (decodeRC < 0)
        {
            log<level::ERR>("decode_new_file_resp failed",
                            entry("RC=%d", decodeRC));
            status = ResponseStatus::failure;
        }
        else
        {
            if (completionCode != PLDM_SUCCESS)
            {
                log<level::ERR>("Bad PLDM completion code",
                                entry("COMPLETION_CODE=%d", completionCode));
                status = ResponseStatus::failure;
            }
        }
    }

    if (_responseFunc)
    {
        try
        {
            (*_responseFunc)(status);
        }
        catch (const std::exception& e)
        {
            log<level::ERR>("PLDM response callback threw an exception",
                            entry("ERROR=%s", e.what()));
        }
    }

    if (responseMsg)
    {
        free(responseMsg);
    }
}

void PLDMInterface::receiveTimerExpired()
{
    log<level::ERR>("Timed out waiting for PLDM response");
    cancelCmd();

    if (_responseFunc)
    {
        try
        {
            (*_responseFunc)(ResponseStatus::failure);
        }
        catch (const std::exception& e)
        {
            log<level::ERR>("PLDM response callback threw an exception",
                            entry("ERROR=%s", e.what()));
        }
    }
}

void PLDMInterface::cancelCmd()
{
    _inProgress = false;
    _source.reset();

    if (_receiveTimer.isEnabled())
    {
        _receiveTimer.setEnabled(false);
    }

    closeFD();
}

} // namespace openpower::pels
