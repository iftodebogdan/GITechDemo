/**
 * @file        RenderStateD3D9.cpp
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

#include "stdafx.h"

#include "RendererD3D9.h"
#include "MappingsD3D9.h"
#include "RenderStateD3D9.h"
#include "ProfilerD3D9.h"
using namespace Synesthesia3D;

RenderStateD3D9::RenderStateD3D9()
{
    Reset();
}

RenderStateD3D9::~RenderStateD3D9()
{}

const unsigned int RenderStateD3D9::MatchRenderState(const DWORD rs, const unsigned int rsEnumClass) const
{
    unsigned int begin, end;
    switch (rsEnumClass)
    {
    case BLEND:
        begin = BLEND;
        end = BLEND_MAX;
        break;
    case CMP:
        begin = CMP;
        end = CMP_MAX;
        break;
    case CULL:
        begin = CULL;
        end = CULL_MAX;
        break;
    case ZB:
        begin = ZB;
        end = ZB_MAX;
        break;
    case STENCILOP:
        begin = STENCILOP;
        end = STENCILOP_MAX;
        break;
    case FILL:
        begin = FILL;
        end = FILL_MAX;
        break;
    default:
        assert(false);
        return 0;
    }

    for (unsigned int i = begin + 1; i < end; i++)
    {
        if (RenderStateMappingD3D9[i] == rs)
            return i;
    }

    assert(false);
    return 0;
}

const bool RenderStateD3D9::SetScissor(const Vec2i size, const Vec2i offset)
{
    IDirect3DDevice9* device = RendererD3D9::GetInstance()->GetDevice();
    const RECT scissorRect = { offset[0], offset[1], offset[0] + size[0], offset[1] + size[1] };
    HRESULT hr = device->SetScissorRect(&scissorRect);
    assert(SUCCEEDED(hr));
    return SUCCEEDED(hr);
}

void RenderStateD3D9::Reset()
{
    IDirect3DDevice9* device = RendererD3D9::GetInstance()->GetDevice();
    HRESULT hr;
    DWORD value;



    hr = device->GetRenderState(D3DRS_ALPHABLENDENABLE, &value);
    assert(SUCCEEDED(hr));
    m_bColorBlendEnabledD3D9 = (value != 0);

    hr = device->GetRenderState(D3DRS_SRCBLEND, &value);
    assert(SUCCEEDED(hr));
    m_eColorSrcBlendD3D9 = (Blend)MatchRenderState(value, BLEND);

    hr = device->GetRenderState(D3DRS_DESTBLEND, &value);
    assert(SUCCEEDED(hr));
    m_eColorDstBlendD3D9 = (Blend)MatchRenderState(value, BLEND);

    hr = device->GetRenderState(D3DRS_BLENDFACTOR, &value);
    assert(SUCCEEDED(hr));
    m_vColorBlendFactorD3D9 = Vec4f(
        (float)((value & (0xff << 16)) >> 16) / 255.f,  // red
        (float)((value & (0xff << 8)) >> 8) / 255.f,    // green
        (float)(value & 0xff) / 255.f,                  // blue
        (float)((value & (0xff << 24)) >> 24) / 255.f); // alpha



    hr = device->GetRenderState(D3DRS_ALPHATESTENABLE, &value);
    assert(SUCCEEDED(hr));
    m_bAlphaTestEnabledD3D9 = (value != 0);

    hr = device->GetRenderState(D3DRS_ALPHAFUNC, &value);
    assert(SUCCEEDED(hr));
    m_eAlphaFuncD3D9 = (Cmp)MatchRenderState(value, CMP);

    hr = device->GetRenderState(D3DRS_ALPHAREF, &value);
    assert(SUCCEEDED(hr));
    m_fAlphaRefD3D9 = (float)value / 255.f;



    hr = device->GetRenderState(D3DRS_CULLMODE, &value);
    assert(SUCCEEDED(hr));
    m_eCullModeD3D9 = (Cull)MatchRenderState(value, CULL);



    hr = device->GetRenderState(D3DRS_ZENABLE, &value);
    assert(SUCCEEDED(hr));
    m_eZEnabledD3D9 = (ZBuffer)MatchRenderState(value, ZB);

    hr = device->GetRenderState(D3DRS_ZFUNC, &value);
    assert(SUCCEEDED(hr));
    m_eZFuncD3D9 = (Cmp)MatchRenderState(value, CMP);

    hr = device->GetRenderState(D3DRS_ZWRITEENABLE, &value);
    assert(SUCCEEDED(hr));
    m_bZWriteEnabledD3D9 = (value != 0);


    hr = device->GetRenderState(D3DRS_COLORWRITEENABLE, &value);
    assert(SUCCEEDED(hr));
    m_bColorWriteRedD3D9     = (value & D3DCOLORWRITEENABLE_RED) != 0;
    m_bColorWriteGreenD3D9   = (value & D3DCOLORWRITEENABLE_GREEN) != 0;
    m_bColorWriteBlueD3D9    = (value & D3DCOLORWRITEENABLE_BLUE) != 0;
    m_bColorWriteAlphaD3D9   = (value & D3DCOLORWRITEENABLE_ALPHA) != 0;


    hr = device->GetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, &value);
    assert(SUCCEEDED(hr));
    m_fSlopeScaledDepthBiasD3D9 = *(float*)&value;

    hr = device->GetRenderState(D3DRS_DEPTHBIAS, &value);
    assert(SUCCEEDED(hr));
    m_fDepthBiasD3D9 = *(float*)&value;// *16777216.0f;



    hr = device->GetRenderState(D3DRS_STENCILENABLE, &value);
    assert(SUCCEEDED(hr));
    m_bStencilEnabledD3D9 = (value != 0);

    hr = device->GetRenderState(D3DRS_STENCILFUNC, &value);
    assert(SUCCEEDED(hr));
    m_eStencilFuncD3D9 = (Cmp)MatchRenderState(value, CMP);

    hr = device->GetRenderState(D3DRS_STENCILREF, &value);
    assert(SUCCEEDED(hr));
    m_lStencilRefD3D9 = value;

    hr = device->GetRenderState(D3DRS_STENCILMASK, &value);
    assert(SUCCEEDED(hr));
    m_lStencilMaskD3D9 = value;

    hr = device->GetRenderState(D3DRS_STENCILWRITEMASK, &value);
    assert(SUCCEEDED(hr));
    m_lStencilWriteMaskD3D9 = value;

    hr = device->GetRenderState(D3DRS_STENCILFAIL, &value);
    assert(SUCCEEDED(hr));
    m_eStencilFailD3D9 = (StencilOp)MatchRenderState(value, STENCILOP);

    hr = device->GetRenderState(D3DRS_STENCILZFAIL, &value);
    assert(SUCCEEDED(hr));
    m_eStencilZFailD3D9 = (StencilOp)MatchRenderState(value, STENCILOP);

    hr = device->GetRenderState(D3DRS_STENCILPASS, &value);
    assert(SUCCEEDED(hr));
    m_eStencilPassD3D9 = (StencilOp)MatchRenderState(value, STENCILOP);



    hr = device->GetRenderState(D3DRS_FILLMODE, &value);
    assert(SUCCEEDED(hr));
    m_eFillModeD3D9 = (Fill)MatchRenderState(value, FILL);



    hr = device->GetRenderState(D3DRS_SCISSORTESTENABLE, &value);
    assert(SUCCEEDED(hr));
    m_bScissorEnabledD3D9 = (value != 0);



    hr = device->GetRenderState(D3DRS_SRGBWRITEENABLE, &value);
    assert(SUCCEEDED(hr));
    m_bSRGBEnabledD3D9 = (value != 0);



    RenderState::Reset();
}

const bool RenderStateD3D9::Flush()
{
    IDirect3DDevice9* device = RendererD3D9::GetInstance()->GetDevice();
    HRESULT hr = E_FAIL;

    if (m_bColorBlendEnabledD3D9 != GetColorBlendEnabled())
    {
        hr = device->SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)GetColorBlendEnabled());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_bColorBlendEnabledD3D9 = GetColorBlendEnabled();
        else
            return false;
    }

    if (m_eColorSrcBlendD3D9 != GetColorSrcBlend())
    {
        hr = device->SetRenderState(D3DRS_SRCBLEND/*ALPHA*/, RenderStateMappingD3D9[GetColorSrcBlend()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eColorSrcBlendD3D9 = GetColorSrcBlend();
        else
            return false;
    }

    if (m_eColorDstBlendD3D9 != GetColorDstBlend())
    {
        hr = device->SetRenderState(D3DRS_DESTBLEND/*ALPHA*/, RenderStateMappingD3D9[GetColorDstBlend()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eColorDstBlendD3D9 = GetColorDstBlend();
        else
            return false;
    }

    if (m_bAlphaTestEnabledD3D9 != GetAlphaTestEnabled())
    {
        hr = device->SetRenderState(D3DRS_ALPHATESTENABLE, (DWORD)GetAlphaTestEnabled());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_bAlphaTestEnabledD3D9 = GetAlphaTestEnabled();
        else
            return false;
    }

    if (m_eAlphaFuncD3D9 != GetAlphaTestFunc())
    {
        hr = device->SetRenderState(D3DRS_ALPHAFUNC, RenderStateMappingD3D9[GetAlphaTestFunc()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eAlphaFuncD3D9 = GetAlphaTestFunc();
        else
            return false;
    }

    if (m_fAlphaRefD3D9 != GetAlphaTestRef())
    {
        hr = device->SetRenderState(D3DRS_ALPHAREF, (DWORD)(255.f * GetAlphaTestRef()));
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_fAlphaRefD3D9 = GetAlphaTestRef();
        else
            return false;
    }

    if (m_vColorBlendFactorD3D9 != GetColorBlendFactor())
    {
        hr = device->SetRenderState(D3DRS_BLENDFACTOR, D3DCOLOR_COLORVALUE(GetColorBlendFactor()[0], GetColorBlendFactor()[1], GetColorBlendFactor()[2], GetColorBlendFactor()[3]));
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_vColorBlendFactorD3D9 = GetColorBlendFactor();
        else
            return false;
    }

    if (m_eCullModeD3D9 != GetCullMode())
    {
        hr = device->SetRenderState(D3DRS_CULLMODE, RenderStateMappingD3D9[GetCullMode()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eCullModeD3D9 = GetCullMode();
        else
            return false;
    }

    if (m_eZEnabledD3D9 != GetZEnabled())
    {
        hr = device->SetRenderState(D3DRS_ZENABLE, RenderStateMappingD3D9[GetZEnabled()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eZEnabledD3D9 = GetZEnabled();
        else
            return false;
    }

    if (m_eZFuncD3D9 != GetZFunc())
    {
        hr = device->SetRenderState(D3DRS_ZFUNC, RenderStateMappingD3D9[GetZFunc()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eZFuncD3D9 = GetZFunc();
        else
            return false;
    }

    if (m_bZWriteEnabledD3D9 != GetZWriteEnabled())
    {
        hr = device->SetRenderState(D3DRS_ZWRITEENABLE, (DWORD)GetZWriteEnabled());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_bZWriteEnabledD3D9 = GetZWriteEnabled();
        else
            return false;
    }

    if (m_bColorWriteRedD3D9 != GetColorWriteRedEnabled() ||
        m_bColorWriteGreenD3D9 != GetColorWriteGreenEnabled() ||
        m_bColorWriteBlueD3D9 != GetColorWriteBlueEnabled() ||
        m_bColorWriteAlphaD3D9 != GetColorWriteAlphaEnabled())
    {
        DWORD colorWriteEnable = 
            ((GetColorWriteRedEnabled() * 0xf) & D3DCOLORWRITEENABLE_RED) |
            ((GetColorWriteGreenEnabled() * 0xf) & D3DCOLORWRITEENABLE_GREEN) |
            ((GetColorWriteBlueEnabled() * 0xf) & D3DCOLORWRITEENABLE_BLUE) |
            ((GetColorWriteAlphaEnabled() * 0xf) & D3DCOLORWRITEENABLE_ALPHA);
        hr = device->SetRenderState(D3DRS_COLORWRITEENABLE, colorWriteEnable);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
        {
            m_bColorWriteRedD3D9 = GetColorWriteRedEnabled();
            m_bColorWriteGreenD3D9 = GetColorWriteGreenEnabled();
            m_bColorWriteBlueD3D9 = GetColorWriteBlueEnabled();
            m_bColorWriteAlphaD3D9 = GetColorWriteAlphaEnabled();
        }
        else
            return false;
    }

    if (m_fSlopeScaledDepthBiasD3D9 != GetSlopeScaledDepthBias())
    {
        const float scale = GetSlopeScaledDepthBias();
        hr = device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&scale);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_fSlopeScaledDepthBiasD3D9 = GetSlopeScaledDepthBias();
        else
            return false;
    }

    if (m_fDepthBiasD3D9 != GetDepthBias())
    {
        //float dwBias = bias / 16777216.0f;
        const float bias = GetDepthBias();
        hr = device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&bias);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_fDepthBiasD3D9 = GetDepthBias();
        else
            return false;
    }

    if (m_bStencilEnabledD3D9 != GetStencilEnabled())
    {
        hr = device->SetRenderState(D3DRS_STENCILENABLE, (DWORD)GetStencilEnabled());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_bStencilEnabledD3D9 = GetStencilEnabled();
        else
            return false;
    }

    if (m_eStencilFuncD3D9 != GetStencilFunc())
    {
        hr = device->SetRenderState(D3DRS_STENCILFUNC, RenderStateMappingD3D9[GetStencilFunc()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eStencilFuncD3D9 = GetStencilFunc();
        else
            return false;
    }

    if (m_lStencilRefD3D9 != GetStencilRef())
    {
        hr = device->SetRenderState(D3DRS_STENCILREF, (DWORD)GetStencilRef());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_lStencilRefD3D9 = GetStencilRef();
        else
            return false;
    }

    if (m_lStencilMaskD3D9 != GetStencilMask())
    {
        hr = device->SetRenderState(D3DRS_STENCILMASK, (DWORD)GetStencilMask());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_lStencilMaskD3D9 = GetStencilMask();
        else
            return false;
    }

    if (m_lStencilWriteMaskD3D9 != GetStencilWriteMask())
    {
        hr = device->SetRenderState(D3DRS_STENCILWRITEMASK, (DWORD)GetStencilWriteMask());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_lStencilWriteMaskD3D9 = GetStencilWriteMask();
        else
            return false;
    }

    if (m_eStencilFailD3D9 != GetStencilFail())
    {
        hr = device->SetRenderState(D3DRS_STENCILFAIL, RenderStateMappingD3D9[GetStencilFail()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eStencilFailD3D9 = GetStencilFail();
        else
            return false;
    }

    if (m_eStencilZFailD3D9 != GetStencilZFail())
    {
        hr = device->SetRenderState(D3DRS_STENCILZFAIL, RenderStateMappingD3D9[GetStencilZFail()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eStencilZFailD3D9 = GetStencilZFail();
        else
            return false;
    }

    if (m_eStencilPassD3D9 != GetStencilPass())
    {
        hr = device->SetRenderState(D3DRS_STENCILPASS, RenderStateMappingD3D9[GetStencilPass()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eStencilPassD3D9 = GetStencilPass();
        else
            return false;
    }

    if (m_eFillModeD3D9 != GetFillMode())
    {
        hr = device->SetRenderState(D3DRS_FILLMODE, RenderStateMappingD3D9[GetFillMode()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eFillModeD3D9 = GetFillMode();
        else
            return false;
    }

    if (m_bScissorEnabledD3D9 != GetScissorEnabled())
    {
        hr = device->SetRenderState(D3DRS_SCISSORTESTENABLE, (DWORD)GetScissorEnabled());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_bScissorEnabledD3D9 = GetScissorEnabled();
        else
            return false;
    }

    if (m_bSRGBEnabledD3D9 != GetSRGBWriteEnabled())
    {
        hr = device->SetRenderState(D3DRS_SRGBWRITEENABLE, (DWORD)GetSRGBWriteEnabled());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_bSRGBEnabledD3D9 = GetSRGBWriteEnabled();
        else
            return false;
    }

    return true;
}
