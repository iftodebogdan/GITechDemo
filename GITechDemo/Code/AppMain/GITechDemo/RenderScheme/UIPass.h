/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   UIPass.h
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

#ifndef UI_PASS_H_
#define UI_PASS_H_

#include <vector>
#include <string>

#include "RenderPass.h"

namespace gainput
{
    class InputManager;
    class InputDeviceMouse;
    class InputDeviceKeyboard;
}

namespace Synesthesia3D
{
    class Texture;
    class VertexFormat;
    class IndexBuffer;
    class VertexBuffer;
}

namespace GITechDemoApp
{
    class ArtistParameter;
    struct GPUProfileMarkerResultCacheEntry;
    struct ParamCategoryWindowState;

    class UIPass : public RenderPass
    {
        IMPLEMENT_RENDER_PASS(UIPass)

    public:
        void SetupInput(gainput::InputManager* pInputManager);
        void AllocateResources();
        void ReleaseResources();

    private:
        void SetupUI();
        void GenerateDrawData();
        void RenderUI();

        void AddParameterInWindow(ArtistParameter* const param) const;
        void DrawGPUProfileBars(const RenderPass* pass = nullptr, const unsigned int level = 0);
        void DrawGPUProfileDetails(const RenderPass* pass = nullptr, const unsigned int level = 0) const;
        void CleanGPUProfileMarkerResultCache(const char* const passName);

        // UI states/parameters
        bool m_bShowAllParameters;
        bool m_bShowProfiler;
        float m_fAlpha;
        std::vector<ParamCategoryWindowState> m_arrParamCategoryWindowStates;
        std::vector<GPUProfileMarkerResultCacheEntry> m_arrGPUProfileMarkerResultCache;

        // Geometry resource data
        unsigned int m_nImGuiVfIdx;
        Synesthesia3D::VertexFormat* m_pImGuiVf;
        unsigned int m_nImGuiIbIdx;
        Synesthesia3D::IndexBuffer* m_pImGuiIb;
        unsigned int m_nImGuiVbIdx;
        Synesthesia3D::VertexBuffer* m_pImGuiVb;

        // Font texture data
        Synesthesia3D::Texture* m_pFontTexture;
        unsigned int m_nFontTextureIdx;

        // Input devices
        gainput::InputDeviceKeyboard* m_pKeyboardDevice;
        gainput::InputDeviceMouse* m_pMouseDevice;
    };

    struct GPUProfileMarkerResultCacheEntry
    {
        GPUProfileMarkerResultCacheEntry(
            std::string _name, float _timing, float _start, float _end,
            float _rootTiming, float _rootStart, float _rootEnd)
            : name(_name), timing(_timing), start(_start), end(_end)
            , rootTiming(_rootTiming), rootStart(_rootStart), rootEnd(_rootEnd)
        {}

        std::string name;
        float timing, start, end;
        float rootTiming, rootStart, rootEnd;
    };

    struct ParamCategoryWindowState
    {
        std::string categoryName;
        bool windowOpen;
    };
}

#endif //UI_PASS_H_