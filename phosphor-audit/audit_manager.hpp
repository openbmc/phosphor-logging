#pragma once

#include <sdbusplus/bus.hpp>
#include <vector>
#include <xyz/openbmc_project/Logging/Audit/Manager/server.hpp>

namespace phosphor
{
namespace audit
{

namespace details
{
template <typename... T>
using ServerObject = typename sdbusplus::server::object::object<T...>;

using ManagerIface =
    sdbusplus::xyz::openbmc_project::Logging::Audit::server::Manager;

} // namespace details

class Manager : public details::ServerObject<details::ManagerIface>
{
  public:
    Manager() = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;
    Manager(Manager&&) = delete;
    Manager& operator=(Manager&&) = delete;
    virtual ~Manager() = default;

    /** @brief Constructor to put object onto bus at a dbus path.
     *  @param[in] bus - Bus to attach to.
     *  @param[in] path - Path to attach at.
     */
    Manager(sdbusplus::bus::bus& bus, const char* objPath) :
        details::ServerObject<details::ManagerIface>(bus, objPath){};

    /*
     * @fn notify()
     * @brief sd_bus notify method implementation callback.
     * @details Create an error/event log based on transaction id and
     *          event message.
     * @param[in] transactionId - Unique identifier of the journal entries
     *                            to be committed.
     */
    void notify(uint64_t transactionId) override;

  private:
    /**
     * @fn getAdditionalData()
     * @brief Get additional data for audit log.
     *
     * @param[in] transactionId - Unique identifier of the journal entries
     *                            to be committed.
     *
     * @return  the vector of additional data
     */
    std::vector<std::string> getAdditionalData(uint64_t transactionId);

    /** @brief Synchronize unwritten journal messages to disk.
     *  @details This is the same implementation as the systemd command
     *  "journalctl --sync".
     */
    void journalSync();
};

} // namespace audit
} // namespace phosphor