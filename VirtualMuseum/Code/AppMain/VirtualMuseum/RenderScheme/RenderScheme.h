/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   RenderScheme.h
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

#ifndef RENDER_SCHEME_H_
#define RENDER_SCHEME_H_

#include "RenderPass.h"

namespace VirtualMuseumApp
{
    class RenderScheme
    {
    public:
        static RenderPass&  GetRootPass() { return RootPass; }
        static void         Draw() { RootPass.Draw(); }

        static void         AllocateResources() { RootPass.AllocateResources(); }
        static void         ReleaseResources() { RootPass.ReleaseResources(); }

    private:
        static RenderPass   RootPass;
    };
}

#endif //RENDER_SCHEME_H_