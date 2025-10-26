/**
 * @file        RenderStateNVRHI.h
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

#ifndef RENDERSTATENVRHI_H
#define RENDERSTATENVRHI_H

#include "ResourceData.h"

#if ENABLE_NVRHI

#include "RenderState.h"

namespace Synesthesia3D
{
    class RenderStateNVRHI : public RenderState
    {
    public:
        const bool SetScissor(const Vec2i size, const Vec2i offset = Vec2i(0, 0));

        void Reset();

    protected:
        RenderStateNVRHI();
        ~RenderStateNVRHI();

        const bool Flush();

        friend class RendererNVRHI;
    };
}

#endif // ENABLE_NVRHI

#endif // RENDERSTATENVRHI_H
