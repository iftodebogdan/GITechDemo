//////////////////////////////////////////////////////////////////////////
// This file is part of the "LibRenderer" 3D graphics library           //
//                                                                      //
// Copyright (C) 2014 - Iftode Bogdan-Marius <iftode.bogdan@gmail.com>  //
//                                                                      //
// This program is free software: you can redistribute it and/or modify //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// This program is distributed in the hope that it will be useful,      //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program. If not, see <http://www.gnu.org/licenses/>. //
//////////////////////////////////////////////////////////////////////////

// LibRenderer.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "Renderer.h"
#include "ResourceManager.h"
#include "RenderState.h"
#include "SamplerState.h"

#ifdef _WIN32
#include "RendererDX9.h"
#include "RendererNULL.h"
#endif

using namespace LibRendererDll;

Renderer* Renderer::ms_pInstance = nullptr;
API Renderer::ms_eAPI = API_NONE;
int Renderer::ms_nProfileMarkerCounter = 0;

Renderer::Renderer()
	: m_vBackBufferSize(800, 600)
	, m_vBackBufferOffset(0, 0)
	, m_pResourceManager(nullptr)
	, m_pRenderState(nullptr)
	, m_pSamplerState(nullptr)
{}

Renderer::~Renderer()
{
	if (m_pResourceManager)
		delete m_pResourceManager;

	if (m_pRenderState)
		delete m_pRenderState;

	if (m_pSamplerState)
		delete m_pSamplerState;
}

void Renderer::CreateInstance(API eApi)
{
	assert(ms_pInstance == nullptr);
	if (ms_pInstance != nullptr)
		return;

	switch (eApi)
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
	#if ENABLE_PROFILE_MARKERS
		assert(ms_nProfileMarkerCounter == 0);
	#endif
		ms_pInstance->GetResourceManager()->ReleaseAll();
		Renderer* tmp = ms_pInstance;
		ms_pInstance = nullptr;
		delete tmp;
	}
}

Renderer* Renderer::GetInstance()
{
	return ms_pInstance;
}

API Renderer::GetAPI()
{
	return ms_eAPI;
}

void Renderer::SetBackBufferSize(const Vec2i size, const Vec2i offset)
{
	m_vBackBufferSize = size;
	m_vBackBufferOffset = offset;
}

Vec2i Renderer::GetBackBufferSize()
{
	return m_vBackBufferSize;
}

Vec2i Renderer::GetBackBufferOffset()
{
	return m_vBackBufferOffset;
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


ResourceManager* Renderer::GetResourceManager()
{
	return m_pResourceManager;
}

RenderState* Renderer::GetRenderStateManager()
{
	return m_pRenderState;
}

SamplerState* Renderer::GetSamplerStateManager()
{
	return m_pSamplerState;
}

DeviceCaps Renderer::GetDeviceCaps()
{
	return m_tDeviceCaps;
}

void Renderer::PushProfileMarker(const char* const /*label*/)
{
	ms_nProfileMarkerCounter++;
}

void Renderer::PopProfileMarker()
{
	ms_nProfileMarkerCounter--;
}