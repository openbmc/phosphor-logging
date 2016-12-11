#include <cstdlib>
#include <exception>
#include <iostream>
#include "config.h"
#include "elog_entry.hpp"

int main(int argc, char *argv[])
{
    auto bus = sdbusplus::bus::new_system();
    auto manager = phosphor::logging::Entry(
                    bus,
                    BUSNAME_ENTRY,
                    OBJ_LOGGING);

    bus.request_name(BUSNAME_ENTRY);

    while(true)
    {
        try
        {
            bus.process_discard();
            bus.wait();
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}
