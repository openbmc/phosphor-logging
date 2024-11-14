/**
 * @brief Phosphor Audit manager processing changes properties and call methods
 * on interfaces
 *
 * This file is part of the phosphor-audit project.
 *
 */

#pragma once

#include <cstdint>
#include <sdbusplus/bus.hpp>
#include <xyz/openbmc_project/Logging/Audit/Manager/server.hpp>

#include "linux_audit.hpp"

namespace phosphor {
namespace logging {
namespace audit {

namespace details {

template <typename... T>
using ServerObject = typename sdbusplus::server::object::object<T...>;
using ManagerIface =
    sdbusplus::xyz::openbmc_project::Logging::Audit::server::Manager;
} // namespace details

using AuditdIface = phosphor::logging::audit::LinuxAudit;

class Manager : public details::ServerObject<details::ManagerIface> {
public:
  Manager() = delete;
  Manager(const Manager &) = delete;
  Manager &operator=(const Manager &) = delete;
  Manager(Manager &&) = delete;
  Manager &operator=(Manager &&) = delete;
  virtual ~Manager() = default;
  /** @brief Constructor to put object onto bus at a dbus path.
   *  @param[in] bus - Bus to attach to.
   *  @param[in] path - Path to attach at.
   */
  Manager(sdbusplus::bus::bus &bus, const char *objPath)
      : details::ServerObject<details::ManagerIface>(bus, objPath){

        };
  /*
   * @fn notify()
   * @brief sd_bus notify method implementation callback.
   * @details Create an error/event log based on transaction id and
   *          event message.
   *  @param[in] ip - The IP address.
   *  @param[in] type - The type of the Audit data.
   *  @param[in] requestId - The unique identifier of the journal entry(ies) to
   * be committed.
   *  @param[in] rc - The handler return code.
   *  @param[in] user - The user name.
   *  @param[in] data - The additional data.
   */

  void notify(std::string ip, uint64_t type, uint64_t requestId, uint64_t rc,
              std::string user, std::vector<uint8_t> data) override;

private:
  std::unique_ptr<AuditdIface> _linuxAudit = std::make_unique<AuditdIface>();
  ;
};

} // namespace audit
} // namespace logging
} // namespace phosphor
