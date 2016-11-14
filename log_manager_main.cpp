#include "config.h"
#include "log_manager.hpp"

int main(int argc, char *argv[])
{
    try {
        auto manager = phosphor::logging::manager::Manager(
                sdbusplus::bus::new_system(),
                BUSNAME,
                INVENTORY_ROOT,
                IFACE);
        manager.run();
        exit(EXIT_SUCCESS);
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    exit(EXIT_FAILURE);
}
