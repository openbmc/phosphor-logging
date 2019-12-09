#pragma once
#include "host_interface.hpp"
#include "pel.hpp"
#include "repository.hpp"

#include <deque>
#include <sdeventplus/clock.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/source/event.hpp>
#include <sdeventplus/utility/timer.hpp>

namespace openpower
{
namespace pels
{

/**
 * @class HostNotifier
 *
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
     * @return size_t - The queue size
     */
    size_t queueSize() const
    {
        return _pelQueue.size();
    }

    void ackPEL(uint32_t id);

    void setHostFull(uint32_t id);

    void setBadPEL(uint32_t id);

    /**
     * @brief Specifies if the PEL needs to go onto the queue to be
     *        set to the host.
     *
     * Only returns false if:
     *  - Already acked by the host (or they didn't like it)
     *  - Hidden and the HMC already got it
     *  - The 'do not report to host' bit is set
     *
     *  @param[in] id - The PEL ID
     *
     *  @return bool - If the enqueue is required
     */
    bool enqueueRequired(uint32_t id) const;

    /**
     * @brief If the host still needs to be notified of the PEL
     *        at the time of the notification.
     *
     * Only returns false if:
     *  - Already acked by the host
     *  - It's hidden, and the HMC already got or will get it.
     *
     *  @param[in] pelID - The PEL ID
     *
     *  @return bool - If the notify is required
     */
    bool notifyRequired(uint32_t id) const;

  private:
    void newLogCallback(const PEL& pel);

    bool addPELToQueue(const PEL& pel);

    void doNewLogNotify();

    void dispatch(sdeventplus::source::EventBase& source);

    void hostStateChange(bool hostUp);

    void commandResponse(ResponseStatus status);

    void retryTimerExpired();

    void hostFullTimerExpired();

    void stopCommand();

    /**
     * @brief The PEL repository object
     */
    Repository& _repo;

    /**
     * @brief The data interface object
     */
    DataInterfaceBase& _dataIface;

    /**
     * @brief Base class pointer for the host command interfaces.
     */
    std::unique_ptr<HostInterface> _hostIface;

    /**
     * @brief The list of PEL IDs that need to be sent.
     */
    std::deque<uint32_t> _pelQueue;

    /**
     * @brief The list of IDs that were sent, but not acked yet.
     */
    std::vector<uint32_t> _sentPELs;

    /**
     * @brief The ID the PEL where there there notification
     *        has been kicked off but the asynchronous response
     *        hasn't been received yet.
     *
     */
    uint32_t _inProgressPEL = 0;

    /**
     * @brief The command retry count
     */
    size_t _retryCount = 0;

    /**
     * @brief Indicates if the host has said it is full and does not
     *        currently have the space for more PELs.
     */
    bool _hostFull = false;

    /**
     * @brief The command retry timer.
     */
    sdeventplus::utility::Timer<sdeventplus::ClockId::Monotonic> _retryTimer;

    /**
     * @brief The host full timer, used to retry sending a PEL if the host
     *        said it is full and hasn't come down with an ack yet, which
     *        it it does when it frees up space.
     */
    sdeventplus::utility::Timer<sdeventplus::ClockId::Monotonic> _hostFullTimer;

    std::unique_ptr<sdeventplus::source::Defer> _dispatcher;
};

} // namespace pels
} // namespace openpower
