#pragma once

#include "host_interface.hpp"
#include "pel.hpp"
#include "repository.hpp"

#include <deque>

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
     * @param[in] status - The response status
     */
    void commandResponse(ResponseStatus status);

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
};

} // namespace openpower::pels
