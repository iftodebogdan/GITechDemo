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
#ifndef RENDERSTATENULL_H
#define RENDERSTATENULL_H

#include "RenderState.h"

namespace LibRendererDll
{
	class RenderStateNULL : public RenderState
	{
	protected:
		RenderStateNULL() {}
		~RenderStateNULL() {}

		const bool	SetScissor(const Vec2i /*size*/, const Vec2i /*offset = Vec2i(0, 0)*/) { return true; }

		friend class RendererNULL;
	};
}

#endif // RENDERSTATENULL_H