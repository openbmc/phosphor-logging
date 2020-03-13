/**
 * Copyright © 2019 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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

size_t FRUIdentity::flattenedSize() const
{
    size_t size = sizeof(_type) + sizeof(_size) + sizeof(_flags);

    if (hasPN() || hasMP())
    {
        size += _pnOrProcedureID.size();
    }

    if (hasCCIN())
    {
        size += _ccin.size();
    }

    if (hasSN())
    {
        size += _sn.size();
    }

    return size;
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

void FRUIdentity::flatten(Stream& pel) const
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
