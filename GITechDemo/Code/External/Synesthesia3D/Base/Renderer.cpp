/**
 * @file        Renderer.cpp
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

#include "Renderer.h"
#include "ResourceManager.h"
#include "RenderState.h"
#include "SamplerState.h"
#include "Profiler.h"
using namespace Synesthesia3D;

#ifdef _WINDOWS
#include "RendererDX9.h"
#endif

#include "RendererNULL.h"

using namespace Synesthesia3D;

Renderer* Renderer::ms_pInstance = nullptr;
API Renderer::ms_eAPI = API_NONE;

Renderer::Renderer()
    : m_vBackBufferOffset(0, 0)
    , m_pResourceManager(nullptr)
    , m_pRenderStateManager(nullptr)
    , m_pSamplerStateManager(nullptr)
    , m_pProfiler(nullptr)
    , m_eDeviceState(DS_NOT_READY)
{

}

Renderer::~Renderer()
{
    if (m_pResourceManager)
        delete m_pResourceManager;

    if (m_pRenderStateManager)
        delete m_pRenderStateManager;

    if (m_pSamplerStateManager)
        delete m_pSamplerStateManager;

    if (m_pProfiler)
        delete m_pProfiler;
}

void Renderer::CreateInstance(API api)
{
    assert(ms_pInstance == nullptr);
    if (ms_pInstance != nullptr)
        return;

    switch (api)
    {
        case API_DX9:
            ms_pInstance = new RendererDX9;
            ms_eAPI = API_DX9;
            break;
        case API_NULL:
            ms_pInstance = new RendererNULL;
            ms_eAPI = API_NULL;
            break;
        default:
            assert(false);
    }
}

void Renderer::DestroyInstance()
{
    if (ms_pInstance)
    {
        ms_pInstance->GetResourceManager()->ReleaseAll();
        Renderer* tmp = ms_pInstance;
        ms_pInstance = nullptr;
        delete tmp;
    }
}

Renderer* const Renderer::GetInstance()
{
    return ms_pInstance;
}

const API Renderer::GetAPI()
{
    return ms_eAPI;
}

const Vec2i Renderer::GetDisplayOffset() const
{
    return m_vBackBufferOffset;
}

void Renderer::ValidateDisplayResolution(Vec2i& size, unsigned int& refreshRate) const
{
    if (GetAPI() == API_NULL)
        return;

    if (refreshRate == 0)
        refreshRate = UINT_MAX;

    Vec2i bestMatch(0, 0);
    unsigned int bestRRMatch = 0;
    for (unsigned int i = 0; i < m_tDeviceCaps.arrSupportedScreenFormats.size(); i++)
    {
        if (GetBackBufferFormat() == m_tDeviceCaps.arrSupportedScreenFormats[i].ePixelFormat)
        {
            if ((int)m_tDeviceCaps.arrSupportedScreenFormats[i].nWidth == size[0] &&
                (int)m_tDeviceCaps.arrSupportedScreenFormats[i].nHeight == size[1] &&
                m_tDeviceCaps.arrSupportedScreenFormats[i].nRefreshRate == refreshRate)
            {
                return;
            }
            else if (((int)m_tDeviceCaps.arrSupportedScreenFormats[i].nWidth >= bestMatch[0] &&
                (int)m_tDeviceCaps.arrSupportedScreenFormats[i].nWidth <= size[0]) ||
                
                ((int)m_tDeviceCaps.arrSupportedScreenFormats[i].nHeight >= bestMatch[1] &&
                (int)m_tDeviceCaps.arrSupportedScreenFormats[i].nHeight <= size[1]))
            {
                bestMatch[0] = m_tDeviceCaps.arrSupportedScreenFormats[i].nWidth;
                bestMatch[1] = m_tDeviceCaps.arrSupportedScreenFormats[i].nHeight;
                
                if ((m_tDeviceCaps.arrSupportedScreenFormats[i].nRefreshRate > bestRRMatch &&
                    m_tDeviceCaps.arrSupportedScreenFormats[i].nRefreshRate <= refreshRate) ||
                    (bestRRMatch > refreshRate && m_tDeviceCaps.arrSupportedScreenFormats[i].nRefreshRate < bestRRMatch) ||
                    bestRRMatch == 0)
                {
                    bestRRMatch = m_tDeviceCaps.arrSupportedScreenFormats[i].nRefreshRate;
                }
            }
        }
    }

    size = bestMatch;
    refreshRate = bestRRMatch;
}

void Renderer::ConvertOGLProjMatToD3D(Matrix44f& matProj)
{
    matProj[2][2] = matProj[2][2] * -1 - (matProj[2][2] + 1) / -2.f;
    matProj[3][2] *= -1;
    matProj[2][3] *= 0.5f;
}

void Renderer::ConvertOGLProjMatToD3D(Matrix44f* const matProj)
{
    ConvertOGLProjMatToD3D(*matProj);
}


ResourceManager* const Renderer::GetResourceManager() const
{
    return m_pResourceManager;
}

RenderState* const Renderer::GetRenderStateManager() const
{
    return m_pRenderStateManager;
}

SamplerState* const Renderer::GetSamplerStateManager() const
{
    return m_pSamplerStateManager;
}

Profiler* const Renderer::GetProfiler() const
{
    return m_pProfiler;
}

const DeviceCaps& Renderer::GetDeviceCaps() const
{
    return m_tDeviceCaps;
}

void Renderer::DrawVertexBuffer(VertexBuffer* const vb, const unsigned int vtxOffset, const unsigned int primCount, const unsigned int vtxCount, const unsigned int idxOffset)
{
    GetSamplerStateManager()->Flush();
    GetRenderStateManager()->Flush();
}

const bool Renderer::BeginFrame()
{
    SetDeviceState(DS_RENDERING);
    return true;
}

void Renderer::EndFrame()
{
    SetDeviceState(DS_PRESENTING);
}

void Renderer::SwapBuffers()
{
    SetDeviceState(DS_READY);
}

const DeviceState Renderer::GetDeviceState() const
{
    return m_eDeviceState;
}

void Renderer::SetDeviceState(const DeviceState deviceState)
{
    m_eDeviceState = deviceState;
}

const char* Renderer::GetEnumString(PixelFormat val)
{
    switch (val)
    {
    case PF_NONE:
        return "NULL";
    case PF_R5G6B5:
        return "R5G6B5";
    case PF_A1R5G5B5:
        return "A1R5G5B5";
    case PF_A4R4G4B4:
        return "A4R4G4B4";
    case PF_A8:
        return "A8";
    case PF_L8:
        return "L8";
    case PF_A8L8:
        return "A8L8";
    case PF_R8G8B8:
        return "R8G8B8";
    case PF_X8R8G8B8:
        return "X8R8G8B8";
    case PF_A8R8G8B8:
        return "A8R8G8B8";
    case PF_A8B8G8R8:
        return "A8B8G8R8";
    case PF_L16:
        return "L16";
    case PF_G16R16:
        return "G16R16";
    case PF_A16B16G16R16:
        return "A16B16G16R16";
    case PF_R16F:
        return "R16F";
    case PF_G16R16F:
        return "G16R16F";
    case PF_A16B16G16R16F:
        return "A16B16G16R16F";
    case PF_R32F:
        return "R32F";
    case PF_G32R32F:
        return "G32R32F";
    case PF_A32B32G32R32F:
        return "A32B32G32R32F";
    case PF_DXT1:
        return "DXT1";
    case PF_DXT3:
        return "DXT3";
    case PF_DXT5:
        return "DXT5";
    case PF_D24S8:
        return "D24S8";
    case PF_INTZ:
        return "INTZ";
    default:
        assert(false);
        return "";
    }
}

const char* Renderer::GetEnumString(TextureType val)
{
    switch (val)
    {
    case TT_1D:
        return "1D";
    case TT_2D:
        return "2D";
    case TT_3D:
        return "3D";
    case TT_CUBE:
        return "Cubemap";
    default:
        assert(false);
        return "";
    }
}

const char* Renderer::GetEnumString(SamplerFilter val)
{
    switch (val)
    {
    case SF_MIN_MAG_POINT_MIP_NONE:
        return "MIN_MAG_POINT_MIP_NONE";
    case SF_MIN_MAG_LINEAR_MIP_NONE:
        return "MIN_MAG_LINEAR_MIP_NONE";
    case SF_MIN_MAG_POINT_MIP_POINT:
        return "MIN_MAG_POINT_MIP_POINT";
    case SF_MIN_MAG_POINT_MIP_LINEAR:
        return "MIN_MAG_POINT_MIP_LINEAR";
    case SF_MIN_MAG_LINEAR_MIP_POINT:
        return "MIN_MAG_LINEAR_MIP_POINT";
    case SF_MIN_MAG_LINEAR_MIP_LINEAR:
        return "MIN_MAG_LINEAR_MIP_LINEAR";
    default:
        assert(false);
        return "";
    }
}

const char* Renderer::GetEnumString(SamplerAddressingMode val)
{
    switch (val)
    {
    case SAM_CLAMP:
        return "clamp";
    case SAM_WRAP:
        return "wrap";
    case SAM_MIRROR:
        return "mirror";
    case SAM_BORDER:
        return "border";
    default:
        assert(false);
        return "";
    }
}

const char* Renderer::GetEnumString(CubeFace val)
{
    switch (val)
    {
    case FACE_XNEG:
        return "X-";
    case FACE_YPOS:
        return "Y+";
    case FACE_YNEG:
        return "Y-";
    case FACE_ZPOS:
        return "Z+";
    case FACE_XPOS:
        return "X+";
    case FACE_ZNEG:
        return "Z-";
    default:
        assert(false);
        return "";
    }
}
