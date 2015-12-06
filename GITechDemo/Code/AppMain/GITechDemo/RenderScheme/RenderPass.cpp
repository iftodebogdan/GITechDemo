/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	RenderPass.cpp
 *		Author:	Bogdan Iftode
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#include "stdafx.h"

#include <Renderer.h>
using namespace Synesthesia3D;

#include "GITechDemo.h"

#include "RenderPass.h"
using namespace GITechDemoApp;

RenderPass::RenderPass(const char* const passName, RenderPass* const parentPass)
	: m_szPassName(passName)
{
	if(parentPass)
		parentPass->AddChildPass(this);
}

RenderPass::~RenderPass()
{}

void RenderPass::AddChildPass(RenderPass* const childPass)
{
	m_arrChildList.push_back(childPass);
}

void RenderPass::Draw()
{
	DrawChildren();
}

void RenderPass::DrawChildren()
{
	for (unsigned int child = 0; child < m_arrChildList.size(); child++)
	{
		if (m_arrChildList[child] != nullptr)
		{
			PUSH_PROFILE_MARKER(m_arrChildList[child]->GetPassName());
			m_arrChildList[child]->Update(((GITechDemo*)AppMain)->GetDeltaTime());
			m_arrChildList[child]->Draw();
			POP_PROFILE_MARKER();
		}
	}
}