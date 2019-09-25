#include "section_factory.hpp"

#include "generic.hpp"
#include "pel_types.hpp"
#include "private_header.hpp"
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
        case static_cast<uint16_t>(SectionID::userHeader):
            section = std::make_unique<UserHeader>(pelData);
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
