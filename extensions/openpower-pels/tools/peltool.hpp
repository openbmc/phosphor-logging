#pragma once

#include "../section.hpp"
#include "../stream.hpp"
#include "../bcd_time.hpp"
#include "../private_header.hpp"
#include "../user_header.hpp"
#include "../pel.hpp"
#include "../repository.hpp"
#include "../bcd_time.hpp"
#include "../paths.hpp"
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
 * @class pelTool
 *
 * pelTool class handels operations like parsing PELs.
 */
class PelTool
{
  public:
    PelTool() = delete;
    ~PelTool() = default;
    PelTool(const PelTool&) = default;
    PelTool& operator=(const PelTool&) = default;
    PelTool(PelTool &&) = default;
    PelTool& operator=(PelTool&&) = default;

    /**
     * @brief Constructor
     *
     * Fills in this class's data fields from the stream.
     *
     * @param[in] pel - the PEL data stream
     */
    explicit PelTool(string _cmd) :
        _repo(getPELRepoPath())
 
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
    //char* PelTool::parse(const Repository&);
    friend Stream& operator>>(Stream& s, PrivateHeader& ph);
    friend Stream& operator<<(Stream& s, PrivateHeader& ph);
    friend Stream& operator>>(Stream& s, UserHeader& uh);
    friend Stream& operator<<(Stream& s, UserHeader& uh);
  private:
  //std::optional<std::vector<uint8_t>> Repository::getPELData(const LogID& id)
     /**
     * @brief parse a raw PEL into human readable strings
     *
     * @param[in] rawPelPath - The path to the file that contains the
     *                         raw PEL.
     * @param[in] obmcLogID - the corresponding OpenBMC event log id
     */
  // void parseRawPEL(const std::string& rawPelPath, uint32_t obmcLogID);	

   std::vector<uint8_t> data;

   //Repository _repo; 
   std::stringstream ss;
};


} // namespace pelTool
} // namespace openpower
