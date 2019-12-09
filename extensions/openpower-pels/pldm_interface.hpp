#pragma once

#include "host_interface.hpp"

#include <libpldm/pldm.h>

#include <chrono>
#include <memory>
#include <sdeventplus/clock.hpp>
#include <sdeventplus/source/io.hpp>
#include <sdeventplus/utility/timer.hpp>

namespace openpower
{
namespace pels
{

/**
 * @class PLDMInterface
 *
 */
class PLDMInterface : public HostInterface
{
  public:
    PLDMInterface() = delete;
    PLDMInterface(const PLDMInterface&) = default;
    PLDMInterface& operator=(const PLDMInterface&) = default;
    PLDMInterface(PLDMInterface&&) = default;
    PLDMInterface& operator=(PLDMInterface&&) = default;

    PLDMInterface(sd_event* event, DataInterfaceBase& dataIface) :
        HostInterface(event, dataIface),
        _receiveTimer(
            event,
            std::bind(std::mem_fn(&PLDMInterface::receiveTimerExpired), this))
    {
    }

    ~PLDMInterface();

    CmdStatus sendNewLogCmd(uint32_t id, uint32_t size) override;

    void cancelCmd() override;

  private:
    void receive(sdeventplus::source::IO& io, int fd,
                 uint32_t revents) override;

    void receiveTimerExpired();

    void registerReceiveCallback();

    void readEID();

    void open();

    void readInstanceID();

    void doSend(uint32_t id, uint32_t size);

    void closeFD();

    std::optional<mctp_eid_t> _eid;

    uint8_t _instanceID;

    int _fd = -1;

    std::unique_ptr<sdeventplus::source::IO> _source;

    sdeventplus::utility::Timer<sdeventplus::ClockId::Monotonic> _receiveTimer;

    const std::chrono::milliseconds _receiveTimeout{10000};
};

} // namespace pels
} // namespace openpower
