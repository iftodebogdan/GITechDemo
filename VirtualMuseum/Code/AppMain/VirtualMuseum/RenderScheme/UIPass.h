/*=============================================================================
 * This file is part of the "VirtualMuseum" application
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

namespace VirtualMuseumApp
{
    #define UI_BUFFER_COUNT (2)

    class ArtistParameter;

    struct UIDrawData
    {
        struct UIDrawCommandList
        {
            struct UIDrawCommand
            {
                Vec4f ClipRect;
                unsigned int ElemCount;
                Synesthesia3D::Texture* Texture;
            };

            int VtxBufferSize;
            std::vector<UIDrawCommand> DrawCmd;
        };

        std::vector<UIDrawCommandList> CmdList;
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

    struct GPUFrametimeHistoryEntry
    {
        GPUFrametimeHistoryEntry(float _timing, float _start)
            : timing(_timing)
            , start(_start)
        {}
        float timing, start;
    };

    struct ParamCategoryWindowState
    {
        std::string categoryName;
        bool windowOpen;
    };

    class GPUProfileMarkerResultHistory
    {
    public:
        GPUProfileMarkerResultHistory()
            : m_nCurrBufferIdx(0u)
            , m_fTimeAccum(0.f)
        {}

        void Update(const float fDeltaTime);
        const float GetAverage(const char* const name) const;
        void PushMarker(const GPUProfileMarkerResultCacheEntry& marker);

    private:
        std::vector<GPUProfileMarkerResultCacheEntry> m_arrGPUProfileMarkerResultHistory[2];
        unsigned int m_nCurrBufferIdx;
        float m_fTimeAccum;
    };

    class UIPass : public RenderPass
    {
        IMPLEMENT_RENDER_PASS(UIPass)

    public:
        void SetupInput(gainput::InputManager* pInputManager);

        void AddTooltip(const char* const tip, const Vec2f screenPos);

    private:
        void SetupUI();
        void GenerateDrawData();
        void RenderUI();

        void AddParameterInWindow(ArtistParameter* const param) const;
        void DrawGPUFrametimeGraph();
        void DrawGPUProfileBars(const RenderPass* pass = nullptr, const unsigned int level = 0);
        void DrawGPUProfileDetails(const RenderPass* pass = nullptr, const unsigned int level = 0) const;
        void CleanGPUProfileMarkerResultCache(const char* const passName);

        // UI states/parameters
        bool m_bShowAllParameters;
        bool m_bShowProfiler;
        bool m_bShowTextureViewer;
        float m_fAlpha;
        std::vector<ParamCategoryWindowState> m_arrParamCategoryWindowStates;
        std::vector<GPUProfileMarkerResultCacheEntry> m_arrGPUProfileMarkerResultCache;
        std::vector<GPUFrametimeHistoryEntry> m_arrGPUFrametimeHistory;

        // Geometry resource data
        unsigned int m_nCurrBufferIdx;
        unsigned int m_nImGuiVfIdx[UI_BUFFER_COUNT];
        Synesthesia3D::VertexFormat* m_pImGuiVf[UI_BUFFER_COUNT];
        unsigned int m_nImGuiIbIdx[UI_BUFFER_COUNT];
        Synesthesia3D::IndexBuffer* m_pImGuiIb[UI_BUFFER_COUNT];
        unsigned int m_nImGuiVbIdx[UI_BUFFER_COUNT];
        Synesthesia3D::VertexBuffer* m_pImGuiVb[UI_BUFFER_COUNT];

        // Draw data
        UIDrawData m_tDrawData[UI_BUFFER_COUNT];

        // Font texture data
        Synesthesia3D::Texture* m_pFontTexture;
        unsigned int m_nFontTextureIdx;

        int m_nTextureViewerIdx;
        int m_nSelectedMip;
        int m_nSelectedSlice;
        int m_nSelectedFace;
        float m_fSelectedSlice;
        Vec<bool, 3u> m_bChannelMask;

        // Placeholder textures
        Synesthesia3D::s3dSampler m_nDummyTex1DIdx;
        Synesthesia3D::s3dSampler m_nDummyTex2DIdx;
        Synesthesia3D::s3dSampler m_nDummyTex3DIdx;
        Synesthesia3D::s3dSampler m_nDummyTexCubeIdx;
        Synesthesia3D::Texture* m_pDummyTex1D;
        Synesthesia3D::Texture* m_pDummyTex2D;
        Synesthesia3D::Texture* m_pDummyTex3D;
        Synesthesia3D::Texture* m_pDummyTexCube;

        // Input devices
        gainput::InputDeviceKeyboard* m_pKeyboardDevice;
        gainput::InputDeviceMouse* m_pMouseDevice;

        GPUProfileMarkerResultHistory m_tGPUProfileMarkerResultHistory;

        struct Tooltip
        {
            Tooltip(const char* const tip, const Vec2f screenPos) : text(tip), pos(screenPos) {}
            std::string text;
            Vec2f pos;
        };
        std::vector<Tooltip> m_tooltips;
    };
}

#endif //UI_PASS_H_
