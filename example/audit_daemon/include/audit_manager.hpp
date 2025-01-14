#pragma once

#include "audit_config.hpp"
#include "auditd.hpp"

#include <sdbusplus/bus.hpp>
#include <xyz/openbmc_project/Logging/Audit/AuditIPMI/server.hpp>
#include <xyz/openbmc_project/Logging/Audit/Manager/server.hpp>

#include <cstdint>

namespace phosphor
{
namespace logging
{
namespace audit
{

using Config = phosphor::logging::audit::config::Config;

using ManagerIface =
    sdbusplus::xyz::openbmc_project::Logging::Audit::server::Manager;
using AuditIPMIIface =
    sdbusplus::xyz::openbmc_project::Logging::Audit::server::AuditIPMI;

using AuditIfaces = sdbusplus::server::object_t<ManagerIface, AuditIPMIIface>;

using AuditdIface = phosphor::logging::audit::Auditd;

class Manager : public AuditIfaces
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
        AuditIfaces(bus, objPath)
    {
        _config = std::make_unique<Config>();
        AuditIPMIIface::allowList(_config->getAllowList());
    }

    /*
     * @fn notify()
     * @brief sd_bus notify method implementation callback.
     * @details Create an error/event log based on transaction id and
     *          event message.
     *  @param[in] type - The type of the Audit data.
     *  @param[in] requestId - The unique identifier of the journal entry(ies)
     * to be committed.
     *  @param[in] rc - The handler return code.
     *  @param[in] user - The user name.
     *  @param[in] sockaddr - The sockaddr struct.
     *  @param[in] data - The additional data.
     */
    void notify(uint8_t type, uint32_t requestId, int32_t rc, std::string user,
                std::vector<uint8_t> sockaddrStruct,
                std::vector<uint8_t> data) override;

  private:
    std::unique_ptr<AuditdIface> _auditd = std::make_unique<AuditdIface>();
    std::unique_ptr<Config> _config;
};

} // namespace audit
} // namespace logging
} // namespace phosphor
