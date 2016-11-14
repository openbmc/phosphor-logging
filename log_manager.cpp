#include <fstream>
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <sdbusplus/vtable.hpp>
#include <systemd/sd-bus.h>
#include <systemd/sd-journal.h>
#include "log.hpp"
#include "log_manager.hpp"

namespace phosphor
{
namespace logging
{

/*
 * @fn commit()
 * @brief Create an error/event log based on specified id and metadata variable
 *        names that includes the journal message and the metadata values.
 */
void Manager::commit(int msgid, std::string emsg)
{
    // TODO Change /tmp path to a permanent location on flash
    constexpr const auto path = "/tmp/elog";
    constexpr const auto msgIdStr = "_PID";

    // Create log file
    std::string filename{};
    filename.append(path);
    // TODO Create error logs in their own separate dir once permanent location
    // on flash is determined. Ex: ../msgid/1
    filename.append(msgid);
    std::ofstream efile;
    efile.open(filename);
    efile << "{" << std::endl;

    const char* metaVar = nullptr;
    std::vector<const char*> metaList;

    int rc = -1;
    sd_journal *j = nullptr;
    rc = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (rc < 0)
    {
        logging::log<logging::level::ERR>("Failed to open journal",
                           logging::entry("DESCRIPTION=%s", strerror(-rc)));
        return;
    }

    // Read the journal from the end to get the most recent entry first.
    // The result from the sd_journal_get_data() is of the form VARIABLE=value.
    SD_JOURNAL_FOREACH_BACKWARDS(j)
    {
        const char *data = nullptr;
        size_t length = 0;

        // Search for the msg id
        rc = sd_journal_get_data(j, msgIdStr, (const void **)&data, &length);
        if (rc < 0)
        {
            // Instance not found, continue to next journal entry
            continue;
        }
        std::string result(data);
        if (result.find(msgid) == std::string::npos)
        {
            // Match not found, continue to next journal entry
            continue;
        }

        // Match found, write to file
        // TODO This is a draft format based on the redfish event logs written
        // in json, the final openbmc format is to be determined
        efile << "\t{" << std::endl;
        efile << "\t\"@" << data << "\"," << std::endl;

        // Include the journal message
        rc = sd_journal_get_data(j, "MESSAGE", (const void **)&data, &length);
        if (rc < 0)
        {
            continue;
        }
        efile << "\t\"@" << data << "\"," << std::endl;

        // Search for the metadata variables in the current journal entry
        for (auto i : metaList)
        {
            rc = sd_journal_get_data(j, i, (const void **)&data, &length);
            if (rc < 0)
            {
                // Not found, continue to next metadata variable
                logging::log<logging::level::INFO>("Failed to find metadata",
                                    logging::entry("META_FIELD=%s", i));
                continue;
            }

            // Metatdata variable found, write to file
            efile << "\t\"@" << data << "\"," << std::endl;
        }
        efile << "\t}" << std::endl;

        // TODO Break only once all metadata fields have been found. Implement
        // once this function reads the metadata fields from the header file.
        break;
    }
    sd_journal_close(j);

    efile << "}" << std::endl;
    efile.close();
    return;
}

Manager::Manager(sdbusplus::bus::bus &&bus,
                 const char* busname,
                 const char* root,
                 const char* iface) :
    sdbusplus::server::object_t<sdbusplus::server::xyz::openbmc_project::Logging::Manager>
    _shutdown(false),
    _root(root),
    _bus(std::move(bus)),
    _manager(sdbusplus::server::manager::manager(_bus, root))
{
    _bus.request_name(busname);
}

void Manager::run()
{
    while(true)
    {
        try
        {
            _bus.process_discard();
            _bus.wait();
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}

} // namespace logging
} // namepsace phosphor
