#include <iostream>
#include "elog.hpp"
#include "log.hpp"

using namespace phosphor;
using namespace logging;

int main()
{
    std::cout << "hello world!\n";

    // Simple elog test
    elog<file_not_found>(file_not_found::errnum(1),
                         file_not_found::file_path("/tmp"),
                         file_not_found::file_name("logging_test.txt"));

    // pass in variables and empty constructor to indicate use of prev entry
    const char *test_string = "/tmp/test_string/";
    elog<file_not_found>(file_not_found::errnum(2),
                         file_not_found::file_path(test_string),
                         file_not_found::file_name());

    // Simple test to prove we fail to compile due to missing param
    //elog<file_not_found>(file_not_found::errnum(1),
    //                     file_not_found::file_path("test"));

    // Simple test to prove we fail to compile due to invalid param
    //elog<file_not_found>(file_not_found::errnum(1),
    //                     file_not_found::file_path("test"),
    //                     file_not_found::file_name(1));

    // Log tests
    log<level::DEBUG>(
            phosphor::logging::msg("Simple Example"));

    const char *file_name = "HELLO.txt";
    int number = 0xFEFE;

    log<level::DEBUG>(
            msg("THIS IS A PHOSPHOR LOGGING TEST"),
            entry("FILE_NAME=%s_%x", file_name, number));

    return 0;
}
