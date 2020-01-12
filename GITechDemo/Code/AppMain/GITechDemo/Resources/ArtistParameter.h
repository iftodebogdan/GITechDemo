/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   ArtistParameter.h
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

#ifndef ARTIST_PARAMETER_H_
#define ARTIST_PARAMETER_H_

#include <string>
#include <vector>
using namespace std;

namespace GITechDemoApp
{
    class ArtistParameter
    {
    public:
        ArtistParameter(
            const char* const name,
            const char* const desc,
            const char* const category,
            void* const param,
            const float step,
            const unsigned long long typeHash,
            const float defaultValue);

        ~ArtistParameter();

        enum ArtistParameterDataType
        {
            APDT_FLOAT = 0,
            APDT_INT,
            //APDT_UINT,
            APDT_BOOL,
            APDT_MAX,
            APDT_NOT_SUPPORTED
        };

                const string            GetName() const { return m_szName; };
                const string            GetDescription() const { return m_szDesc; }
                const string            GetCategoryName() const { return m_szCategory; }
        const ArtistParameterDataType   GetDataType() const;
                const bool              IsDataType(const ArtistParameterDataType type) const;
                    float&              GetParameterAsFloat() const { assert(IsDataType(APDT_FLOAT)); return *(float*)m_pParam; }
                    int&                GetParameterAsInt() const { assert(IsDataType(APDT_INT)); return *(int*)m_pParam; }
        //        unsigned int&           GetParameterAsUInt() const { assert(IsDataType(APDT_UINT)); return *(unsigned int*)m_pParam; }
                    bool&               GetParameterAsBool() const { assert(IsDataType(APDT_BOOL)); return *(bool*)m_pParam; }
                    float               GetStepValue() const { return m_fStepValue; }
                    float               GetDefaultValue() const { return m_fDefaultValue; }

        static ArtistParameter* const   GetParameterByIdx(const unsigned int idx);
        static const unsigned int       GetParameterCount() { return (unsigned int)ms_arrParams.size(); }

    protected:
        string m_szName;
        string m_szDesc;
        string m_szCategory;
        void* m_pParam;
        float m_fStepValue;
        unsigned long long m_nTypeHash;
        float m_fDefaultValue;

        static vector<ArtistParameter*> ms_arrParams;
    };
}

#endif //ARTIST_PARAMETER_MANAGER_H_