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
#include <libpldm/oem/ibm/file_io.h>
#include <unistd.h>

#include <phosphor-logging/lg2.hpp>

#include <fstream>

namespace openpower::pels
{

using namespace sdeventplus;
using namespace sdeventplus::source;

constexpr auto eidPath = "/usr/share/pldm/host_eid";
constexpr mctp_eid_t defaultEIDValue = 9;

constexpr uint16_t pelFileType = 0;

PLDMInterface::~PLDMInterface()
{
    freeIID();
    pldm_instance_db_destroy(_pldm_idb);
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
        lg2::error("Could not open host EID file");
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
            lg2::error("EID file was empty");
        }
    }
}

void PLDMInterface::open()
{
    _fd = pldm_open();
    if (_fd < 0)
    {
        auto e = errno;
        lg2::error("pldm_open failed.  errno = {ERRNO}, rc = {RC}", "ERRNO", e,
                   "RC", _fd);
        throw std::runtime_error{"pldm_open failed"};
    }
}

void PLDMInterface::startCommand()
{
    try
    {
        closeFD();

        open();

        registerReceiveCallback();

        doSend();

        _receiveTimer.restartOnce(_receiveTimeout);
    }
    catch (const std::exception& e)
    {
        lg2::error("startCommand exception: {ERROR}", "ERROR", e);

        cleanupCmd();

        throw;
    }
}

void PLDMInterface::allocIID()
{
    if (_instanceID)
    {
        return;
    }

    pldm_instance_id_t iid = 0;
    auto rc = pldm_instance_id_alloc(_pldm_idb, _eid, &iid);

    if (rc == -EAGAIN)
    {
        throw std::runtime_error("No free instance ids");
    }
    else if (rc)
    {
        throw std::system_category().default_error_condition(rc);
    }

    _instanceID = iid;
}

void PLDMInterface::freeIID()
{
    if (!_instanceID)
    {
        return;
    }

    auto rc = pldm_instance_id_free(_pldm_idb, _eid, *_instanceID);

    if (rc == -EINVAL)
    {
        throw std::runtime_error("Instance ID " + std::to_string(*_instanceID) +
                                 " for TID " + std::to_string(_eid) +
                                 " was not previously allocated");
    }
    else if (rc)
    {
        throw std::system_category().default_error_condition(rc);
    }

    _instanceID = std::nullopt;
}

CmdStatus PLDMInterface::sendNewLogCmd(uint32_t id, uint32_t size)
{
    _pelID = id;
    _pelSize = size;
    _inProgress = true;

    try
    {
        // Allocate the instance ID, as needed.
        if (!_instanceID)
        {
            allocIID();
        }
        startCommand();
    }
    catch (const std::exception& e)
    {
        _inProgress = false;
        return CmdStatus::failure;
    }

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

void PLDMInterface::doSend()
{
    std::array<uint8_t, sizeof(pldm_msg_hdr) + sizeof(pelFileType) +
                            sizeof(_pelID) + sizeof(uint64_t)>
        requestMsg;

    auto request = reinterpret_cast<pldm_msg*>(requestMsg.data());

    auto rc = encode_new_file_req(*_instanceID, pelFileType, _pelID, _pelSize,
                                  request);
    if (rc != PLDM_SUCCESS)
    {
        lg2::error("encode_new_file_req failed, rc = {RC}", "RC", rc);
        throw std::runtime_error{"encode_new_file_req failed"};
    }

    rc = pldm_send(_eid, _fd, requestMsg.data(), requestMsg.size());
    if (rc < 0)
    {
        auto e = errno;
        lg2::error("pldm_send failed, rc = {RC}, errno = {ERRNO}", "RC", rc,
                   "ERRNO", e);
        throw std::runtime_error{"pldm_send failed"};
    }
}

void PLDMInterface::receive(IO& /*io*/, int fd, uint32_t revents)
{
    if (!(revents & EPOLLIN))
    {
        return;
    }

    uint8_t* responseMsg = nullptr;
    size_t responseSize = 0;
    ResponseStatus status = ResponseStatus::success;

    auto rc = pldm_recv(_eid, fd, *_instanceID, &responseMsg, &responseSize);
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
        lg2::error("pldm_recv failed, rc = {RC}, errno = {ERRNO}", "RC",
                   static_cast<std::underlying_type_t<pldm_requester_rc_t>>(rc),
                   "ERRNO", e);
        status = ResponseStatus::failure;

        responseMsg = nullptr;
    }

    cleanupCmd();

    // Can't use this instance ID anymore.
    freeIID();

    if (status == ResponseStatus::success)
    {
        uint8_t completionCode = 0;
        auto response = reinterpret_cast<pldm_msg*>(responseMsg);

        auto decodeRC = decode_new_file_resp(response, PLDM_NEW_FILE_RESP_BYTES,
                                             &completionCode);
        if (decodeRC < 0)
        {
            lg2::error("decode_new_file_resp failed, rc = {RC}", "RC",
                       decodeRC);
            status = ResponseStatus::failure;
        }
        else
        {
            if (completionCode != PLDM_SUCCESS)
            {
                lg2::error("Bad PLDM completion code {CODE}", "CODE",
                           completionCode);
                status = ResponseStatus::failure;
            }
        }
    }

    callResponseFunc(status);

    if (responseMsg)
    {
        free(responseMsg);
    }
}

void PLDMInterface::receiveTimerExpired()
{
    lg2::error("Timed out waiting for PLDM response");

    // Cleanup, but keep the instance ID because the host didn't
    // respond so we can still use it.
    cleanupCmd();

    callResponseFunc(ResponseStatus::failure);
}

void PLDMInterface::cancelCmd()
{
    freeIID();
    cleanupCmd();
}

void PLDMInterface::cleanupCmd()
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
