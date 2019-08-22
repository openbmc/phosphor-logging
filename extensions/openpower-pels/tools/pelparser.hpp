#pragma once

#include "../bcd_time.hpp"
#include "../paths.hpp"
#include "../pel.hpp"
#include "../pel_values.hpp"
#include "../private_header.hpp"
#include "../repository.hpp"
#include "../section.hpp"
#include "../stream.hpp"
#include "../user_header.hpp"
#include "test/openpower-pels/pel_utils.hpp"

#include <CLI/CLI.hpp>
#include <bitset>
#include <filesystem>
#include <fstream>
#include <iostream> // std::cout
#include <iterator>
#include <map>
#include <phosphor-logging/log.hpp>
#include <sstream> // std::stringstream
#include <string> // std::string

using namespace std;

namespace openpower
{
namespace pels
{

/**
 * @class pelParser
 *
 * pelParser class handels operations like parsing PELs.
 */
class PelParser
{
  public:
    PelParser() = delete;
    ~PelParser() = default;
    PelParser(const PelParser&) = default;
    PelParser& operator=(const PelParser&) = default;
    PelParser(PelParser&&) = default;
    PelParser& operator=(PelParser&&) = default;

    /**
     * @brief Constructor
     *
     * Fills in this class's data fields from the stream.
     *
     * @param[in] pel - the PEL data stream
     */
    explicit PelParser(string _cmd) : _repo(getPELRepoPath())

    {
    }
    Repository _repo;
    string test;
    /**
     * @brief parse
     *
     * parse pel to human readable form
     *
     */
    // char* PelTool::parse(const Repository&);

    /**
     *  @brief Translate & display failing subsystem
     *
     *  The failing subsystem is translated from a table and
     *  then output using a standard message string.
     *
     *  @param  i_val
     *      Subsystem identifier
     *
     *  @return None
     *
     */
    void PrintFailingSubSystem(uint8_t i_val);

    /**
     *  @brief Translate & display the log severity
     *
     *  The severity is translated from a table and
     *  then output using a standard message string.
     *
     *  @param  i_val
     *      Severity
     *
     *  @return None
     *
     */
    void PrintSeverity(uint8_t i_val);
    /**
     *  @brief Translate & display the log domain of subsystem
     *
     *  The Domain is translated from a table and
     *  then output using a standard message string.
     *
     *  @param  i_val
     *      Domain
     *
     *  @return None
     *
     */
    void PrintDomain(uint8_t i_val);

    /**
     *  @brief Translate & display the log vector of subsystem
     *
     *  The vector is translated from a table and
     *  then output using a standard message string.
     *
     *  @param  i_val
     *      Vector
     *
     *  @return None
     *
     */
    void PrintVector(uint8_t i_val);

    /**
     *  @brief Make strings look prity when printed.
     *
     *
     *  @param  i_label
     *        char *
     *
     *  @return i_string
     *        char *
     *
     */
    void PrintString(const char* i_label, const char* i_string);

    /**
     *  @brief My helper function to print user headers.
     *
     *
     *  @param iv_ssid
     *        uint8_t
     *  @param iv_severity
     *        uint8_t
     *  @param iv_etype
     *        uint8_t
     *  @param iv_actions
     *        uint8_t
     *
     */
    void printUH(uint8_t iv_ssid, uint8_t iv_severity, uint8_t iv_etype,
                 uint8_t iv_actions);

    /*****************************************************************************/
    // Send the label & return # of chars printed
    /*****************************************************************************/
    static int PrintLabel(const char* i_label)
    {
        if (!i_label)
        {
            i_label = "";
        }

        return printf("| %-25.25s: ", i_label);
    }

  private:
    // std::optional<std::vector<uint8_t>> Repository::getPELData(const LogID&
    // id)
    /**
     * @brief parse a raw PEL into human readable strings
     *
     * @param[in] rawPelPath - The path to the file that contains the
     *                         raw PEL.
     * @param[in] obmcLogID - the corresponding OpenBMC event log id
     */
    // void parseRawPEL(const std::string& rawPelPath, uint32_t obmcLogID);

    std::vector<uint8_t> data;

    // Repository _repo;
    std::stringstream ss;
};
} // namespace pels
} // namespace openpower
