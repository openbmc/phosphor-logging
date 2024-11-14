/**
 * @brief Phosphor Audit manager processing changes properties and call methods on
 * interfaces
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

using LinuxAuditIface = phosphor::logging::audit::LinuxAudit;

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
   * @param[in] transactionId - Unique identifier of the journal entries
   *                            to be committed.
   */
  void notify(std::uint64_t type, std::uint64_t requestId, std::uint64_t rc,
              std::string user) override;

private:
  std::unique_ptr<LinuxAuditIface> _linuxAudit =
      std::make_unique<LinuxAuditIface>();
  ;
};

} // namespace audit
} // namespace logging
} // namespace phosphor
