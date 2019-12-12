#pragma once

#include "host_interface.hpp"

#include <libpldm/pldm.h>

#include <chrono>
#include <memory>
#include <sdeventplus/clock.hpp>
#include <sdeventplus/source/io.hpp>
#include <sdeventplus/utility/timer.hpp>

namespace openpower::pels
{

/**
 * @class PLDMInterface
 *
 * This class handles sending the 'new file available' PLDM
 * command to the host to notify it of a new PEL's ID and size.
 *
 * The command response is asynchronous.
 */
class PLDMInterface : public HostInterface
{
  public:
    PLDMInterface() = delete;
    PLDMInterface(const PLDMInterface&) = default;
    PLDMInterface& operator=(const PLDMInterface&) = default;
    PLDMInterface(PLDMInterface&&) = default;
    PLDMInterface& operator=(PLDMInterface&&) = default;

    /**
     * @brief Constructor
     *
     * @param[in] event - The sd_event object pointer
     * @param[in] dataIface - The DataInterface object
     */
    PLDMInterface(sd_event* event, DataInterfaceBase& dataIface) :
        HostInterface(event, dataIface),
        _receiveTimer(
            event,
            std::bind(std::mem_fn(&PLDMInterface::receiveTimerExpired), this))
    {
        readEID();
    }

    /**
     * @brief Destructor
     */
    ~PLDMInterface();

    /**
     * @brief Kicks off the send of the 'new file available' command
     *        to send up the ID and size of the new PEL.
     *
     * @param[in] id - The PEL ID
     * @param[in] size - The PEL size in bytes
     *
     * @return CmdStatus - the success/fail status of the send
     */
    CmdStatus sendNewLogCmd(uint32_t id, uint32_t size) override;

    /**
     * @brief Cancels waiting for a command response
     */
    void cancelCmd() override;

  private:
    /**
     * @brief The asynchronous callback for getting the response
     *        of the 'new file available' command.
     *
     * Calls the response callback that is registered.
     *
     * @param[in] io - The event source object
     * @param[in] fd - The FD used
     * @param[in] revents - The event bits
     */
    void receive(sdeventplus::source::IO& io, int fd,
                 uint32_t revents) override;

    /**
     * @brief Function called when the receive timer expires.
     *
     * This is considered a failure and so will invoke the
     * registered response callback function with a failure
     * indication.
     */
    void receiveTimerExpired();

    /**
     * @brief Configures the sdeventplus::source::IO object to
     *        call receive() on EPOLLIN activity on the PLDM FD
     *        which is used for command responses.
     */
    void registerReceiveCallback();

    /**
     * @brief Reads the MCTP endpoint ID out of a file
     */
    void readEID();

    /**
     * @brief Opens the PLDM file descriptor
     */
    void open();

    /**
     * @brief Reads the PLDM instance ID to use for the upcoming
     *        command.
     */
    void readInstanceID();

    /**
     * @brief Encodes and sends the PLDM 'new file available' cmd
     *
     * @param[in] id - The PEL ID
     * @param[in] size - The PEL size in bytes
     */
    void doSend(uint32_t id, uint32_t size);

    /**
     * @brief Closes the PLDM file descriptor
     */
    void closeFD();

    /**
     * @brief The MCTP endpoint ID
     */
    mctp_eid_t _eid;

    /**
     * @brief The PLDM instance ID of the current command
     */
    uint8_t _instanceID;

    /**
     * @brief The PLDM command file descriptor for the current command
     */
    int _fd = -1;

    /**
     * @brief The event object for handling callbacks on the PLDM FD
     */
    std::unique_ptr<sdeventplus::source::IO> _source;

    /**
     * @brief A timer to only allow a certain amount of time for the
     *        async PLDM receive before it is considered a failure.
     */
    sdeventplus::utility::Timer<sdeventplus::ClockId::Monotonic> _receiveTimer;

    /**
     * @brief The command timeout value
     */
    const std::chrono::milliseconds _receiveTimeout{10000};
};

} // namespace openpower::pels
