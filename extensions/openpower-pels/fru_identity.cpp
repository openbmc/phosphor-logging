#include "fru_identity.hpp"

namespace openpower
{
namespace pels
{
namespace src
{

FRUIdentity::FRUIdentity(Stream& pel)
{
    pel >> _type >> _size >> _flags;

    if (_flags & (pnSupplied | maintProcSupplied))
    {
        pel.read(_pnOrProcedureID.data(), _pnOrProcedureID.size());
    }

    if (_flags & ccinSupplied)
    {
        pel.read(_ccin.data(), _ccin.size());
    }

    if (_flags & snSupplied)
    {
        pel.read(_sn.data(), _sn.size());
    }
}

std::optional<std::string> FRUIdentity::getPN() const
{
    if (hasPN())
    {
        // NULL terminated
        std::string pn{_pnOrProcedureID.data()};
        return pn;
    }

    return std::nullopt;
}

std::optional<std::string> FRUIdentity::getMaintProc() const
{
    if (hasMP())
    {
        // NULL terminated
        std::string mp{_pnOrProcedureID.data()};
        return mp;
    }

    return std::nullopt;
}

std::optional<std::string> FRUIdentity::getCCIN() const
{
    if (hasCCIN())
    {
        std::string ccin{_ccin.begin(), _ccin.begin() + _ccin.size()};
        return ccin;
    }

    return std::nullopt;
}

std::optional<std::string> FRUIdentity::getSN() const
{
    if (hasSN())
    {
        std::string sn{_sn.begin(), _sn.begin() + _sn.size()};
        return sn;
    }

    return std::nullopt;
}

void FRUIdentity::flatten(Stream& pel)
{
    pel << _type << _size << _flags;

    if (hasPN() || hasMP())
    {
        pel.write(_pnOrProcedureID.data(), _pnOrProcedureID.size());
    }

    if (hasCCIN())
    {
        pel.write(_ccin.data(), _ccin.size());
    }

    if (hasSN())
    {
        pel.write(_sn.data(), _sn.size());
    }
}

} // namespace src
} // namespace pels
} // namespace openpower
