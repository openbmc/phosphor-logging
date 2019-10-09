#include "pel_utils.hpp"

#include "extensions/openpower-pels/private_header.hpp"
#include "extensions/openpower-pels/user_header.hpp"

#include <fstream>

#include <gtest/gtest.h>

namespace fs = std::filesystem;
using namespace openpower::pels;

std::filesystem::path CleanLogID::pelIDFile{};
std::filesystem::path CleanPELFiles::pelIDFile{};
std::filesystem::path CleanPELFiles::repoPath{};
std::filesystem::path CleanPELFiles::registryPath{};

constexpr uint8_t simplePEL[] = {
    // private header section header
    0x50, 0x48, // ID 'PH'
    0x00, 0x30, // Size
    0x01, 0x02, // version, subtype
    0x03, 0x04, // comp ID

    // private header
    0x20, 0x30, 0x05, 0x09, 0x11, 0x1E, 0x1, 0x63,  // create timestamp
    0x20, 0x31, 0x06, 0x0F, 0x09, 0x22, 0x3A, 0x00, // commit timestamp
    0xAA,                                           // creatorID
    0x00,                                           // logtype
    0x00,                                           // reserved
    0x02,                                           // section count
    0x90, 0x91, 0x92, 0x93,                         // OpenBMC log ID
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0,    // creator version
    0x50, 0x51, 0x52, 0x53,                         // plid
    0x80, 0x81, 0x82, 0x83,                         // id

    // user header section header
    0x55, 0x48, // ID 'UH'
    0x00, 0x18, // Size
    0x01, 0x0A, // version, subtype
    0x0B, 0x0C, // comp ID

    // user header
    0x10, 0x04,             // subsystem, scope
    0x20, 0x00,             // severity, type
    0x00, 0x00, 0x00, 0x00, // reserved
    0x03, 0x04,             // problem domain, vector
    0x80, 0xC0,             // action flags
    0x00, 0x00, 0x00, 0x00  // reserved

    // Add more as the code supports more
};

std::vector<uint8_t> srcFRUIdentityCallout{
    'I', 'D', 0x1C, 0x1D,                     // type, size, flags
    '1', '2', '3',  '4',                      // PN
    '5', '6', '7',  0x00, 'A', 'A', 'A', 'A', // CCIN
    '1', '2', '3',  '4',  '5', '6', '7', '8', // SN
    '9', 'A', 'B',  'C'};

std::vector<uint8_t> srcPCEIdentityCallout{
    'P', 'E', 0x24, 0x00,                      // type, size, flags
    'T', 'T', 'T',  'T',  '-', 'M', 'M',  'M', // MTM
    '1', '2', '3',  '4',  '5', '6', '7',       // SN
    '8', '9', 'A',  'B',  'C', 'P', 'C',  'E', // Name + null padded
    'N', 'A', 'M',  'E',  '1', '2', 0x00, 0x00, 0x00};

std::vector<uint8_t> srcMRUCallout{
    'M',  'R',  0x28, 0x04, // ID, size, flags
    0x00, 0x00, 0x00, 0x00, // Reserved
    0x00, 0x00, 0x00, 'H',  // priority 0
    0x01, 0x01, 0x01, 0x01, // MRU ID 0
    0x00, 0x00, 0x00, 'M',  // priority 1
    0x02, 0x02, 0x02, 0x02, // MRU ID 1
    0x00, 0x00, 0x00, 'L',  // priority 2
    0x03, 0x03, 0x03, 0x03, // MRU ID 2
    0x00, 0x00, 0x00, 'H',  // priority 3
    0x04, 0x04, 0x04, 0x04, // MRU ID 3
};

std::unique_ptr<std::vector<uint8_t>> pelDataFactory(TestPelType type)
{
    std::unique_ptr<std::vector<uint8_t>> data;
    switch (type)
    {
        case TestPelType::pelSimple:
            data = std::make_unique<std::vector<uint8_t>>(
                simplePEL, simplePEL + sizeof(simplePEL));
            break;
        case TestPelType::privateHeaderSimple:
            data = std::make_unique<std::vector<uint8_t>>(
                simplePEL, simplePEL + PrivateHeader::flattenedSize());
            break;
        case TestPelType::userHeaderSimple:
            data = std::make_unique<std::vector<uint8_t>>(
                simplePEL + PrivateHeader::flattenedSize(),
                simplePEL + PrivateHeader::flattenedSize() +
                    UserHeader::flattenedSize());
            break;
    }
    return data;
}

std::vector<uint8_t> srcDataFactory(TestSRCType type)
{
    switch (type)
    {
        case TestSRCType::fruIdentityStructure:
            return srcFRUIdentityCallout;

        case TestSRCType::pceIdentityStructure:
            return srcPCEIdentityCallout;

        case TestSRCType::mruStructure:
            return srcMRUCallout;
    }
    return {};
}

std::unique_ptr<std::vector<uint8_t>> readPELFile(const fs::path& path)
{
    std::ifstream file{path};

    auto pel = std::make_unique<std::vector<uint8_t>>(
        std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return pel;
}
