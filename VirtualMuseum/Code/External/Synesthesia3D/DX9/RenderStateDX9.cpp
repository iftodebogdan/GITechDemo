/**
 * @file        RenderStateDX9.cpp
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

#include "RendererDX9.h"
#include "MappingsDX9.h"
#include "RenderStateDX9.h"
#include "ProfilerDX9.h"
using namespace Synesthesia3D;

RenderStateDX9::RenderStateDX9()
{
    Reset();
}

RenderStateDX9::~RenderStateDX9()
{}

const unsigned int RenderStateDX9::MatchRenderState(const DWORD rs, const unsigned int rsEnumClass) const
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
        if (RenderStateMappingDX9[i] == rs)
            return i;
    }

    assert(false);
    return 0;
}

const bool RenderStateDX9::SetScissor(const Vec2i size, const Vec2i offset)
{
    IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
    const RECT scissorRect = { offset[0], offset[1], offset[0] + size[0], offset[1] + size[1] };
    HRESULT hr = device->SetScissorRect(&scissorRect);
    assert(SUCCEEDED(hr));
    return SUCCEEDED(hr);
}

void RenderStateDX9::Reset()
{
    IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
    HRESULT hr;
    DWORD value;



    hr = device->GetRenderState(D3DRS_ALPHABLENDENABLE, &value);
    assert(SUCCEEDED(hr));
    m_bColorBlendEnabledDX9 = (value != 0);

    hr = device->GetRenderState(D3DRS_SRCBLEND, &value);
    assert(SUCCEEDED(hr));
    m_eColorSrcBlendDX9 = (Blend)MatchRenderState(value, BLEND);

    hr = device->GetRenderState(D3DRS_DESTBLEND, &value);
    assert(SUCCEEDED(hr));
    m_eColorDstBlendDX9 = (Blend)MatchRenderState(value, BLEND);

    hr = device->GetRenderState(D3DRS_BLENDFACTOR, &value);
    assert(SUCCEEDED(hr));
    m_vColorBlendFactorDX9 = Vec4f(
        (float)((value & (0xff << 16)) >> 16) / 255.f,  // red
        (float)((value & (0xff << 8)) >> 8) / 255.f,    // green
        (float)(value & 0xff) / 255.f,                  // blue
        (float)((value & (0xff << 24)) >> 24) / 255.f); // alpha



    hr = device->GetRenderState(D3DRS_ALPHATESTENABLE, &value);
    assert(SUCCEEDED(hr));
    m_bAlphaTestEnabledDX9 = (value != 0);

    hr = device->GetRenderState(D3DRS_ALPHAFUNC, &value);
    assert(SUCCEEDED(hr));
    m_eAlphaFuncDX9 = (Cmp)MatchRenderState(value, CMP);

    hr = device->GetRenderState(D3DRS_ALPHAREF, &value);
    assert(SUCCEEDED(hr));
    m_fAlphaRefDX9 = (float)value / 255.f;



    hr = device->GetRenderState(D3DRS_CULLMODE, &value);
    assert(SUCCEEDED(hr));
    m_eCullModeDX9 = (Cull)MatchRenderState(value, CULL);



    hr = device->GetRenderState(D3DRS_ZENABLE, &value);
    assert(SUCCEEDED(hr));
    m_eZEnabledDX9 = (ZBuffer)MatchRenderState(value, ZB);

    hr = device->GetRenderState(D3DRS_ZFUNC, &value);
    assert(SUCCEEDED(hr));
    m_eZFuncDX9 = (Cmp)MatchRenderState(value, CMP);

    hr = device->GetRenderState(D3DRS_ZWRITEENABLE, &value);
    assert(SUCCEEDED(hr));
    m_bZWriteEnabledDX9 = (value != 0);


    hr = device->GetRenderState(D3DRS_COLORWRITEENABLE, &value);
    assert(SUCCEEDED(hr));
    m_bColorWriteRedDX9     = (value & D3DCOLORWRITEENABLE_RED) != 0;
    m_bColorWriteGreenDX9   = (value & D3DCOLORWRITEENABLE_GREEN) != 0;
    m_bColorWriteBlueDX9    = (value & D3DCOLORWRITEENABLE_BLUE) != 0;
    m_bColorWriteAlphaDX9   = (value & D3DCOLORWRITEENABLE_ALPHA) != 0;


    hr = device->GetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, &value);
    assert(SUCCEEDED(hr));
    m_fSlopeScaledDepthBiasDX9 = *(float*)&value;

    hr = device->GetRenderState(D3DRS_DEPTHBIAS, &value);
    assert(SUCCEEDED(hr));
    m_fDepthBiasDX9 = *(float*)&value;// *16777216.0f;



    hr = device->GetRenderState(D3DRS_STENCILENABLE, &value);
    assert(SUCCEEDED(hr));
    m_bStencilEnabledDX9 = (value != 0);

    hr = device->GetRenderState(D3DRS_STENCILFUNC, &value);
    assert(SUCCEEDED(hr));
    m_eStencilFuncDX9 = (Cmp)MatchRenderState(value, CMP);

    hr = device->GetRenderState(D3DRS_STENCILREF, &value);
    assert(SUCCEEDED(hr));
    m_lStencilRefDX9 = value;

    hr = device->GetRenderState(D3DRS_STENCILMASK, &value);
    assert(SUCCEEDED(hr));
    m_lStencilMaskDX9 = value;

    hr = device->GetRenderState(D3DRS_STENCILWRITEMASK, &value);
    assert(SUCCEEDED(hr));
    m_lStencilWriteMaskDX9 = value;

    hr = device->GetRenderState(D3DRS_STENCILFAIL, &value);
    assert(SUCCEEDED(hr));
    m_eStencilFailDX9 = (StencilOp)MatchRenderState(value, STENCILOP);

    hr = device->GetRenderState(D3DRS_STENCILZFAIL, &value);
    assert(SUCCEEDED(hr));
    m_eStencilZFailDX9 = (StencilOp)MatchRenderState(value, STENCILOP);

    hr = device->GetRenderState(D3DRS_STENCILPASS, &value);
    assert(SUCCEEDED(hr));
    m_eStencilPassDX9 = (StencilOp)MatchRenderState(value, STENCILOP);



    hr = device->GetRenderState(D3DRS_FILLMODE, &value);
    assert(SUCCEEDED(hr));
    m_eFillModeDX9 = (Fill)MatchRenderState(value, FILL);



    hr = device->GetRenderState(D3DRS_SCISSORTESTENABLE, &value);
    assert(SUCCEEDED(hr));
    m_bScissorEnabledDX9 = (value != 0);



    hr = device->GetRenderState(D3DRS_SRGBWRITEENABLE, &value);
    assert(SUCCEEDED(hr));
    m_bSRGBEnabledDX9 = (value != 0);



    RenderState::Reset();
}

const bool RenderStateDX9::Flush()
{
    IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
    HRESULT hr = E_FAIL;

    if (m_bColorBlendEnabledDX9 != GetColorBlendEnabled())
    {
        hr = device->SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)GetColorBlendEnabled());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_bColorBlendEnabledDX9 = GetColorBlendEnabled();
        else
            return false;
    }

    if (m_eColorSrcBlendDX9 != GetColorSrcBlend())
    {
        hr = device->SetRenderState(D3DRS_SRCBLEND/*ALPHA*/, RenderStateMappingDX9[GetColorSrcBlend()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eColorSrcBlendDX9 = GetColorSrcBlend();
        else
            return false;
    }

    if (m_eColorDstBlendDX9 != GetColorDstBlend())
    {
        hr = device->SetRenderState(D3DRS_DESTBLEND/*ALPHA*/, RenderStateMappingDX9[GetColorDstBlend()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eColorDstBlendDX9 = GetColorDstBlend();
        else
            return false;
    }

    if (m_bAlphaTestEnabledDX9 != GetAlphaTestEnabled())
    {
        hr = device->SetRenderState(D3DRS_ALPHATESTENABLE, (DWORD)GetAlphaTestEnabled());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_bAlphaTestEnabledDX9 = GetAlphaTestEnabled();
        else
            return false;
    }

    if (m_eAlphaFuncDX9 != GetAlphaTestFunc())
    {
        hr = device->SetRenderState(D3DRS_ALPHAFUNC, RenderStateMappingDX9[GetAlphaTestFunc()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eAlphaFuncDX9 = GetAlphaTestFunc();
        else
            return false;
    }

    if (m_fAlphaRefDX9 != GetAlphaTestRef())
    {
        hr = device->SetRenderState(D3DRS_ALPHAREF, (DWORD)(255.f * GetAlphaTestRef()));
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_fAlphaRefDX9 = GetAlphaTestRef();
        else
            return false;
    }

    if (m_vColorBlendFactorDX9 != GetColorBlendFactor())
    {
        hr = device->SetRenderState(D3DRS_BLENDFACTOR, D3DCOLOR_COLORVALUE(GetColorBlendFactor()[0], GetColorBlendFactor()[1], GetColorBlendFactor()[2], GetColorBlendFactor()[3]));
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_vColorBlendFactorDX9 = GetColorBlendFactor();
        else
            return false;
    }

    if (m_eCullModeDX9 != GetCullMode())
    {
        hr = device->SetRenderState(D3DRS_CULLMODE, RenderStateMappingDX9[GetCullMode()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eCullModeDX9 = GetCullMode();
        else
            return false;
    }

    if (m_eZEnabledDX9 != GetZEnabled())
    {
        hr = device->SetRenderState(D3DRS_ZENABLE, RenderStateMappingDX9[GetZEnabled()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eZEnabledDX9 = GetZEnabled();
        else
            return false;
    }

    if (m_eZFuncDX9 != GetZFunc())
    {
        hr = device->SetRenderState(D3DRS_ZFUNC, RenderStateMappingDX9[GetZFunc()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eZFuncDX9 = GetZFunc();
        else
            return false;
    }

    if (m_bZWriteEnabledDX9 != GetZWriteEnabled())
    {
        hr = device->SetRenderState(D3DRS_ZWRITEENABLE, (DWORD)GetZWriteEnabled());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_bZWriteEnabledDX9 = GetZWriteEnabled();
        else
            return false;
    }

    if (m_bColorWriteRedDX9 != GetColorWriteRedEnabled() ||
        m_bColorWriteGreenDX9 != GetColorWriteGreenEnabled() ||
        m_bColorWriteBlueDX9 != GetColorWriteBlueEnabled() ||
        m_bColorWriteAlphaDX9 != GetColorWriteAlphaEnabled())
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
            m_bColorWriteRedDX9 = GetColorWriteRedEnabled();
            m_bColorWriteGreenDX9 = GetColorWriteGreenEnabled();
            m_bColorWriteBlueDX9 = GetColorWriteBlueEnabled();
            m_bColorWriteAlphaDX9 = GetColorWriteAlphaEnabled();
        }
        else
            return false;
    }

    if (m_fSlopeScaledDepthBiasDX9 != GetSlopeScaledDepthBias())
    {
        const float scale = GetSlopeScaledDepthBias();
        hr = device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&scale);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_fSlopeScaledDepthBiasDX9 = GetSlopeScaledDepthBias();
        else
            return false;
    }

    if (m_fDepthBiasDX9 != GetDepthBias())
    {
        //float dwBias = bias / 16777216.0f;
        const float bias = GetDepthBias();
        hr = device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&bias);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_fDepthBiasDX9 = GetDepthBias();
        else
            return false;
    }

    if (m_bStencilEnabledDX9 != GetStencilEnabled())
    {
        hr = device->SetRenderState(D3DRS_STENCILENABLE, (DWORD)GetStencilEnabled());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_bStencilEnabledDX9 = GetStencilEnabled();
        else
            return false;
    }

    if (m_eStencilFuncDX9 != GetStencilFunc())
    {
        hr = device->SetRenderState(D3DRS_STENCILFUNC, RenderStateMappingDX9[GetStencilFunc()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eStencilFuncDX9 = GetStencilFunc();
        else
            return false;
    }

    if (m_lStencilRefDX9 != GetStencilRef())
    {
        hr = device->SetRenderState(D3DRS_STENCILREF, (DWORD)GetStencilRef());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_lStencilRefDX9 = GetStencilRef();
        else
            return false;
    }

    if (m_lStencilMaskDX9 != GetStencilMask())
    {
        hr = device->SetRenderState(D3DRS_STENCILMASK, (DWORD)GetStencilMask());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_lStencilMaskDX9 = GetStencilMask();
        else
            return false;
    }

    if (m_lStencilWriteMaskDX9 != GetStencilWriteMask())
    {
        hr = device->SetRenderState(D3DRS_STENCILWRITEMASK, (DWORD)GetStencilWriteMask());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_lStencilWriteMaskDX9 = GetStencilWriteMask();
        else
            return false;
    }

    if (m_eStencilFailDX9 != GetStencilFail())
    {
        hr = device->SetRenderState(D3DRS_STENCILFAIL, RenderStateMappingDX9[GetStencilFail()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eStencilFailDX9 = GetStencilFail();
        else
            return false;
    }

    if (m_eStencilZFailDX9 != GetStencilZFail())
    {
        hr = device->SetRenderState(D3DRS_STENCILZFAIL, RenderStateMappingDX9[GetStencilZFail()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eStencilZFailDX9 = GetStencilZFail();
        else
            return false;
    }

    if (m_eStencilPassDX9 != GetStencilPass())
    {
        hr = device->SetRenderState(D3DRS_STENCILPASS, RenderStateMappingDX9[GetStencilPass()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eStencilPassDX9 = GetStencilPass();
        else
            return false;
    }

    if (m_eFillModeDX9 != GetFillMode())
    {
        hr = device->SetRenderState(D3DRS_FILLMODE, RenderStateMappingDX9[GetFillMode()]);
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_eFillModeDX9 = GetFillMode();
        else
            return false;
    }

    if (m_bScissorEnabledDX9 != GetScissorEnabled())
    {
        hr = device->SetRenderState(D3DRS_SCISSORTESTENABLE, (DWORD)GetScissorEnabled());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_bScissorEnabledDX9 = GetScissorEnabled();
        else
            return false;
    }

    if (m_bSRGBEnabledDX9 != GetSRGBWriteEnabled())
    {
        hr = device->SetRenderState(D3DRS_SRGBWRITEENABLE, (DWORD)GetSRGBWriteEnabled());
        assert(SUCCEEDED(hr));

        if (SUCCEEDED(hr))
            m_bSRGBEnabledDX9 = GetSRGBWriteEnabled();
        else
            return false;
    }

    return true;
}
