#pragma once

#include "host_interface.hpp"
#include "pel.hpp"
#include "repository.hpp"

#include <deque>
#include <sdeventplus/clock.hpp>
#include <sdeventplus/utility/timer.hpp>

namespace openpower::pels
{

/**
 * @class HostNotifier
 *
 * This class handles notifying the host firmware of new PELs.
 */
class HostNotifier
{
  public:
    HostNotifier() = delete;
    HostNotifier(const HostNotifier&) = delete;
    HostNotifier& operator=(const HostNotifier&) = delete;
    HostNotifier(HostNotifier&&) = delete;
    HostNotifier& operator=(HostNotifier&&) = delete;

    /**
     * @brief Constructor
     *
     * @param[in] repo - The PEL repository object
     * @param[in] dataIface - The data interface object
     * @param[in] hostIface - The host interface object
     */
    HostNotifier(Repository& repo, DataInterfaceBase& dataIface,
                 std::unique_ptr<HostInterface> hostIface);

    /**
     * @brief Destructor
     */
    ~HostNotifier();

    /**
     * @brief Returns the PEL queue size.
     *
     * For testing.
     *
     * @return size_t - The queue size
     */
    size_t queueSize() const
    {
        return _pelQueue.size();
    }

    /**
     * @brief Specifies if the PEL needs to go onto the queue to be
     *        set to the host.
     *
     * Only returns false if:
     *  - Already acked by the host (or they didn't like it)
     *  - Hidden and the HMC already got it
     *  - The 'do not report to host' bit is set
     *
     * @param[in] id - The PEL ID
     *
     * @return bool - If enqueue is required
     */
    bool enqueueRequired(uint32_t id) const;

  private:
    /**
     * @brief This function gets called by the Repository class
     *        when a new PEL is added to it.
     *
     * @param[in] pel - The new PEL
     */
    void newLogCallback(const PEL& pel);

    /**
     * @brief This function runs on every existing PEL at startup
     *        and puts the PEL on the queue to send if necessary.
     *
     * @param[in] pel - The PEL
     *
     * @return bool - This is an indicator to the Repository::for_each
     *                function to traverse every PEL.  Always false.
     */
    bool addPELToQueue(const PEL& pel);

    /**
     * @brief Takes the PEL off the front of the queue and issues
     *        the PLDM send.
     */
    void doNewLogNotify();

    /**
     * @brief Called when the host changes state.
     *
     * @param[in] hostUp - The new host state
     */
    void hostStateChange(bool hostUp);

    /**
     * @brief The callback function invoked after the asynchronous
     *        PLDM receive function is complete.
     *
     * If the command was successful, the state of that PEL will
     * be set to 'sent', and the next send will be triggered.
     *
     * If the command failed, a retry timer will be started so it
     * can be sent again.
     *
     * @param[in] status - The response status
     */
    void commandResponse(ResponseStatus status);

    /**
     * @brief The function called when the command failure retry
     *        time is up.
     *
     * It will issue a send of the previous PEL and increment the
     * retry count.
     */
    void retryTimerExpired();

    /**
     * @brief The PEL repository object
     */
    Repository& _repo;

    /**
     * @brief The data interface object
     */
    DataInterfaceBase& _dataIface;

    /**
     * @brief Base class pointer for the host command interface
     */
    std::unique_ptr<HostInterface> _hostIface;

    /**
     * @brief The list of PEL IDs that need to be sent.
     */
    std::deque<uint32_t> _pelQueue;

    /**
     * @brief The list of IDs that were sent, but not acked yet.
     *
     * These move back to _pelQueue on a power off.
     */
    std::vector<uint32_t> _sentPELs;

    /**
     * @brief The ID the PEL where the notification has
     *        been kicked off but the asynchronous response
     *        hasn't been received yet.
     */
    uint32_t _inProgressPEL = 0;

    /**
     * @brief The command retry count
     */
    size_t _retryCount = 0;

    /**
     * @brief The command retry timer.
     */
    sdeventplus::utility::Timer<sdeventplus::ClockId::Monotonic> _retryTimer;
};

} // namespace openpower::pels
