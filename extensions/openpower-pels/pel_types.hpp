#pragma once

namespace openpower
{
namespace pels
{

enum class ComponentID
{
    phosphorLogging = 0x2000
};

/**
 * @brief PEL section IDs
 */
enum class SectionID
{
    privateHeader = 0x5048,      // 'PH'
    userHeader = 0x5548,         // 'UH'
    primarySRC = 0x5053,         // 'PS'
    secondarySRC = 0x5353,       // 'SS'
    extendedUserHeader = 0x4558, // 'EH'
    failingMTMS = 0x4D54,        // 'MT'
    dumpLocation = 0x4448,       // 'DH'
    firmwareError = 0x5357,      // 'SW'
    impactedPart = 0x4C50,       // 'LP'
    logicalResource = 0x4C52,    // 'LR'
    hmcID = 0x484D,              // 'HM'
    epow = 0x4550,               // 'EP'
    ioEvent = 0x4945,            // 'IE'
    mfgInfo = 0x4D49,            // 'MI'
    callhome = 0x4348,           // 'CH'
    userData = 0x5544,           // 'UD'
    envInfo = 0x4549,            // 'EI'
    extUserData = 0x4544         // 'ED'
};

enum class SRCType
{
    bmcError = 0xBD,
    powerError = 0x11
};

} // namespace pels
} // namespace openpower
