#include <iostream>
#include <elog.hpp>
#include <log.hpp>

using namespace phosphor;
using namespace logging;

int main()
{
    std::cout << "hello world!\n";

    log<level::DEBUG>(
            phosphor::logging::msg("Simple Example"));

    const char *file_name = "HELLO.txt";
    int number = 0xFEFE;

    log<level::DEBUG>(
            msg("THIS IS A PHOSPHOR LOGGING TEST"),
            entry("FILE_NAME=%s_%x", file_name, number));

    number = 0xFFFF;

    // TODO - next task
    //struct FILE_NOT_FOUND_ERROR a;
//    elog(a,
//         (new_entry(ERRNO,99)),
//         (new_entry(FILE_NAME,name))
//         (prev_entry(FILE_NAME)));

    return 0;
}
