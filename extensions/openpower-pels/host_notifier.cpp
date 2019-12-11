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
#include "host_notifier.hpp"

#include <phosphor-logging/log.hpp>

namespace openpower::pels
{

const auto subscriptionName = "PELHostNotifier";

using namespace phosphor::logging;

HostNotifier::HostNotifier(Repository& repo, DataInterfaceBase& dataIface,
                           std::unique_ptr<HostInterface> hostIface) :
    _repo(repo),
    _dataIface(dataIface), _hostIface(std::move(hostIface))
{
    // Subscribe to be told about new PELs.
    _repo.subscribeToAdds(subscriptionName,
                          std::bind(std::mem_fn(&HostNotifier::newLogCallback),
                                    this, std::placeholders::_1));

    // Add any existing PELs to the queue to send them if necessary.
    _repo.for_each(std::bind(std::mem_fn(&HostNotifier::addPELToQueue), this,
                             std::placeholders::_1));

    // Subscribe to be told about host state changes.
    _dataIface.subscribeToHostStateChange(
        subscriptionName,
        std::bind(std::mem_fun(&HostNotifier::hostStateChange), this,
                  std::placeholders::_1));

    // Set the function to call when the async reponse is received.
    _hostIface->setResponseFunction(
        std::bind(std::mem_fn(&HostNotifier::commandResponse), this,
                  std::placeholders::_1));

    // Start sending logs if the host is running
    if (!_pelQueue.empty() && _dataIface.isHostUp())
    {
        doNewLogNotify();
    }
}

HostNotifier::~HostNotifier()
{
    _repo.unsubscribeFromAdds(subscriptionName);
    _dataIface.unsubscribeFromHostStateChange(subscriptionName);
}

bool HostNotifier::addPELToQueue(const PEL& pel)
{
    if (enqueueRequired(pel.id()))
    {
        _pelQueue.push_back(pel.id());
    }

    // Return false so that Repo::for_each keeps going.
    return false;
}

bool HostNotifier::enqueueRequired(uint32_t id) const
{
    bool required = true;
    Repository::LogID i{Repository::LogID::Pel{id}};

    if (auto attributes = _repo.getPELAttributes(i); attributes)
    {
        auto a = attributes.value().get();

        if ((a.hostState == TransmissionState::acked) ||
            (a.hostState == TransmissionState::badPEL))
        {
            required = false;
        }
        else if (a.actionFlags.test(hiddenFlagBit) &&
                 (a.hmcState == TransmissionState::acked))
        {
            required = false;
        }
        else if (a.actionFlags.test(dontReportToHostFlagBit))
        {
            required = false;
        }
    }
    else
    {
        using namespace phosphor::logging;
        log<level::ERR>("Host Enqueue: Unable to find PEL ID in repository",
                        entry("PEL_ID=0x%X", id));
        required = false;
    }

    return required;
}

void HostNotifier::newLogCallback(const PEL& pel)
{
    if (!enqueueRequired(pel.id()))
    {
        return;
    }

    _pelQueue.push_back(pel.id());

    // TODO: Check if a send is needed now
}

void HostNotifier::doNewLogNotify()
{
}

void HostNotifier::hostStateChange(bool hostUp)
{
}

void HostNotifier::commandResponse(ResponseStatus status)
{
}

} // namespace openpower::pels
