#pragma once

#include "../bcd_time.hpp"

#include <iostream> // std::cout
#include <sstream>  // std::stringstream
#include <string>   // std::string

using namespace std;
namespace openpower
{
namespace pels
{
int bcdToDec(uint8_t val);
int decToBcd(uint8_t val);
int hexCharToInt(char a);
string hexToString(string str);
string parseTimeStamp(const BCDTime& timeStamp);
string printLine();
string printLineBreak();

// TODO put this under user header - Aatir
enum errlSeverity
{
    ERRL_SEV_INFORMATIONAL = 0,
    ERRL_SEV_RECOVERED = 0x10,           ///< Recovered Error, general
    ERRL_SEV_PREDICTIVE = 0x20,          ///< Predictive Error, general
    ERRL_SEV_PREDICTIVE_DEGRADED = 0x21, ///< Predictive, degraded performance
    ERRL_SEV_PREDICTIVE_CORRECTABLE =
        0x22, ///< Predictive, may correct after re-ipl
    ERRL_SEV_PREDICTIVE_CORRECTABLE2 =
        0x23, ///< Predictive, degraded perf, re-ipl correctable
    ERRL_SEV_PREDICTIVE_REDUNDANCY_LOST =
        0x24,                       ///< Predictive, loss of redundancy
    ERRL_SEV_UNRECOVERABLE = 0x40,  ///< Unrecoverable, general
    ERRL_SEV_UNRECOVERABLE1 = 0x41, ///< Unrecoverable, bypassed w/degraded perf
    ERRL_SEV_UNRECOVERABLE2 =
        0x44, ///< Unrecoverable, bypassed w/loss of redundancy
    ERRL_SEV_UNRECOVERABLE3 =
        0x45, ///< Unrecoverable, bypassed w/loss of red+perf
    ERRL_SEV_UNRECOVERABLE4 =
        0x48, ///< Unrecoverable, bypassed w/loss of function
    ERRL_SEV_CRITICAL_FAIL_UNKNOWN =
        0x50,                           ///< Critical, Scope of failure unknown
    ERRL_SEV_CRITICAL_SYS_TERM = 0x51,  ///< Critical, System Termination
    ERRL_SEV_CRITICAL_SYS_FAIL = 0x52,  ///< Critical, System Failure
    ERRL_SEV_CRITICAL_PART_TERM = 0x53, ///< Critical, Partition Termination
    ERRL_SEV_CRITICAL_PART_FAIL = 0x54, ///< Critical, Partition Failure
    ERRL_SEV_DIAGNOSTIC_ERROR1 = 0x60,  ///< Disagnostic test error
    ERRL_SEV_DIAGNOSTIC_ERROR2 =
        0x61, ///< Diagnostic test error, resource may produce bad results
    ERRL_SEV_SYMPTOM_RECOVERED = 0x71,  ///< Symptom Recovered
    ERRL_SEV_SYMPTOM_PREDICTIVE = 0x72, ///< Symptom Predictive
    ERRL_SEV_SYMPTOM_UNRECOV = 0x74,    ///< Symptom Unrecoverable
    ERRL_SEV_SYMPTOM_DIAGERR = 0x76,    ///< Symptom Diag Err
    ERRL_SEV_RESERVED = 0xFF            ///< Reserved
};

} // namespace pels
} // namespace openpower
