/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   UI3DPass.cpp
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

#include "stdafx.h"

#include "UI3DPass.h"
#include "Scene.h"
#include "VirtualMuseum.h"
using namespace VirtualMuseumApp;

UI3DPass::UI3DPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

UI3DPass::~UI3DPass()
{}

void UI3DPass::Update(float fDeltaTime)
{
}

void UI3DPass::Draw()
{
    ((VirtualMuseum*)AppMain)->GetScene()->Draw(Scene::Actor::UI_3D);
}

void UI3DPass::AllocateResources()
{

}

void UI3DPass::ReleaseResources()
{

}
