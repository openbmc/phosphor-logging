#include "pelparser.hpp"

#include "../pel.hpp"
#include "extensions/openpower-pels/paths.hpp"
#include "extensions/openpower-pels/repository.hpp"
#include "extensions/openpower-pels/tools/peltool_utility.hpp"
#include "extensions/openpower-pels/user_header.hpp"

#include <stdio.h>

#include <CLI/CLI.hpp>
#include <cstdio>
#include <filesystem>
#include <fstream>
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
void  PELParser::printFailingSubSystem(uint8_t i_val)
{
    auto tmp = pel_values::findByValue(i_val, pel_values::subsystemValues);
    PELParser::printString("Failing Subsystem",
                           std::get<pel_values::registryNamePos>(*tmp));
} 

/*****************************************************************************/
// Failing SubSystem
/*****************************************************************************/
const char* PELParser::getFailingSubSystem(uint8_t i_val)
{
    auto tmp = pel_values::findByValue(i_val, pel_values::subsystemValues);
    return std::get<pel_values::registryNamePos>(*tmp);
}

/*****************************************************************************/
// Severity
/*****************************************************************************/
void PELParser::printSeverity(uint8_t i_val)
{
    auto tmp = pel_values::findByValue(i_val, pel_values::severityValues);
    PELParser::printString("Severity",
                           std::get<pel_values::registryNamePos>(*tmp));
}

/*****************************************************************************/
// Severity
/*****************************************************************************/
const char* PELParser::getSeverity(uint8_t i_val)
{
    auto tmp = pel_values::findByValue(i_val, pel_values::severityValues);
    return std::get<pel_values::registryNamePos>(*tmp);
}

/*****************************************************************************/
// Domain
/*****************************************************************************/
void PELParser::printDomain(uint8_t i_val)
{
    /*    auto tmp = pel_values::findByValue(i_val, pel_values::domainValues);
        PelParser::PrintString("Domain",
                               std::get<pel_values::registryNamePos>(*tmp));
    */
}

/*****************************************************************************/
// Vector
/*****************************************************************************/
void PELParser::printVector(uint8_t i_val)
{
    //    auto tmp = pel_values::findByValue(i_val, pel_values::VectorValues);
    //    PelParser::PrintString("Vector",
    //                           std::get<pel_values::registryNamePos>(*tmp));
}

/*****************************************************************************/
// Regular string ( may be multiline )
/*****************************************************************************/

void PELParser::printString(const char* i_label, const char* i_string)
{
    // Must make sure the string fits on the available width
    int l_strlen = 0;
    int l_printed = 0;

    // Ensure String is valid
    if (i_string)
    {
        l_strlen = strlen(i_string);
    }

    // Fake a blank string
    if (!l_strlen)
    {
        l_strlen = 1;
        i_string = " ";
    }

    // String processing variables
    const int ASCII_ALERT = 7;
    const int ASCII_NEWLINE = 10;
    const int ASCII_CARRIAGE_RETURN = 13;
    const int ASCII_ESCAPE = 27;
    const int LINE_WIDTH = 50;
    int l_widthRemaining = LINE_WIDTH;
    bool l_badCharactersFound = false, l_onNewline = true;

    // Print it out.
    while (l_strlen > l_printed)
    {
        if ((LINE_WIDTH == l_widthRemaining) && l_onNewline)
        {
            // Leader ( label or blanks )
            PrintLabel(i_label);

            // label is only printed once
            i_label = "";

            l_onNewline = false;
        }

        // Pre-process the information to be printed by walking the string and
        // looking for characters that shouldn't be printed out.
        l_badCharactersFound = false;
        int l_offset = l_printed;
        while (!l_badCharactersFound &&
               ((l_offset - l_printed) <= l_widthRemaining))
        {
            if ('\0' == i_string[l_offset])
            {
                break;
            }

            // Look for characters that fall between (inclusive) ASCII_ALERT and
            // ASCII_CARRAIGE_RETURN plus ASCII_ESCAPE.
            // '\a', '\b', '\t', '\n', '\v', '\f', '\r', and '\e'
            if ((ASCII_ALERT <= i_string[l_offset] &&
                 ASCII_CARRIAGE_RETURN >= i_string[l_offset]) ||
                (ASCII_ESCAPE == i_string[l_offset]))
            {
                l_badCharactersFound = true;
            }
            else
            {
                // Only increment offset when undesirable characters haven't
                // been found. That way the offset can be reused as an index to
                // the character.
                ++l_offset;
            }
        }

        // If no undesirable characters were found then just print the rest of
        // the remaining characters onto the line and start a new one.
        if (!l_badCharactersFound)
        {
            l_printed += printf("%-*.*s", l_widthRemaining, l_widthRemaining,
                                i_string + l_printed);

            // Start a new line.
            printf("|\n");
            l_widthRemaining = LINE_WIDTH;
            l_onNewline = true;
        }
        else
        {
            int l_numCharsToPrint = l_offset - l_printed;

            // Undesirable character found. Print up to it.
            // Note: If l_widthRemaining becomes 0 as a result of this print
            //       call then the next iteration of the outer while loop will
            //       cause it to reset by passing through the if part of this
            //       if statement.
            l_widthRemaining -= printf("%-*.*s", l_numCharsToPrint,
                                       l_numCharsToPrint, i_string + l_printed);

            // Skip over undesirable character.
            l_printed += l_numCharsToPrint + 1;

            // '\n' characters found in the string are treated as new lines in
            // the error log output.
            if (ASCII_NEWLINE == i_string[l_offset])
            {
                // Fill the remainder of the line with spaces and start a new
                // one.
                printf("%-*c", l_widthRemaining, ' ');

                // Start a new line.
                printf("|\n");
                l_widthRemaining = LINE_WIDTH;
                l_onNewline = true;
            }
        }
    }
}
void PELParser::printUH(uint8_t iv_ssid, uint8_t iv_severity, uint8_t iv_etype,
                        uint8_t iv_actions)
{
    printFailingSubSystem(iv_ssid);

    printSeverity(iv_severity);
    /* to do
    // If severity is of "Symptom" type print the domain and vector
    if(isSymptomSev((errlSeverity)iv_severity) == true)
    {
        i_parser.PrintDomain( iv_domain );
        i_parser.PrintVector( iv_vector );
    }

    // Display event type only if severity is informational
    if ( iv_severity == ERRL_SEV_INFORMATIONAL )
    {
        i_parser.PrintEventType( iv_etype );
    }

    i_parser.PrintActions( iv_actions );

    // Print State and HMC ack flag if log has that information
    if ( reserved1Valid() )
    {
        bool        l_hmcack = UTIL_GET_BYTE( iv_reserved1, 2 );
        bool        l_ammack = UTIL_GET_BYTE( iv_reserved1, 0 );
        errlState   l_state = errlState ( UTIL_GET_BYTE ( iv_reserved1, 1 ) );

        i_parser.PrintActionStatus( l_hmcack,l_state,l_ammack );
    }
    */
}
} // namespace pels
} // namespace openpower
