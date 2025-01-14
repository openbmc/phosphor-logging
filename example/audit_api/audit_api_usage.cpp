#include <getopt.h>
#include <linux/mctp.h>
#include <linux/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/uio.h>

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

using Args = std::unordered_map<std::string, std::string>;

void usage(char** argv)
{
    std::cerr << "Usage: " << argv[0] << " [options]\n";
    std::cerr << "Options:\n";
    std::cerr << "    --help                   Print this menu\n";
    std::cerr << "    --type=<uint8_t>         type of the Audit data\n";
    std::cerr << "    --request=<uint32_t>     request ID\n";
    std::cerr << "    --retcode=<int32_t>      handler return code\n";
    std::cerr << "    --user=<string>          user name\n";

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
    Args args = parseArgs(argc, argv);

    if (!args.contains("type") || !args.contains("request") ||
        !args.contains("retcode") || !args.contains("user"))
    {
        usage(argv);
        exit(-1);
    }

    std::uint8_t type = std::stoul(args["type"].c_str());
    std::uint32_t requestId = std::stoul(args["request"].c_str());
    std::int32_t rc = std::stoi(args["retcode"].c_str());
    std::string user(args["user"]);

    /*
        struct sockaddr_mctp saddr_mctp =
        {
            .smctp_family = AF_MCTP,
            .smctp_addr.s_addr = 0;
            .smctp_type = MCTP_MSG_TYPE_PLDM;
            .smctp_tag = MCTP_TAG_OWNER;
        };
    */

    struct sockaddr_in6 saddr_in6;
    saddr_in6.sin6_family = AF_INET6;
    saddr_in6.sin6_addr = in6addr_any;
    size_t saddr_size = sizeof(struct sockaddr_in6);

    uint8_t test_data[] = {0xfe, 0xfa, 0xaf, 0xef};
    struct iovec iovec_data;
    iovec_data.iov_base = test_data;
    iovec_data.iov_len = sizeof(test_data);

    phosphor::logging::audit::audit_event(
        type, requestId, rc, user, (struct sockaddr*)&saddr_in6, saddr_size,
        &iovec_data);

    return 0;
}
