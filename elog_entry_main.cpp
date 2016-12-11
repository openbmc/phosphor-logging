#include <iostream>
#include "config.h"
#include "elog_entry.hpp"

int main(int argc, char *argv[])
{
    auto bus = sdbusplus::bus::new_default();

    bus.request_name(BUSNAME_ENTRY);

    while(true)
    {
        bus.process_discard();
        bus.wait();
    }
    return 0;
}
