#pragma once

#include "../bcd_time.hpp"
#include "../paths.hpp"
#include "../pel.hpp"
#include "../private_header.hpp"
#include "../repository.hpp"
#include "../section.hpp"
#include "../stream.hpp"
#include "../user_header.hpp"
#include "samplepel.hpp"

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
 * PELTool: command line tool for viewing PELs.
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
    explicit PELTool(const PEL&)

    {
    }

  private:
};

} // namespace pels
} // namespace openpower
