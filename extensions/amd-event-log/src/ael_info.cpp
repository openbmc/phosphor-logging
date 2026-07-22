#include "ael_info.hpp"

namespace phosphor::logging::extensions::ael
{

AELInfoProvider::AELInfoProvider(const std::string& msg, Entry::Level level,
                                 const AdditionalDataVec& additionalData,
                                 const AssociationList& associations) :
    msg(msg), level(level), additionalData(additionalData),
    associations(associations)
{}

AELInfo AELInfoProvider::get() const
{
    AELInfo info;

    auto afidInfo = lookupAFID(msg, additionalData);

    if (!afidInfo)
    {
        return info;
    }

    info.afid = afidInfo->afid;
    info.fruList = afidInfo->frus;

    return info;
}

} // namespace phosphor::logging::extensions::ael
