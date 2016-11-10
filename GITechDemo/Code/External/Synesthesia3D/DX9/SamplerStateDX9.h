/**
 * @file        SamplerStateDX9.h
 *
 * @note        This file is part of the "Synesthesia3D" graphics engine
 *
 * @copyright   Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * @copyright
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * @copyright
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SAMPLERSTATEDX9_H
#define SAMPLERSTATEDX9_H

#include "SamplerState.h"

enum _D3DTEXTUREFILTERTYPE;

namespace Synesthesia3D
{
    class RendererDX9;

    class SamplerStateDX9 : public SamplerState
    {
    public:
        void    Reset();

    protected:
        SamplerStateDX9();
        ~SamplerStateDX9();

        const bool  Flush();

        _D3DTEXTUREFILTERTYPE   m_eCurrentMinFilter[MAX_NUM_PSAMPLERS];

        // Local sampler states, sync'ed with underlying API
        SamplerStateDesc m_tCurrentStateDX9[MAX_NUM_PSAMPLERS];

        friend class RendererDX9;
    };
}

#endif // SAMPLERSTATEDX9_H