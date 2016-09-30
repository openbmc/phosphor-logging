#include <iostream>
#include "elog.hpp"
#include "log.hpp"

using namespace phosphor;
using namespace logging;

int main()
{
    std::cout << "hello world!\n";

    // Simple elog test
    const char *test_string = "/tmp/test_string/";
    elog<file_not_found>(file_not_found::errnum(1),
                         file_not_found::file_path(test_string),
                         file_not_found::file_name("elog_test_1.txt"));

    log<level::DEBUG>("Info trace to log file path",
                      entry(file_not_found::file_path::str,
                            "/tmp/log_file_test/"));

    // pass parameter and previous_entry
    elog<file_not_found>(file_not_found::errnum(2),
                         prev_entry<file_not_found::file_path>(),
                         file_not_found::file_name("elog_test_2.txt"));

    // Simple test to prove we fail to compile due to missing param
    //elog<file_not_found>(file_not_found::errnum(1),
    //                     file_not_found::file_path("test"));

    // Simple test to prove we fail to compile due to invalid param
    //elog<file_not_found>(file_not_found::errnum(1),
    //                     file_not_found::file_path("test"),
    //                     file_not_found::file_name(1));

    // Log tests
    log<level::DEBUG>("Simple Example");

    const char *file_name = "HELLO.txt";
    int number = 0xFEFE;

    log<level::DEBUG>("THIS IS A PHOSPHOR LOGGING TEST",
            entry("FILE_NAME=%s_%x", file_name, number));

    return 0;
}
