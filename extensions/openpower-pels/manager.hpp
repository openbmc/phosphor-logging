#include "elog_entry.hpp"
#include "sdbusplus/bus.hpp"

#include <iostream>

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

class Manager
{
  public:
    // TODO: Could use a global instead of a singleton
    Manager(sdbusplus::bus::bus& bus) : _bus(bus)
    {
    }

    static void createManager(sdbusplus::bus::bus& bus)
    {
        std::cerr << "Startup\n";
        if (!_mgr)
        {
            _mgr = std::make_unique<Manager>(bus);
        }
    }

    static std::unique_ptr<Manager>& get()
    {
        return _mgr;
    }

    std::vector<std::uint32_t> create(const Entry& entry,
                                      const std::string& json)
    {
        std::cerr << "Create Log " << entry.id() << "\n";
        return {};
    }

    void erase(std::uint32_t id)
    {
        std::cerr << "Erase " << id << "\n";
    }

  private:
    static std::unique_ptr<Manager> _mgr;
    sdbusplus::bus::bus& _bus;
};

} // namespace pels
} // namespace openpower
