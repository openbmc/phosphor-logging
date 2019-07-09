#include "manager.hpp"

#include "additional_data.hpp"

namespace openpower
{
namespace pels
{

using namespace phosphor::logging;

namespace additional_data
{
constexpr auto rawPEL = "RAWPEL";
}

void Manager::create(const std::string& message, uint32_t obmcLogID,
                     uint64_t timestamp, Entry::Level severity,
                     const std::vector<std::string>& additionalData,
                     const std::vector<std::string>& associations)
{
    AdditionalData ad{additionalData};

    // If a PEL was passed in, use that.  Otherwise, create one.
    auto rawPelPath = ad.getValue(additional_data::rawPEL);
    if (rawPelPath)
    {
        addRawPEL(*rawPelPath, obmcLogID);
    }
    else
    {
        createPEL(message, obmcLogID, timestamp, severity, additionalData,
                  associations);
    }
}

void Manager::addRawPEL(const std::string& rawPelPath, uint32_t obmcLogID)
{
}

void Manager::erase(uint32_t obmcLogID)
{
}

bool Manager::isDeleteProhibited(uint32_t obmcLogID)
{
    return false;
}

void Manager::createPEL(const std::string& message, uint32_t obmcLogID,
                        uint64_t timestamp,
                        phosphor::logging::Entry::Level severity,
                        const std::vector<std::string>& additionalData,
                        const std::vector<std::string>& associations)
{
}

} // namespace pels
} // namespace openpower
