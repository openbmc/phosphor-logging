#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/manager.hpp>
#include <fstream>
#include <memory>
#include "config.h"
#include "log_manager.hpp"

int main(int argc, char *argv[])
{
    auto bus = sdbusplus::bus::new_default();

    // Add sdbusplus ObjectManager for the 'root' path of the logging manager.
    sdbusplus::server::manager::manager objManager(bus, OBJ_LOGGING);

    phosphor::logging::Manager manager(bus, OBJ_INTERNAL);

    bus.request_name(BUSNAME_LOGGING);

    std::ifstream is("/tmp/out.cereal", std::ios::in|std::ios::binary);
    if(is.is_open())
    {
        phosphor::logging::AssociationList empty{};
        std::vector<std::string> data{};
        auto e = std::make_unique<phosphor::logging::Entry>(
                     bus,
                     std::string(OBJ_ENTRY) + "/1",
                     1,
                     0,
                     phosphor::logging::Entry::Level::Informational,
                     "",
                     std::move(data),
                     std::move(empty),
                     manager);
        cereal::BinaryInputArchive iarchive(is);
        iarchive(*e);
        manager.entries.insert(std::make_pair(1, std::move(e)));
    }

    while(true)
    {
        bus.process_discard();
        bus.wait();
    }

    return 0;
}
