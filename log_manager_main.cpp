#include "config.h"
#include "log_manager.hpp"
#include <sdbusplus/bus.hpp>
#include <cstdlib>
#include <iostream>
#include <exception>

int main(int argc, char *argv[])
{
    try {
        auto manager = phosphor::logging::Manager(
                sdbusplus::bus::new_system(),
                BUSNAME,
                OBJ);
        manager.run();
        exit(EXIT_SUCCESS);
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    exit(EXIT_FAILURE);
}
