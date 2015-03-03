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
#include "stdafx.h"

#include "SamplerState.h"
using namespace LibRendererDll;

SamplerState::SamplerState()
{
	for (unsigned int i = 0; i < MAX_NUM_PSAMPLERS; i++)
	{
		m_tCurrentState[i].fAnisotropy = 1.f;
		m_tCurrentState[i].fLodBias = 0.f;
		m_tCurrentState[i].eFilter = SF_MIN_MAG_POINT_MIP_NONE;
		m_tCurrentState[i].vBorderColor = Vec4f(0.f, 0.f, 0.f, 0.f);
		for (unsigned int j = 0; j < 3; j++)
			m_tCurrentState[i].eAddressingMode[j] = SAM_WRAP;
	}
}

SamplerState::~SamplerState()
{}