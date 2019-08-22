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
#include "extensions/openpower-pels/paths.hpp"
#include "extensions/openpower-pels/repository.hpp"
#include "extensions/openpower-pels/user_header.hpp"
#include "test/openpower-pels/pel_utils.hpp"

#include <CLI/CLI.hpp>
#include <bitset>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iostream> // std::cout
#include <iostream>
#include <iterator>
#include <map>
#include <phosphor-logging/log.hpp>
#include <sstream> // std::stringstream
#include <sstream>
#include <string> // std::string
#include <string>

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
