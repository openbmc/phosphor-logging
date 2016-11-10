// A basic unit test that runs on a BMC (qemu or hardware)

#include <iostream>
#include <systemd/sd-journal.h>
#include <sstream>
#include "elog.hpp"
#include "log.hpp"
#include "elog-gen.hpp"

using namespace phosphor;
using namespace logging;

// validate the journal metadata equals the input value
int validate_journal(const char *i_entry, const char *i_value)
{
    sd_journal *journal;
    const void *data;
    size_t l;
    int rc;
    bool validated = false;

    rc = sd_journal_open(&journal, SD_JOURNAL_LOCAL_ONLY);
    if (rc < 0) {
            std::cerr << "Failed to open journal: " << strerror(-rc) << "\n";
            return 1;
    }
    rc = sd_journal_query_unique(journal, i_entry);
    if (rc < 0) {
            std::cerr << "Failed to query journal: " << strerror(-rc) << "\n";
            return 1;
    }
    SD_JOURNAL_FOREACH_UNIQUE(journal, data, l)
    {
        std::string journ_entry((const char*)data);
        std::cout << journ_entry << "\n";
        if(journ_entry.find(i_value) != std::string::npos)
        {
            std::cout << "We found it!\n";
            validated = true;
            break;
        }
    }

    sd_journal_close(journal);

    rc = (validated) ? 0 : 1;
    if(rc)
    {
        std::cerr << "Failed to find " << i_entry << " with value " << i_value
                  <<" in journal!" << "\n";
    }

    return rc;
}

int main()
{
    // TEST 1 - Basic log
    log<level::DEBUG>("Basic phosphor logging test");

    // TEST 2 - Log with metadata field
    const char *file_name = "phosphor_logging_test.txt";
    int number = 0xFEFE;
    log<level::DEBUG>("phosphor logging test with attribute",
            entry("FILE_NAME_WITH_NUM_TEST=%s_%x", file_name, number));

    // Now read back and verify our data made it into the journal
    int rc = validate_journal("FILE_NAME_WITH_NUM_TEST",
                              "phosphor_logging_test.txt_fefe");
    if(rc)
        return(rc);

    // TEST 3 - Create error log with 2 meta data fields (rvalue and lvalue)
    number = 0x1234;
    const char *test_string = "/tmp/test_string/";
    try
    {
        elog<org::freedesktop::DBus::Error::FileNotFound>(
                org::freedesktop::DBus::Error::FileNotFound::
                    ERRNUM(number),
                org::freedesktop::DBus::Error::FileNotFound::
                    FILE_PATH(test_string),
                org::freedesktop::DBus::Error::FileNotFound::
                    FILE_NAME("elog_test_3.txt"));
    }
    catch (elogException<org::freedesktop::DBus::Error::FileNotFound>& e)
    {
        std::cout << "elog exception caught: " << e.what() << std::endl;
    }

    // Reduce our error namespaces
    using namespace org::freedesktop::DBus::Error;

    // Now read back and verify our data made it into the journal
    std::stringstream stream;
    stream << std::hex << number;
    rc = validate_journal(FileNotFound::ERRNUM::str_short,
                          std::string(stream.str()).c_str());
    if(rc)
        return(rc);

    rc = validate_journal(FileNotFound::FILE_PATH::str_short,
                          test_string);
    if(rc)
        return(rc);

    rc = validate_journal(FileNotFound::FILE_NAME::str_short,
                          "elog_test_3.txt");
    if(rc)
        return(rc);

    // TEST 4 - Create error log with previous entry use
    number = 0x9876;
    try
    {
        elog<FileNotFound>(FileNotFound::ERRNUM(number),
                             prev_entry<FileNotFound::FILE_PATH>(),
                             FileNotFound::FILE_NAME("elog_test_4.txt"));
    }
    catch (elogExceptionBase& e)
    {
        std::cout << "elog exception caught: " << e.what() << std::endl;
    }

    // Now read back and verify our data made it into the journal
    stream.str("");
    stream << std::hex << number;
    rc = validate_journal(FileNotFound::ERRNUM::str_short,
                          std::string(stream.str()).c_str());
    if(rc)
        return(rc);

    // This should just be equal to what we put in test 3
    rc = validate_journal(FileNotFound::FILE_PATH::str_short,
                          test_string);
    if(rc)
        return(rc);

    rc = validate_journal(FileNotFound::FILE_NAME::str_short,
                          "elog_test_4.txt");
    if(rc)
        return(rc);

    // Compile fail tests

    // Simple test to prove we fail to compile due to missing param
    //elog<FileNotFound>(FileNotFound::ERRNUM(1),
    //                   FileNotFound::FILE_PATH("test"));

    // Simple test to prove we fail to compile due to invalid param
    //elog<FileNotFound>(FileNotFound::ERRNUM(1),
    //                   FileNotFound::FILE_PATH("test"),
    //                   FileNotFound::FILE_NAME(1));

    return 0;
}


