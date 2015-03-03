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

Renderer* Renderer::m_pInstance = nullptr;
API Renderer::m_eAPI = API_NULL;

Renderer::Renderer()
	: m_vViewportSize(800, 600)
	, m_vViewportOffset(0, 0)
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
	assert(m_pInstance == nullptr);

	switch (eApi)
	{
		case API_DX9:
			m_pInstance = new RendererDX9;
			m_eAPI = API_DX9;
			break;
		case API_NULL:
			m_pInstance = new RendererNULL;
			m_eAPI = API_NULL;
	}
}

void Renderer::DestroyInstance()
{
	if (m_pInstance)
	{
		m_pInstance->GetResourceManager()->ReleaseAll();
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void Renderer::ConvertOGLProjMatToD3D(Matrix44f& matProj)
{
	matProj[2][2] = matProj[2][2] * -1 - (matProj[2][2] + 1) / -2.f;
	matProj[3][2] *= -1;
	matProj[2][3] *= 0.5f;
}