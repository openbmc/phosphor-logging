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

#include <CLI/CLI.hpp>
#include <bitset>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <phosphor-logging/log.hpp>
#include <sstream>
#include <string>

namespace openpower
{
namespace pels
{

/**
 * @class PELParser
 *
 * PELParser class handles operations like parsing PELs.
 */
class PELParser
{
  public:
    PELParser() = delete;
    ~PELParser() = default;
    PELParser(const PELParser&) = default;
    PELParser& operator=(const PELParser&) = default;
    PELParser(PELParser&&) = default;
    PELParser& operator=(PELParser&&) = default;

    /**
     * @brief Constructor
     *
     *
     */
    explicit PELParser(std::string _cmd) : _repo(getPELRepoPath())

    {
    }
    /**
     *  @brief Translate & get failing subsystem
     *
     *  The failing subsystem is translated from a table and
     *  then output using a standard message string.
     *
     *  @param  i_val
     *      Subsystem identifier
     *
     *  @return const char*
     *
     */
    const char* getFailingSubSystem(uint8_t i_val);

    /**
     *  @brief Translate & display failing subsystem
     *
     *  The failing subsystem is translated from a table and
     *  then output using a standard message string.
     *
     *  @param[in] i_val - subsystem identifier
     *  @return None
     *
     */
    void printFailingSubSystem(uint8_t i_val);

    /**
     *  @brief Translate & display the log severity
     *
     *  The severity is translated from a table and
     *  then output using a standard message string.
     *
     *  @param  i_val
     *     Severity identifier
     *
     *  @return none
     *
     */
    void printSeverity(uint8_t i_val);

    /**
     *  @brief Translate & get the log severity
     *
     *  The severity is translated from a table and
     *  then output using a standard message string.
     *
     *  @param  i_val
     *     Severity identifier
     *
     *  @return  const char*
     *
     */
    const char* getSeverity(uint8_t i_val);

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
    void printDomain(uint8_t i_val);

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
    void printVector(uint8_t i_val);

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
    void printString(const char* i_label, const char* i_string);

    /**
     *  @brief Helper function printing user headers.
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

    /**
     *  @brief Helper function printing user headers.
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
    static int printLabel(const char* i_label)
    {
        if (!i_label)
        {
            i_label = "";
        }

        return printf("| %-25.25s: ", i_label);
    }

  private:
    Repository _repo;
};
} // namespace pels
} // namespace openpower
