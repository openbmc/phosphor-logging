#include <getopt.h>

#include <phosphor-logging/audit/audit.hpp>
#include <sdbusplus/bus.hpp>
#include <sdeventplus/event.hpp>
#include <xyz/openbmc_project/Logging/Audit/Manager/client.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// example call from console:
// busctl call xyz.openbmc_project.Logging.Audit
//   /xyz/openbmc_project/logging/audit/manager
//     xyz.openbmc_project.Logging.Audit.Manager Notify ttts 144 145 146
//     'test_user'

using Args = std::unordered_map<std::string, std::string>;

void usage(char** argv)
{
    std::cerr << "Usage: " << argv[0] << " [options]\n";
    std::cerr << "Options:\n";
    std::cerr << "    --help                    Print this menu\n";
    std::cerr << "    --type=<uint64_t>         type of the Audit data\n";
    std::cerr << "    --request=<uint64_t>      request ID\n";
    std::cerr << "    --retcode=<uint64_t>      handler return code\n";
    std::cerr << "    --user=<string>           user name\n";

    std::cerr << std::flush;
}

static constexpr auto optionShortForm = "t:r:c:u:h?";
static const option optionLongForm[] = {
    {"ip", required_argument, nullptr, 'i'},
    {"type", required_argument, nullptr, 't'},
    {"request", required_argument, nullptr, 'r'},
    {"retcode", required_argument, nullptr, 'c'},
    {"user", required_argument, nullptr, 'u'},
    {"data", optional_argument, nullptr, 'd'},
    {"help", no_argument, nullptr, 'h'},
    {0, 0, 0, 0},
};

Args parseArgs(int argc, char** argv)
{
    Args args;
    int option = 0;
    while (-1 != (option = getopt_long(argc, argv, optionShortForm,
                                       optionLongForm, nullptr)))
    {
        if ((option == '?') || (option == 'h'))
        {
            usage(argv);
            exit(-1);
        }

        auto i = &optionLongForm[0];
        while ((i->val != option) && (i->val != 0))
        {
            ++i;
        }

        if ((no_argument != i->has_arg) && optarg)
        {
            args[i->name] = optarg;
        }
    }

    return args;
}

int main(int argc, char** argv)
{
    static constexpr auto busName = "xyz.openbmc_project.Logging.Audit";
    static constexpr auto objPath = "/xyz/openbmc_project/logging/audit";
    static constexpr auto objMgr = "/xyz/openbmc_project/logging/audit/manager";

    Args args = parseArgs(argc, argv);

    if (!args.contains("type") || !args.contains("request") ||
        !args.contains("retcode") || !args.contains("user"))
    {
        usage(argv);
        exit(-1);
    }

    std::uint64_t type = std::stoull(args["type"].c_str());
    std::uint64_t requestId = std::stoull(args["request"].c_str());
    std::uint64_t rc = std::stoull(args["retcode"].c_str());
    std::string ip(args["ip"]);
    std::string user(args["user"]);
    std::vector<uint8_t> data;

    phosphor::logging::audit::notify_event(ip, type, requestId, rc, user, data);

    return 0;
}
 