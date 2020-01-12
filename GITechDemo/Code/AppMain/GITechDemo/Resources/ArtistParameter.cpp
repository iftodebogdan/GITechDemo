/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   ArtistParameter.cpp
 *      Author: Bogdan Iftode
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#include "stdafx.h"

#include <gainput/gainput.h>

#include "ArtistParameter.h"
using namespace GITechDemoApp;

// Moved to AppResources.cpp until the issue with the static initialization fiasco is resolved
//vector<ArtistParameter*> ArtistParameter::ms_arrParams;

namespace GITechDemoApp
{
    extern unsigned long long g_TypeHash[];
}

#define PARAM_IS_FLOAT()    (m_nTypeHash == g_TypeHash[APDT_FLOAT])
#define PARAM_IS_INT()      (m_nTypeHash == g_TypeHash[APDT_INT])
//#define PARAM_IS_UINT()     (m_nTypeHash == g_TypeHash[APDT_UINT])
#define PARAM_IS_BOOL()     (m_nTypeHash == g_TypeHash[APDT_BOOL])
#define PARAM_IS(APDT_TYPE) (m_nTypeHash == g_TypeHash[APDT_TYPE])

ArtistParameter::ArtistParameter(
    const char* const name,
    const char* const desc,
    const char* const category,
    void* const param,
    const float step,
    const unsigned long long typeHash,
    const float defaultValue)
    : m_szName(name)
    , m_szDesc(desc)
    , m_szCategory(category)
    , m_pParam(param)
    , m_fStepValue(step)
    , m_nTypeHash(typeHash)
    , m_fDefaultValue(defaultValue)
{
    ms_arrParams.push_back(this);

    switch (GetDataType())
    {
    case APDT_FLOAT:
        GetParameterAsFloat() = m_fDefaultValue;
        break;

    case APDT_INT:
        GetParameterAsInt() = m_fDefaultValue;
        break;

    case APDT_BOOL:
        GetParameterAsBool() = m_fDefaultValue;
        break;

    default:
        assert(false);
    }
}

ArtistParameter::~ArtistParameter()
{
    for (unsigned int i = 0; i < ms_arrParams.size(); i++)
    {
        if (ms_arrParams[i] == this)
        {
            ms_arrParams.erase(ms_arrParams.begin() + i);
            break;
        }
    }
}

const ArtistParameter::ArtistParameterDataType ArtistParameter::GetDataType() const
{
    if (PARAM_IS_FLOAT())
    {
        return APDT_FLOAT;
    }
    else if (PARAM_IS_INT())
    {
        return APDT_INT;
    }
    //else if (PARAM_IS_UINT())
    //{
    //    return APDT_UINT;
    //}
    else if (PARAM_IS_BOOL())
    {
        return APDT_BOOL;
    }
    else
    {
        assert(false); // Support for data type hasn't been implemented!
        return APDT_NOT_SUPPORTED;
    }
}

const bool ArtistParameter::IsDataType(const ArtistParameter::ArtistParameterDataType type) const
{
    if (type >= 0 && type < APDT_MAX)
    {
        return PARAM_IS(type);
    }
    else
    {
        assert(false);
        return false;
    }
}

ArtistParameter* const ArtistParameter::GetParameterByIdx(const unsigned int idx)
{
    if (idx < ms_arrParams.size())
    {
        return ms_arrParams[idx];
    }
    else
    {
        return nullptr;
    }
}
