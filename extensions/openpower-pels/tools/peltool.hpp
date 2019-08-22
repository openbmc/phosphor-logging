#pragma once

#include "../bcd_time.hpp"
#include "../paths.hpp"
#include "../pel.hpp"
#include "../private_header.hpp"
#include "../repository.hpp"
#include "../section.hpp"
#include "../stream.hpp"
#include "../user_header.hpp"
#include "pelparser.hpp"
#include "peltool_utility.hpp"

#include <bitset>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

namespace openpower
{
namespace pels
{

/**
 * @class PELTool
 *
 * PELTool class handels operations like parsing PELs.
 */
class PELTool
{
  public:
    PELTool() = delete;
    ~PELTool() = default;
    PELTool(const PELTool&) = default;
    PELTool& operator=(const PELTool&) = default;
    PELTool(PELTool&&) = default;
    PELTool& operator=(PELTool&&) = default;

    /**
     * @brief Constructor
     *
     * Fills in this class's data fields from the stream.
     *
     * @param[in] pel - the PEL data stream
     */
    explicit PELTool(string _cmd) : _repo(getPELRepoPath()), _parser("standard")

    {
    }
    Repository _repo;
    PELParser _parser;

    string test;
    /**
     * @brief parse
     *
     * parse PEL to human readable form
     *
     */
    // char* PelTool::parse(const Repository&);
    friend Stream& operator>>(Stream& s, PrivateHeader& ph);
    friend Stream& operator<<(Stream& s, PrivateHeader& ph);
    friend Stream& operator>>(Stream& s, UserHeader& uh);
    friend Stream& operator<<(Stream& s, UserHeader& uh);

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
    void parseRawPEL(const std::string& rawPelPath, uint32_t obmcLogID);

    std::vector<uint8_t> data;

    // Repository _repo;
    std::stringstream ss;
};

} // namespace pels
} // namespace openpower
