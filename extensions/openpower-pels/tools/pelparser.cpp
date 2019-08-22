#include "pelparser.hpp"

#include "../pel.hpp"
#include "extensions/openpower-pels/paths.hpp"
#include "extensions/openpower-pels/repository.hpp"
#include "extensions/openpower-pels/tools/peltool_utility.hpp"
#include "extensions/openpower-pels/user_header.hpp"
#include "peltool_utility.hpp"
#include "test/openpower-pels/pel_utils.hpp"

#include <CLI/CLI.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iostream> // std::cout
#include <iterator>
#include <map>
#include <phosphor-logging/log.hpp>
#include <sstream> // std::stringstream
#include <string>  // std::string

using namespace phosphor::logging;
namespace fs = std::filesystem;
using namespace openpower::pels;

namespace openpower
{
namespace pels
{

/*****************************************************************************/
// Failing SubSystem
/*****************************************************************************/
void PelParser::PrintFailingSubSystem(uint8_t i_val)
{
    //    PrintTableItem((uint32_t)i_val, ErrlSubSysTable, ErrlSubSysTableSz);
}

/*****************************************************************************/
// Severity
/*****************************************************************************/
void PelParser::PrintSeverity(uint8_t i_val)
{
    //    PrintTableItem((uint32_t)i_val, ErrlSevTable, ErrlSevTableSz);
}

/*****************************************************************************/
// Domain
/*****************************************************************************/
void PelParser::PrintDomain(uint8_t i_val)
{
    //    PrintTableItem((uint32_t)i_val, ErrlDomainTable, ErrlDomainTableSz);
}

/*****************************************************************************/
// Vector
/*****************************************************************************/
void PelParser::PrintVector(uint8_t i_val)
{
    //    PrintTableItem((uint32_t)i_val, ErrlVectorTable, ErrlVectorTableSz);
}
} // namespace pels
} // namespace openpower
