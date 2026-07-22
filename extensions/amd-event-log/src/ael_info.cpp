#include "ael_info.hpp"

namespace phosphor::logging::extensions::ael
{

AELInfoProvider::AELInfoProvider(
    const std::string& msg, Entry::Level level,
    const std::map<std::string, std::string>& additionalData) :
    msg(msg), level(level), additionalData(additionalData)
{}

AELInfo AELInfoProvider::get() const
{
    (void)level;

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
