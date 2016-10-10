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
            entry("FILE_NAME_TEST=%s_%x", file_name, number));

    number = 0x1234;
    struct FILE_NOT_FOUND_ERROR a;

    // This one works fine, log shows up in journal
    //elog(a);

    // This one seg faults
    //elog(a,
    //    (new_entry(ERRNO,number)));
    // Debug Traces
    //elog:new_entry: About to make the new_entry ERRNO=%d!
    //log:entry: About to make the entry!
    //elog: About to make the log for File was not found and that is bad!
    //log: About to make the log!
    //log: About to make the details!
    //log: details:log start!
    //log: details:log we got the size of 6
    //Segmentation fault (core dumped)

    // This next test passes but the log does not show up in the journal
    // and neither does the FILE_PATH entry.
    const char *file_path = "/tmp/wish_this_worked.txt";
    elog(a,
        (new_entry(FILE_PATH,file_path)));

    return 0;
}
