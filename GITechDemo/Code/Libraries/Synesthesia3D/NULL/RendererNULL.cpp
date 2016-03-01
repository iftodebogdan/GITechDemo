/**
 *	@file		RendererNULL.cpp
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *	@copyright
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *	@copyright
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *	@copyright
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"

#include "RendererNULL.h"
#include "ResourceManagerNULL.h"
#include "RenderStateNULL.h"
#include "SamplerStateNULL.h"
using namespace Synesthesia3D;

void RendererNULL::Initialize(void* /*hWnd*/)
{
	m_pResourceManager = new ResourceManagerNULL();
	m_pRenderStateManager = new RenderStateNULL();
	m_pSamplerStateManager = new SamplerStateNULL();

	m_pSamplerStateManager->Reset();
	m_pRenderStateManager->Reset();
}

void RendererNULL::CreatePerspectiveMatrix(Matrix44f& matProj, const float fovYRad, const float aspectRatio, const float zNear, const float zFar) const
{
	gmtl::setPerspective(matProj, gmtl::Math::rad2Deg(fovYRad), aspectRatio, zNear, zFar);
}

void RendererNULL::CreateOrthographicMatrix(Matrix44f& matProj, const float left, const float top, const float right, const float bottom, const float zNear, const float zFar) const
{
	gmtl::setOrtho(matProj, left, top, right, bottom, zNear, zFar);
}

void RendererNULL::PushProfileMarker(const char* const label)
{
	Renderer::PushProfileMarker(label);
}

void RendererNULL::PopProfileMarker()
{
	Renderer::PopProfileMarker();
}
