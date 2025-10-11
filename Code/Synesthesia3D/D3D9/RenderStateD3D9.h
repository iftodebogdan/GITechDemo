/**
 * @file        RenderStateD3D9.h
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

#ifndef RENDERSTATED3D9_H
#define RENDERSTATED3D9_H

#include "RenderState.h"

namespace Synesthesia3D
{
    class RenderStateD3D9 : public RenderState
    {
    public:
        const unsigned int MatchRenderState(const DWORD rs, const unsigned int rsEnumClass) const;

        const bool  SetScissor(const Vec2i size, const Vec2i offset = Vec2i(0, 0));

        void        Reset();

    protected:
        RenderStateD3D9();
        ~RenderStateD3D9();

        const bool Flush();

        // Local render states, sync'ed with underlying API
        bool            m_bColorBlendEnabledD3D9;
        Blend           m_eColorSrcBlendD3D9;
        Blend           m_eColorDstBlendD3D9;
        Vec4f           m_vColorBlendFactorD3D9;
        bool            m_bAlphaTestEnabledD3D9;
        Cmp             m_eAlphaFuncD3D9;
        float           m_fAlphaRefD3D9;
        Cull            m_eCullModeD3D9;
        ZBuffer         m_eZEnabledD3D9;
        Cmp             m_eZFuncD3D9;
        bool            m_bZWriteEnabledD3D9;
        bool            m_bColorWriteRedD3D9;
        bool            m_bColorWriteGreenD3D9;
        bool            m_bColorWriteBlueD3D9;
        bool            m_bColorWriteAlphaD3D9;
        float           m_fSlopeScaledDepthBiasD3D9;
        float           m_fDepthBiasD3D9;
        bool            m_bStencilEnabledD3D9;
        Cmp             m_eStencilFuncD3D9;
        unsigned long   m_lStencilRefD3D9;
        unsigned long   m_lStencilMaskD3D9;
        unsigned long   m_lStencilWriteMaskD3D9;
        StencilOp       m_eStencilFailD3D9;
        StencilOp       m_eStencilZFailD3D9;
        StencilOp       m_eStencilPassD3D9;
        Fill            m_eFillModeD3D9;
        bool            m_bScissorEnabledD3D9;
        bool            m_bSRGBEnabledD3D9;

        friend class RendererD3D9;
    };
}

#endif // RENDERSTATED3D9_H