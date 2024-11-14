#include <iostream>
#include <sdbusplus/bus.hpp>
#include <sdeventplus/event.hpp>
#include <phosphor-logging/audit/audit.hpp>
#include <xyz/openbmc_project/Logging/Audit/Manager/client.hpp>


// example call from console:
// busctl call xyz.openbmc_project.Logging.Audit
//   /xyz/openbmc_project/logging/audit/manager
//     xyz.openbmc_project.Logging.Audit.Manager Notify ttts 144 145 146 'test_user'


int main(int argc, char *argv[]) {

  static constexpr auto busName = "xyz.openbmc_project.Logging.Audit";
  static constexpr auto objPath = "/xyz/openbmc_project/logging/audit";
  static constexpr auto objMgr = "/xyz/openbmc_project/logging/audit/manager";

  if (argc != 5) {
    std::cout << "Usage: audit_api_usage type requestId rc user" << std::endl;
    return -1;
  }

  std::uint64_t type = std::stoull(argv[1]);
  std::uint64_t requestId = std::stoull(argv[2]);
  std::uint64_t rc = std::stoull(argv[3]);
  std::string user(argv[4]);

  phosphor::logging::audit::notify_event(type, requestId, rc, user);

  return 0;
}