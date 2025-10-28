// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2019 IBM Corporation

#include "section_factory.hpp"

#include "extended_user_data.hpp"
#include "extended_user_header.hpp"
#include "failing_mtms.hpp"
#include "generic.hpp"
#include "pel_types.hpp"
#include "private_header.hpp"
#include "src.hpp"
#include "user_data.hpp"
#include "user_header.hpp"

namespace openpower
{
namespace pels
{
namespace section_factory
{
std::unique_ptr<Section> create(Stream& pelData)
{
    std::unique_ptr<Section> section;

    // Peek the section ID to create the appriopriate object.
    // If not enough data remains to do so, an invalid
    // Generic object will be created in the default case.
    uint16_t sectionID = 0;

    if (pelData.remaining() >= 2)
    {
        pelData >> sectionID;
        pelData.offset(pelData.offset() - 2);
    }

    switch (sectionID)
    {
        case static_cast<uint16_t>(SectionID::privateHeader):
            section = std::make_unique<PrivateHeader>(pelData);
            break;
        case static_cast<uint16_t>(SectionID::userData):
            section = std::make_unique<UserData>(pelData);
            break;
        case static_cast<uint16_t>(SectionID::userHeader):
            section = std::make_unique<UserHeader>(pelData);
            break;
        case static_cast<uint16_t>(SectionID::failingMTMS):
            section = std::make_unique<FailingMTMS>(pelData);
            break;
        case static_cast<uint16_t>(SectionID::primarySRC):
        case static_cast<uint16_t>(SectionID::secondarySRC):
            section = std::make_unique<SRC>(pelData);
            break;
        case static_cast<uint16_t>(SectionID::extendedUserHeader):
            section = std::make_unique<ExtendedUserHeader>(pelData);
            break;
        case static_cast<uint16_t>(SectionID::extUserData):
            section = std::make_unique<ExtendedUserData>(pelData);
            break;
        default:
            // A generic object, but at least an object.
            section = std::make_unique<Generic>(pelData);
            break;
    }

    return section;
}

} // namespace section_factory
} // namespace pels
} // namespace openpower
