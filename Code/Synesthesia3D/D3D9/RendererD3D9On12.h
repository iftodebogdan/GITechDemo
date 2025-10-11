/**
 * @file        RendererD3D9On12.h
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

#ifndef RENDERERD3D9ON12_H
#define RENDERERD3D9ON12_H


#include "RendererD3D9.h"

#if ENABLE_D3D9_ON_12

#include <dxgi.h>
#include <d3d9on12.h>

namespace Synesthesia3D
{
    class RendererD3D9On12 : public RendererD3D9
    {
    protected:
        RendererD3D9On12();
        ~RendererD3D9On12();

        IDirect3D9* CreateDriver();
        void ValidateDevice();

    private:
        const bool LoadDLLFunctions();
        IDXGIAdapter1* CreateD3D12Adapter();

        IDirect3D9* WINAPI Direct3DCreate9On12(UINT SDKVersion, D3D9ON12_ARGS* pOverrideList, UINT NumOverrideEntries);

        PFN_Direct3DCreate9On12 m_pfnDirect3DCreate9On12 = nullptr;

        friend class Renderer;
    };
}

#endif //ENABLE_D3D9_ON_12

#endif //RENDERD3D9ON12_H
