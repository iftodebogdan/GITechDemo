/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   UIPass.cpp
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

#include <imgui.h>
#include <gainput/gainput.h>

#include <Renderer.h>
#include <Profiler.h>
#include <ResourceManager.h>
#include <Texture.h>
#include <RenderState.h>
#include <VertexFormat.h>
#include <IndexBuffer.h>
#include <VertexBuffer.h>
#include <RenderTarget.h>
using namespace Synesthesia3D;

#include <Utility/Hash.h>

#include "Framework.h"
using namespace AppFramework;

#include "UIPass.h"
#include "App.h"
#include "AppResources.h"
#include "GITechDemo.h"
#include "ArtistParameter.h"
#include "RenderScheme.h"
using namespace GITechDemoApp;

#define FRAMETIME_GRAPH_HEIGHT (100.f)
#define FRAMETIME_GRAPH_HISTORY (10.f)

#define STEP_FAST (10.f)
#define BUTTON_WIDTH (100.f)
#define ALPHA_PER_SECOND (5.f)
#define ALPHA_MIN (0.25f)

#define MARKER_RESULT_HISTORY (1.f)

static const string HumanReadableByteCount(unsigned long bytes)
{
    int i = 0;
    const char* units[] = { "B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB" };
    float size = (float)bytes;
    while (size > 1024.f)
    {
        size /= 1024.f;
        i++;
    }
    std::ostringstream stringStream;
    stringStream.imbue(std::locale(""));
    stringStream << std::fixed << std::setprecision(2);
    if (i == 0)
    {
        stringStream << bytes << " " << units[i];
    }
    else
    {
        stringStream << roundf(size * 100.f) / 100.f << " " << units[i] << " (" << bytes << " " << units[0] << ")";
    }
    string string = stringStream.str();
    return string;
}

UIPass::UIPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
    , m_pKeyboardDevice(nullptr)
    , m_pMouseDevice(nullptr)
    , m_pFontTexture(nullptr)
    , m_nFontTextureIdx(~0u)
    , m_nTextureViewerIdx(-1)
    , m_nSelectedMip(0)
    , m_nSelectedSlice(0)
    , m_fSelectedSlice(0.f)
    , m_nSelectedFace(0)
    , m_bChannelMask(true, true, true)
    , m_nCurrBufferIdx(0u)
    , m_bShowAllParameters(false)
    , m_bShowProfiler(ENABLE_PROFILE_MARKERS)
    , m_bShowTextureViewer(false)
    , m_fAlpha(0.f)
    , m_nDummyTex1DIdx(~0u)
    , m_nDummyTex2DIdx(~0u)
    , m_nDummyTex3DIdx(~0u)
    , m_nDummyTexCubeIdx(~0u)
    , m_pDummyTex1D(nullptr)
    , m_pDummyTex2D(nullptr)
    , m_pDummyTex3D(nullptr)
    , m_pDummyTexCube(nullptr)
{
    for (unsigned int i = 0; i < UI_BUFFER_COUNT; i++)
    {
        m_nImGuiVfIdx[i] = ~0u;
        m_pImGuiVf[i] = nullptr;
        m_nImGuiIbIdx[i] = ~0u;
        m_pImGuiIb[i] = nullptr;
        m_nImGuiVbIdx[i] = ~0u;
        m_pImGuiVb[i] = nullptr;
    }
}

UIPass::~UIPass()
{}

void UIPass::Update(const float fDeltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    ResourceManager* ResMgr = RenderContext ? RenderContext->GetResourceManager() : nullptr;
    if (!RenderContext || !ResMgr)
        return;

    Framework* const pFW = Framework::GetInstance();
    ImGuiIO& io = ImGui::GetIO();

    m_tGPUProfileMarkerResultHistory.Update(pFW->GetDeltaTime());

    io.IniFilename = nullptr;
    io.RenderDrawListsFn = nullptr;

    io.DisplaySize = ImVec2((float)RenderContext->GetDisplayResolution()[0], (float)RenderContext->GetDisplayResolution()[1]);
    io.FontGlobalScale = Math::Max(io.DisplaySize.y / 1080.f, 1.f); // 1080p is the reference font size
    io.DeltaTime = pFW->GetDeltaTime();

    // Pass current mouse position to ImGui
    io.MousePos.x = m_pMouseDevice->GetFloat(gainput::MouseAxisX) * io.DisplaySize.x;
    io.MousePos.y = m_pMouseDevice->GetFloat(gainput::MouseAxisY) * io.DisplaySize.y;

    // Pass input to ImGui
    if (((GITechDemo*)AppMain)->IsUIInFocus())
    {
        io.MouseDown[0] = m_pMouseDevice->GetBool(gainput::MouseButtonLeft);
        io.MouseDown[1] = m_pMouseDevice->GetBool(gainput::MouseButtonRight);
        io.MouseDown[2] = m_pMouseDevice->GetBool(gainput::MouseButtonMiddle);

        io.MouseWheel += m_pMouseDevice->GetBool(gainput::MouseButtonWheelUp) ? 1.f : 0.f;
        io.MouseWheel -= m_pMouseDevice->GetBool(gainput::MouseButtonWheelDown) ? 1.f : 0.f;

        io.AddInputCharacter(m_pKeyboardDevice->GetNextCharacter());

        io.KeysDown[gainput::KeyTab] = m_pKeyboardDevice->GetBool(gainput::KeyTab);
        io.KeysDown[gainput::KeyLeft] = m_pKeyboardDevice->GetBool(gainput::KeyLeft);
        io.KeysDown[gainput::KeyRight] = m_pKeyboardDevice->GetBool(gainput::KeyRight);
        io.KeysDown[gainput::KeyUp] = m_pKeyboardDevice->GetBool(gainput::KeyUp);
        io.KeysDown[gainput::KeyDown] = m_pKeyboardDevice->GetBool(gainput::KeyDown);
        io.KeysDown[gainput::KeyPageUp] = m_pKeyboardDevice->GetBool(gainput::KeyPageUp);
        io.KeysDown[gainput::KeyPageDown] = m_pKeyboardDevice->GetBool(gainput::KeyPageDown);
        io.KeysDown[gainput::KeyHome] = m_pKeyboardDevice->GetBool(gainput::KeyHome);
        io.KeysDown[gainput::KeyEnd] = m_pKeyboardDevice->GetBool(gainput::KeyEnd);
        io.KeysDown[gainput::KeyDelete] = m_pKeyboardDevice->GetBool(gainput::KeyDelete);
        io.KeysDown[gainput::KeyBackSpace] = m_pKeyboardDevice->GetBool(gainput::KeyBackSpace);
        io.KeysDown[gainput::KeyReturn] = m_pKeyboardDevice->GetBool(gainput::KeyReturn);
        io.KeysDown[gainput::KeyEscape] = m_pKeyboardDevice->GetBool(gainput::KeyEscape);
        io.KeysDown[gainput::KeyA] = m_pKeyboardDevice->GetBool(gainput::KeyA) && (m_pKeyboardDevice->GetBool(gainput::KeyCtrlL) || m_pKeyboardDevice->GetBool(gainput::KeyCtrlR));
        io.KeysDown[gainput::KeyC] = m_pKeyboardDevice->GetBool(gainput::KeyC) && (m_pKeyboardDevice->GetBool(gainput::KeyCtrlL) || m_pKeyboardDevice->GetBool(gainput::KeyCtrlR));
        io.KeysDown[gainput::KeyV] = m_pKeyboardDevice->GetBool(gainput::KeyV) && (m_pKeyboardDevice->GetBool(gainput::KeyCtrlL) || m_pKeyboardDevice->GetBool(gainput::KeyCtrlR));
        io.KeysDown[gainput::KeyX] = m_pKeyboardDevice->GetBool(gainput::KeyX) && (m_pKeyboardDevice->GetBool(gainput::KeyCtrlL) || m_pKeyboardDevice->GetBool(gainput::KeyCtrlR));
        io.KeysDown[gainput::KeyY] = m_pKeyboardDevice->GetBool(gainput::KeyY) && (m_pKeyboardDevice->GetBool(gainput::KeyCtrlL) || m_pKeyboardDevice->GetBool(gainput::KeyCtrlR));
        io.KeysDown[gainput::KeyZ] = m_pKeyboardDevice->GetBool(gainput::KeyZ) && (m_pKeyboardDevice->GetBool(gainput::KeyCtrlL) || m_pKeyboardDevice->GetBool(gainput::KeyCtrlR));

        io.KeyCtrl = m_pKeyboardDevice->GetBool(gainput::KeyCtrlL) || m_pKeyboardDevice->GetBool(gainput::KeyCtrlR);
        io.KeyShift = m_pKeyboardDevice->GetBool(gainput::KeyShiftL) || m_pKeyboardDevice->GetBool(gainput::KeyShiftR);
        io.KeyAlt = m_pKeyboardDevice->GetBool(gainput::KeyAltL) || m_pKeyboardDevice->GetBool(gainput::KeyAltR) || m_pKeyboardDevice->GetBool(gainput::KeyAltGr);
        io.KeySuper = m_pKeyboardDevice->GetBool(gainput::KeySuperL) || m_pKeyboardDevice->GetBool(gainput::KeySuperR);
    }
    else
    {
        // Reset input if UI is not in focus
        io.MouseDown[0] = false;
        io.MouseDown[1] = false;
        io.MouseDown[2] = false;

        io.KeysDown[gainput::KeyTab] = false;
        io.KeysDown[gainput::KeyLeft] = false;
        io.KeysDown[gainput::KeyRight] = false;
        io.KeysDown[gainput::KeyUp] = false;
        io.KeysDown[gainput::KeyDown] = false;
        io.KeysDown[gainput::KeyPageUp] = false;
        io.KeysDown[gainput::KeyPageDown] = false;
        io.KeysDown[gainput::KeyHome] = false;
        io.KeysDown[gainput::KeyEnd] = false;
        io.KeysDown[gainput::KeyDelete] = false;
        io.KeysDown[gainput::KeyBackSpace] = false;
        io.KeysDown[gainput::KeyReturn] = false;
        io.KeysDown[gainput::KeyEscape] = false;
        io.KeysDown[gainput::KeyA] = false;
        io.KeysDown[gainput::KeyC] = false;
        io.KeysDown[gainput::KeyV] = false;
        io.KeysDown[gainput::KeyX] = false;
        io.KeysDown[gainput::KeyY] = false;
        io.KeysDown[gainput::KeyZ] = false;

        io.KeyCtrl = false;
        io.KeyShift = false;
        io.KeyAlt = false;
        io.KeySuper = false;
    }

    m_fAlpha += (((GITechDemo*)AppMain)->IsUIInFocus() ? ALPHA_PER_SECOND : -ALPHA_PER_SECOND) * pFW->GetDeltaTime();
    m_fAlpha = Math::clamp(m_fAlpha, FLT_MIN, 1.f);

    SetupUI();
    GenerateDrawData();

    HLSL::UI_Texture1D = m_nDummyTex1DIdx;
    HLSL::UI_Texture2D = m_nFontTextureIdx;
    HLSL::UI_Texture3D = m_nDummyTex3DIdx;
    HLSL::UI_TextureCube = m_nDummyTexCubeIdx;

    const float L = 0.5f, R = io.DisplaySize.x + 0.5f, T = 0.5f, B = io.DisplaySize.y + 0.5f;
    HLSL::UIParams->UIProjMat.set(
        2.0f / (R - L), 0.0f, 0.0f, (L + R) / (L - R),
        0.0f, 2.0f / (T - B), 0.0f, (T + B) / (B - T),
        0.0f, 0.0f, 0.5f, 0.5f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    HLSL::UIParams->TextureSwitch = 2;
    HLSL::UIParams->MipLevel = m_nSelectedMip;
    HLSL::UIParams->FaceIdx = m_nSelectedFace;
    HLSL::UIParams->DepthSlice = m_fSelectedSlice;
}

void UIPass::Draw()
{
    RenderUI();
}

void UIPass::AddParameterInWindow(ArtistParameter* const param) const
{
    const ImGuiIO& io = ImGui::GetIO();

    switch (param->GetDataType())
    {
    case ArtistParameter::ArtistParameterDataType::APDT_FLOAT:
        ImGui::InputFloat(param->GetName().c_str(), &param->GetParameterAsFloat(), param->GetStepValue(), param->GetStepValue() * STEP_FAST);
        break;

    case ArtistParameter::ArtistParameterDataType::APDT_INT:
        ImGui::InputInt(param->GetName().c_str(), &param->GetParameterAsInt(), (int)param->GetStepValue(), (int)(param->GetStepValue() * STEP_FAST));
        break;

    case ArtistParameter::ArtistParameterDataType::APDT_BOOL:
        ImGui::Checkbox(param->GetName().c_str(), &param->GetParameterAsBool());
        break;

    case ArtistParameter::ArtistParameterDataType::APDT_DROPDOWN:
    {
        int dropdownItemCount = 0;
        while (param->GetParameterAsDropdownFncPtr()(AppMain, dropdownItemCount, nullptr)) // maybe limit this to something sensible?
        {
            dropdownItemCount++;
        };
        ImGui::Combo(param->GetName().c_str(), &param->GetDropdownIndex(), param->GetParameterAsDropdownFncPtr(), AppMain, dropdownItemCount);
        break;
    }

    default:
        assert(false);
        ImGui::TextDisabled(param->GetName().c_str());
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(io.DisplaySize.x * 0.25f);
        ImGui::TextUnformatted(param->GetDescription().c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void UIPass::CleanGPUProfileMarkerResultCache(const char* const passName)
{
    // Clean up old cached profile marker results
    for (int i = (int)m_arrGPUProfileMarkerResultCache.size() - 1; i >= 0; i--)
    {
        if (passName == m_arrGPUProfileMarkerResultCache[i].name)
        {
            m_arrGPUProfileMarkerResultCache.erase(m_arrGPUProfileMarkerResultCache.begin() + i);
        }
    }
}

void UIPass::DrawGPUFrametimeGraph()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    const ImGuiStyle& style = ImGui::GetStyle();

    float minTiming = FLT_MAX;
    float maxTiming = -FLT_MAX;

    // Update GPU frame time history buffer
    const GPUProfileMarkerResult* const rootMarker = RenderContext->GetProfiler()->RetrieveGPUProfileMarker(RenderScheme::GetRootPass().GetPassName());
    if (rootMarker)
    {
        const float rootTiming = rootMarker->GetTiming();
        const float rootStart = rootMarker->GetStart();
        m_arrGPUFrametimeHistory.push_back(GPUFrametimeHistoryEntry(rootTiming, rootStart));

        // Clean up entries older than FRAMETIME_GRAPH_HISTORY seconds
        for (int i = (int)m_arrGPUFrametimeHistory.size() - 1; i >= 0; i--)
        {
            if (m_arrGPUFrametimeHistory[i].start < rootStart - FRAMETIME_GRAPH_HISTORY * 1000000.f || m_arrGPUFrametimeHistory[i].start > rootStart)
            {
                m_arrGPUFrametimeHistory.erase(m_arrGPUFrametimeHistory.begin() + i);
            }
            else
            {
                if (m_arrGPUFrametimeHistory[i].timing < minTiming)
                {
                    minTiming = m_arrGPUFrametimeHistory[i].timing;
                }

                if (m_arrGPUFrametimeHistory[i].timing > maxTiming)
                {
                    maxTiming = m_arrGPUFrametimeHistory[i].timing;
                }
            }
        }
    }

    if (m_arrGPUFrametimeHistory.size() > 0)
    {
        const float widthFactor = Math::clamp((m_arrGPUFrametimeHistory.back().start - m_arrGPUFrametimeHistory.front().start) / (FRAMETIME_GRAPH_HISTORY * 1000000.f), 0.f, 1.f);
        const float width = ImGui::GetWindowWidth() - style.WindowPadding.x * 2.f;
        const float height = FRAMETIME_GRAPH_HEIGHT - ImGui::GetFontSize() * 2.f - style.WindowPadding.y * 2.f - style.FramePadding.y * 2.f;
        
        const ImVec2 graphSize(widthFactor * width, height);

        const float lineR = maxTiming > 1000.f / 60.f;
        const float lineG = maxTiming < 1000.f / 30.f;

        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(lineR, lineG, 0.0f, 1.f));

        ImGui::Text("%7.3f ms (%6.3f fps)", maxTiming, 1000.f / maxTiming);
        ImGui::SetCursorPosX((1.f - widthFactor) * width + style.WindowPadding.x);
        ImGui::PlotLines("", &(m_arrGPUFrametimeHistory[0].timing), (int)m_arrGPUFrametimeHistory.size(), 0, nullptr, minTiming, maxTiming, graphSize, sizeof(GPUFrametimeHistoryEntry));
        ImGui::Text("%7.3f ms (%6.3f fps)", minTiming, 1000.f / minTiming);

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
}

void UIPass::DrawGPUProfileBars(const RenderPass* pass, const unsigned int level)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    const ImGuiStyle& style = ImGui::GetStyle();

    if (pass == nullptr)
        pass = &RenderScheme::GetRootPass();

    if (pass)
    {
        const GPUProfileMarkerResult* const marker = RenderContext->GetProfiler()->RetrieveGPUProfileMarker(pass->GetPassName());
        float timing = marker ? marker->GetTiming() : 0.f;
        float start = marker ? marker->GetStart() : 0.f;
        float end = marker ? marker->GetEnd() : 0.f;

        const GPUProfileMarkerResult* const rootMarker = RenderContext->GetProfiler()->RetrieveGPUProfileMarker(RenderScheme::GetRootPass().GetPassName());
        float rootTiming = rootMarker ? rootMarker->GetTiming() : 0.f;
        float rootStart = rootMarker ? rootMarker->GetStart() : 0.f;
        float rootEnd = rootMarker ? rootMarker->GetEnd() : 0.f;

        if (level > 0 && timing > 0.f)
        {
            // Add this profile marker result to the cache
            if (start >= rootStart && end <= rootEnd)
            {
                CleanGPUProfileMarkerResultCache(pass->GetPassName());

                // Profile marker result is from current frame
                m_arrGPUProfileMarkerResultCache.push_back(GPUProfileMarkerResultCacheEntry(pass->GetPassName(), timing, start, end, rootTiming, rootStart, rootEnd));
            }
            else
            {
                // Profile marker result is from a future frame
                m_arrGPUProfileMarkerResultCache.push_back(GPUProfileMarkerResultCacheEntry(pass->GetPassName(), timing, start, end, 0.f, 0.f, 0.f));
            }

            // Add root timings to incomplete cached profile marker results (if applicable)
            for (unsigned int i = 0; i < m_arrGPUProfileMarkerResultCache.size(); i++)
            {
                if (m_arrGPUProfileMarkerResultCache[i].rootTiming == 0.f)
                {
                    if (m_arrGPUProfileMarkerResultCache[i].start >= rootStart && m_arrGPUProfileMarkerResultCache[i].end <= rootEnd)
                    {
                        GPUProfileMarkerResultCacheEntry entry = m_arrGPUProfileMarkerResultCache[i];
                        entry.rootTiming = rootTiming;
                        entry.rootStart = rootStart;
                        entry.rootEnd = rootEnd;

                        CleanGPUProfileMarkerResultCache(entry.name.c_str());

                        m_arrGPUProfileMarkerResultCache.push_back(entry);
                    }
                }
            }

            // This profile marker result is from a future frame so try and find an older profile marker result from the cache
            if (start > rootEnd)
            {
                for (int i = (int)m_arrGPUProfileMarkerResultCache.size() - 1; i >= 0; i--)
                {
                    if (pass->GetPassName() == m_arrGPUProfileMarkerResultCache[i].name && m_arrGPUProfileMarkerResultCache[i].rootTiming != 0.f)
                    {
                        timing = m_arrGPUProfileMarkerResultCache[i].timing;
                        start = m_arrGPUProfileMarkerResultCache[i].start;
                        end = m_arrGPUProfileMarkerResultCache[i].end;
                        rootTiming = m_arrGPUProfileMarkerResultCache[i].rootTiming;
                        rootStart = m_arrGPUProfileMarkerResultCache[i].rootStart;
                        rootEnd = m_arrGPUProfileMarkerResultCache[i].rootEnd;
                        break;
                    }
                }
            }

            float barLeft, barRight;
            Math::lerp(
                barLeft,
                (start - rootStart) / (rootEnd - rootStart),
                style.WindowPadding.x, ImGui::GetWindowWidth() - style.WindowPadding.x
            );
            Math::lerp(barRight,
                (end - rootStart) / (rootEnd - rootStart),
                style.WindowPadding.x, ImGui::GetWindowWidth() - style.WindowPadding.x
            );

            barLeft += 0.5f;
            barRight -= 0.5f;

            const float barHeight = ImGui::GetFontSize() + style.FramePadding.y * 2.f;
            ImVec2 barSize = ImVec2(barRight - barLeft, barHeight);

            ImGui::SetCursorPosX(barLeft);
            ImGui::SetCursorPosY((level - 1) * (barHeight + style.FramePadding.y) + style.WindowPadding.y);

            const char* const passName = pass->GetPassName();
            const unsigned int passNameHash = S3DHASH(passName);
            const float R = ((passNameHash & 0x000000FF) >> 0) / 255.f;
            const float G = ((passNameHash & 0x0000FF00) >> 8) / 255.f;
            const float B = ((passNameHash & 0x00FF0000) >> 16) / 255.f;
            const float luma = R * 0.299f + G * 0.587f + B * 0.114f;

            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(R, G, B, 0.75f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(luma < 0.5f, luma < 0.5f, luma < 0.5f, 1.f));
            ImGui::PlotLines("", &timing, 0, 0, pass->GetPassName(), 0.f, 0.f, barSize);
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }

        m_tGPUProfileMarkerResultHistory.PushMarker(GPUProfileMarkerResultCacheEntry(pass->GetPassName(), timing, start, end, rootTiming, rootStart, rootEnd));

        for (unsigned int i = 0; i < (unsigned int)pass->GetChildren().size(); i++)
        {
            DrawGPUProfileBars(pass->GetChildren()[i], level + 1);
        }
    }
}

void UIPass::DrawGPUProfileDetails(const RenderPass* pass, const unsigned int level) const
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    const ImGuiStyle& style = ImGui::GetStyle();

    if (pass == nullptr)
        pass = &RenderScheme::GetRootPass();

    if (pass)
    {
        const GPUProfileMarkerResult* const marker = RenderContext->GetProfiler()->RetrieveGPUProfileMarker(pass->GetPassName());
        const float timing = marker ? marker->GetTiming() : 0.f;

        ImGui::Text("%s: %6.3f ms (avg %6.3f ms)", pass->GetPassName(), timing, m_tGPUProfileMarkerResultHistory.GetAverage(pass->GetPassName()));

        for (unsigned int i = 0; i < (unsigned int)pass->GetChildren().size(); i++)
        {
            ImGui::Indent();
            DrawGPUProfileDetails(pass->GetChildren()[i], level + 1);
            ImGui::Unindent();
        }
    }
}

void UIPass::SetupUI()
{
    Framework* const pFW = Framework::GetInstance();
    ImGuiIO& io = ImGui::GetIO();
    const ImGuiStyle& style = ImGui::GetStyle();

    ImGui::NewFrame();

    // ImGui test window
    // bool show_test_window = false;
    // ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
    // ImGui::ShowTestWindow(&show_test_window);

    // Main menu + title bar
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_fAlpha);
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Parameters"))
        {
            ImGui::MenuItem("All parameters", nullptr, &m_bShowAllParameters);

            ImGui::TreePush("Individual parameters");
            for (unsigned int itemIdx = 0; itemIdx < m_arrParamCategoryWindowStates.size(); itemIdx++)
            {
                ImGui::MenuItem(m_arrParamCategoryWindowStates[itemIdx].categoryName.c_str(), nullptr, &m_arrParamCategoryWindowStates[itemIdx].windowOpen);
            }
            ImGui::TreePop();

            ImGui::EndMenu();
        }

        bool pauseUpdate = pFW->IsUpdatePaused();
        ImGui::MenuItem(pauseUpdate ? "Unpause update" : "Pause update", nullptr, &pauseUpdate);
        pFW->PauseUpdate(pauseUpdate);

        ImGui::MenuItem(m_bShowProfiler ? "Hide profiler" : "Show profiler", "Debug/Profile only", &m_bShowProfiler, ENABLE_PROFILE_MARKERS);

    #if !ENABLE_PROFILE_MARKERS
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(450.0f);
            ImGui::TextUnformatted("Profiler is only available on Debug and Profile builds.");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    #endif

        ImGui::MenuItem(m_bShowTextureViewer ? "Close texture viewer" : "Open texture viewer", nullptr, &m_bShowTextureViewer);

        if (ImGui::MenuItem("Quit", "Alt+F4"))
        {
            ImGui::OpenPopup("Quit?");
        }

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.f);
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize(pFW->GetWindowTitle().c_str()).x - style.WindowPadding.x);
        ImGui::TextDisabled(pFW->GetWindowTitle().c_str());
        ImGui::PopStyleVar();

        // Quit confirmation dialog
        ImGui::PushStyleColor(ImGuiCol_ModalWindowDarkening, ImVec4(0.1f, 0.1f, 0.1f, 0.75f));
        if (ImGui::BeginPopupModal("Quit?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {


            ImGui::PushTextWrapPos((BUTTON_WIDTH + style.WindowPadding.x) * 2.f + style.FramePadding.x);
            ImGui::Text("Are you sure you want to exit the application?");
            ImGui::PopTextWrapPos();
            ImGui::Separator();

            if (ImGui::Button("Yes", ImVec2(BUTTON_WIDTH, 0))) { pFW->Quit(); }
            ImGui::SameLine();
            if (ImGui::Button("No", ImVec2(BUTTON_WIDTH, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
        ImGui::PopStyleColor();

        ImGui::EndMainMenuBar();
    }
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, Math::Max(m_fAlpha, ALPHA_MIN));
    if (!m_bShowAllParameters)
    {
        // Parameter windows (one per category)
        for (unsigned int catIdx = 0; catIdx < m_arrParamCategoryWindowStates.size(); catIdx++)
        {
            if (m_arrParamCategoryWindowStates[catIdx].windowOpen)
            {
                if (ImGui::Begin(m_arrParamCategoryWindowStates[catIdx].categoryName.c_str(), &m_arrParamCategoryWindowStates[catIdx].windowOpen, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    for (unsigned int paramIdx = 0; paramIdx < ArtistParameter::GetParameterCount(); paramIdx++)
                    {
                        ArtistParameter* const param = ArtistParameter::GetParameterByIdx(paramIdx);
                        if (param->GetCategoryName() == m_arrParamCategoryWindowStates[catIdx].categoryName)
                        {
                            AddParameterInWindow(param);
                        }
                    }
                }
                ImGui::End();
            }
        }
    }
    else
    {
        if (ImGui::Begin("All parameters", &m_bShowAllParameters))
        {
            if (!ImGui::IsWindowCollapsed())
            {
                ImGui::SetWindowSize(ImVec2(0, io.DisplaySize.y * 0.9f));

                for (unsigned int catIdx = 0; catIdx < m_arrParamCategoryWindowStates.size(); catIdx++)
                {
                    ImGui::TextDisabled(m_arrParamCategoryWindowStates[catIdx].categoryName.c_str());
                    for (unsigned int paramIdx = 0; paramIdx < ArtistParameter::GetParameterCount(); paramIdx++)
                    {
                        ArtistParameter* const param = ArtistParameter::GetParameterByIdx(paramIdx);
                        if (param->GetCategoryName() == m_arrParamCategoryWindowStates[catIdx].categoryName)
                        {
                            AddParameterInWindow(param);
                        }
                    }
                    ImGui::Separator();
                }
            }
            else
            {
                ImGui::SetWindowSize(ImVec2(ImGui::GetWindowWidth(), io.DisplaySize.y * 0.9f));
            }
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    if (m_bShowProfiler)
    {
        const float mainMenuBarHeight = ImGui::GetFontSize() + style.FramePadding.y;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.75f));
        if (ImGui::Begin("GPU frametime graph", &m_bShowProfiler, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs))
        {
            ImGui::SetWindowPos(ImVec2(style.WindowPadding.x, mainMenuBarHeight + style.WindowPadding.y));
            ImGui::SetWindowSize(ImVec2(io.DisplaySize.x - style.WindowPadding.x * 2.f, FRAMETIME_GRAPH_HEIGHT));
            DrawGPUFrametimeGraph();
            ImGui::End();
        }
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        if (ImGui::Begin("GPU Profiler Bars", &m_bShowProfiler, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs))
        {
            ImGui::SetWindowPos(ImVec2(style.WindowPadding.x, mainMenuBarHeight + FRAMETIME_GRAPH_HEIGHT + style.WindowPadding.y * 2.f));
            ImGui::SetWindowSize(ImVec2(io.DisplaySize.x - style.WindowPadding.x * 2.f, 0.f));
            DrawGPUProfileBars();
            ImGui::End();
        }
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.75f));
        if (ImGui::Begin("GPU Profiler Details", &m_bShowProfiler, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs))
        {
            ImGui::SetWindowPos(ImVec2(io.DisplaySize.x - style.WindowPadding.x - ImGui::GetWindowWidth(), io.DisplaySize.y - style.WindowPadding.y - ImGui::GetWindowHeight()));
            DrawGPUProfileDetails();
            ImGui::End();
        }
        ImGui::PopStyleColor();
    }

    if (m_bShowTextureViewer)
    {
        const float mainMenuBarHeight = ImGui::GetFontSize() + style.FramePadding.y;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        if (ImGui::Begin("Texture viewer", &m_bShowProfiler, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
        {
            ImGui::SetWindowPos(ImVec2(style.WindowPadding.x, mainMenuBarHeight + style.WindowPadding.y));
            ImGui::SetWindowSize(ImVec2(io.DisplaySize.x - style.WindowPadding.x * 2.f, io.DisplaySize.y - style.WindowPadding.y * 2.f - mainMenuBarHeight));

            const Synesthesia3D::ResourceManager* const resMan = Renderer::GetInstance()->GetResourceManager();

            string texDesc;
            vector<s3dSampler> texList;
            for (unsigned int i = 0; i < RenderResource::GetResourceList().size(); i++)
            {
                if (RenderResource::GetResourceList()[i]->GetResourceType() == RenderResource::RES_RENDERTARGET)
                {
                    const unsigned int targetCount = ((RenderTarget*)(RenderResource::GetResourceList()[i]))->GetRenderTarget()->GetTargetCount();
                    const bool hasDepth = ((RenderTarget*)(RenderResource::GetResourceList()[i]))->GetRenderTarget()->HasDepthBuffer();

                    for (unsigned int j = 0; j < targetCount; j++)
                    {
                        texList.push_back(((RenderTarget*)RenderResource::GetResourceList()[i])->GetRenderTarget()->GetColorBuffer(j));
                        texDesc += RenderResource::GetResourceList()[i]->GetDesc();
                        if (hasDepth)
                        {
                            texDesc += " - color";
                            if (targetCount > 1)
                            {
                                char num[3];
                                sprintf_s(num, " %d", j);
                                texDesc += num;
                            }
                        }
                        texDesc += '\0';
                    }

                    if (hasDepth)
                    {
                        texList.push_back(((RenderTarget*)RenderResource::GetResourceList()[i])->GetRenderTarget()->GetDepthBuffer());
                        texDesc += RenderResource::GetResourceList()[i]->GetDesc();
                        if (targetCount > 0)
                        {
                            texDesc += " - depth";
                        }
                        texDesc += '\0';
                    }
                }

                if (RenderResource::GetResourceList()[i]->GetResourceType() == RenderResource::RES_TEXTURE)
                {
                    texList.push_back(((Texture*)RenderResource::GetResourceList()[i])->GetTextureIndex());
                    texDesc += RenderResource::GetResourceList()[i]->GetDesc();
                    texDesc += '\0';
                }
            }

            const float viewerWidth = ImGui::GetWindowWidth() * 2.f / 3.f;
            const float viewerHeight = ImGui::GetWindowHeight() - ImGui::GetCursorPosY() - style.WindowPadding.y;
            const float viewerPosY = ImGui::GetCursorPosY();

            Synesthesia3D::Texture* const tex = m_nTextureViewerIdx >= 0 ? Renderer::GetInstance()->GetResourceManager()->GetTexture(texList[m_nTextureViewerIdx]) : nullptr;

            if (m_nTextureViewerIdx >= 0)
            {
                float imgWidth, imgHeight;

                if (tex->GetWidth() * viewerHeight / tex->GetHeight() > viewerWidth)
                {
                    imgWidth = viewerWidth;
                    imgHeight = tex->GetHeight() * viewerWidth / tex->GetWidth();
                    ImGui::SetCursorPosY(gmtl::Math::clamp((viewerHeight - imgHeight) * 0.5f, 0.f, viewerHeight));
                }
                else
                {
                    imgWidth = tex->GetWidth() * viewerHeight / tex->GetHeight();
                    imgHeight = viewerHeight;
                    ImGui::SetCursorPosX(gmtl::Math::clamp((viewerWidth - imgWidth) * 0.5f, 0.f, viewerWidth));
                }

                ImGui::Image(tex, ImVec2(imgWidth, imgHeight), ImVec2(0, 0), ImVec2(1, 1),
                    ImVec4(m_bChannelMask[0] ? 1.f : 0.f, m_bChannelMask[1] ? 1.f : 0.f, m_bChannelMask[2] ? 1.f : 0.f, 1.f), ImVec4(1, 0, 0, 1));
                const ImVec2 imgPos = ImGui::GetItemRectMin();

                if (ImGui::IsItemHovered() && ((GITechDemo*)AppMain)->IsUIInFocus())
                {
                    ImGui::BeginTooltip();
                    float focus_sz = 32.0f;
                    float focus_x = ImGui::GetMousePos().x - imgPos.x - focus_sz * 0.5f; if (focus_x < 0.0f) focus_x = 0.0f; else if (focus_x > imgWidth - focus_sz) focus_x = imgWidth - focus_sz;
                    float focus_y = ImGui::GetMousePos().y - imgPos.y - focus_sz * 0.5f; if (focus_y < 0.0f) focus_y = 0.0f; else if (focus_y > imgHeight - focus_sz) focus_y = imgHeight - focus_sz;
                    //ImGui::Text("Min: (%d, %d)", int(round((focus_x * tex->GetWidth() / imgWidth))), int(round(focus_y * tex->GetHeight() / imgHeight)));
                    //ImGui::Text("Max: (%d, %d)", int(round((focus_x + focus_sz) * tex->GetWidth() / imgWidth)), int(round((focus_y + focus_sz) * tex->GetHeight() / imgHeight)));
                    ImVec2 uv0 = ImVec2((focus_x) / imgWidth, (focus_y) / imgHeight);
                    ImVec2 uv1 = ImVec2((focus_x + focus_sz) / imgWidth, (focus_y + focus_sz) / imgHeight);
                    ImGui::Image(tex, ImVec2(256, 256), uv0, uv1, ImVec4(m_bChannelMask[0] ? 1.f : 0.f, m_bChannelMask[1] ? 1.f : 0.f, m_bChannelMask[2] ? 1.f : 0.f, 1.f), ImColor(255, 255, 255, 128));
                    ImGui::EndTooltip();
                }
            }

            ImGui::SameLine();
            ImGui::SetCursorPos(ImVec2(viewerWidth + style.WindowPadding.x + style.FramePadding.x * 2.f, viewerPosY));

            if (ImGui::BeginChild("", ImVec2(viewerWidth / 2.f - style.WindowPadding.x - style.FramePadding.x * 4.f, viewerHeight)))
            {
                ImGui::Separator();
                ImGui::NewLine();

                ImGui::Text("Select texture");
                ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
                ImGui::Combo("Texture", &m_nTextureViewerIdx, texDesc.c_str(), INT_MAX);
                ImGui::PopItemWidth();

                if (tex)
                {
                    ImGui::NewLine();

                    ImGui::Text("Texture type: %s", Renderer::GetEnumString(tex->GetTextureType()));
                    ImGui::Text("Pixel format: %s", Renderer::GetEnumString(tex->GetPixelFormat()));
                    ImGui::Text("sRGB: %s", tex->GetSRGBEnabled() ? "true" : "false");
                    ImGui::Text("Bytes per pixel: %u B", Synesthesia3D::Texture::GetBytesPerPixel(tex->GetPixelFormat()));
                    ImGui::Text("Render target: %s", tex->IsRenderTarget() ? "true" : "false");
                    ImGui::Text("Depth/stencil: %s", tex->IsDepthStencil() ? "true" : "false");
                    ImGui::Text("Width: %u", tex->GetWidth());
                    ImGui::Text("Height: %u", tex->GetHeight());
                    ImGui::Text("Depth: %u", tex->GetDepth());
                    ImGui::Text("MIP count: %u", tex->GetMipCount());
                    ImGui::Text("Total size: %s", HumanReadableByteCount(tex->GetSize()).c_str());

                    if (tex->GetTextureType() == TT_CUBE)
                    {
                        ImGui::NewLine();
                        ImGui::Separator();
                        ImGui::NewLine();

                        ImGui::Text("Select cube face");
                        ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
                        ImGui::SliderInt("Face", &m_nSelectedFace, 0, FACE_MAX - 1, Renderer::GetEnumString((CubeFace)m_nSelectedFace));
                        ImGui::PopItemWidth();
                        ImGui::NewLine();

                        ImGui::Text("Face size (incl. MIPs): %s", HumanReadableByteCount(tex->GetCubeFaceOffset()).c_str());
                    }

                    if (tex->GetTextureType() == TT_3D)
                    {
                        ImGui::NewLine();
                        ImGui::Separator();
                        ImGui::NewLine();

                        ImGui::Text("Select depth slice");
                        ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
                        ImGui::SliderInt("Depth", &m_nSelectedSlice, 0, tex->GetDepth() - 1);
                        m_fSelectedSlice = (float)m_nSelectedSlice / (float)(tex->GetDepth() - 1);
                        ImGui::PopItemWidth();
                    }

                    if (tex->GetMipCount() > 1)
                    {
                        ImGui::NewLine();
                        ImGui::Separator();
                        ImGui::NewLine();

                        ImGui::Text("Select MIP level");
                        ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
                        ImGui::SliderInt("MIP", &m_nSelectedMip, 0, tex->GetMipCount() - 1);
                        ImGui::PopItemWidth();
                        ImGui::NewLine();

                        ImGui::Text("MIP width: %u", tex->GetWidth(m_nSelectedMip));
                        ImGui::Text("MIP height: %u", tex->GetHeight(m_nSelectedMip));
                        ImGui::Text("MIP depth: %u", tex->GetDepth(m_nSelectedMip));
                        ImGui::Text("MIP size: %s", HumanReadableByteCount(tex->GetMipSizeBytes(m_nSelectedMip)).c_str());
                    }

                    ImGui::NewLine();
                    ImGui::Separator();
                    ImGui::NewLine();

                    ImGui::Text("Sampler states:");
                    ImGui::NewLine();

                    ImGui::Text("Anisotropy: %u", tex->GetAnisotropy());
                    ImGui::Text("MIP bias: %u", tex->GetMipLodBias());
                    ImGui::Text("Filter: %s", Renderer::GetEnumString(tex->GetFilter()));
                    ImGui::Text("Addressing mode U: %s", Renderer::GetEnumString(tex->GetAddressingModeU()));
                    ImGui::Text("Addressing mode V: %s", Renderer::GetEnumString(tex->GetAddressingModeV()));
                    ImGui::Text("Addressing mode W: %s", Renderer::GetEnumString(tex->GetAddressingModeW()));
                    ImGui::Text("Border color: RGBA(%f, %f, %f, %f)",
                        tex->GetBorderColor()[0], tex->GetBorderColor()[1],
                        tex->GetBorderColor()[2], tex->GetBorderColor()[3]);

                    ImGui::NewLine();
                    ImGui::Text("Channel mask:");

                    ImGui::Checkbox("Red", &m_bChannelMask[0]);
                    ImGui::SameLine();
                    ImGui::Checkbox("Green", &m_bChannelMask[1]);
                    ImGui::SameLine();
                    ImGui::Checkbox("Blue", &m_bChannelMask[2]);
                }

                ImGui::NewLine();
                ImGui::Separator();
            }
            ImGui::EndChild();

            ImGui::End();
        }

        ImGui::PopStyleColor();
    }

    ImGui::Render();
}

void UIPass::GenerateDrawData()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResMgr = RenderContext->GetResourceManager();
    if (!ResMgr)
        return;

    ImDrawData* drawData = ImGui::GetDrawData();
    if (!drawData)
        return;

    m_nCurrBufferIdx = (m_nCurrBufferIdx + 1) % UI_BUFFER_COUNT;

    const bool isUIInFocus = ((GITechDemo*)AppMain)->IsUIInFocus();

    const bool needsIbReallocation = !m_pImGuiIb[m_nCurrBufferIdx] || m_pImGuiIb[m_nCurrBufferIdx]->GetElementCount() < (unsigned int)drawData->TotalIdxCount;
    if (needsIbReallocation)
    {
        if (m_nImGuiIbIdx[m_nCurrBufferIdx] != ~0u)
        {
            ResMgr->ReleaseIndexBuffer(m_nImGuiIbIdx[m_nCurrBufferIdx]);
        }

        m_nImGuiIbIdx[m_nCurrBufferIdx] = ResMgr->CreateIndexBuffer(Math::Max(drawData->TotalIdxCount, 1), sizeof(ImDrawIdx) == 2 ? IBF_INDEX16 : IBF_INDEX32);
        m_pImGuiIb[m_nCurrBufferIdx] = ResMgr->GetIndexBuffer(m_nImGuiIbIdx[m_nCurrBufferIdx]);
        assert(sizeof(ImDrawIdx) == m_pImGuiIb[m_nCurrBufferIdx]->GetElementSize());
        assert(m_pImGuiIb[m_nCurrBufferIdx]->GetElementCount() <= Math::pow(2.f, sizeof(ImDrawIdx) * 8.f));

        //cout << "UI index buffer reallocation: " << m_pImGuiIb[m_nCurrBufferIdx]->GetElementCount() << " indices." << endl;
    }

    const bool needsVbReallocation = !m_pImGuiVb[m_nCurrBufferIdx] || m_pImGuiVb[m_nCurrBufferIdx]->GetElementCount() < (unsigned int)drawData->TotalVtxCount;
    if (needsIbReallocation || needsVbReallocation)
    {
        const unsigned int prevMaxVtxCount = m_pImGuiVb[m_nCurrBufferIdx] ? m_pImGuiVb[m_nCurrBufferIdx]->GetElementCount() : 1;
        if (m_nImGuiVbIdx[m_nCurrBufferIdx] != ~0u)
        {
            ResMgr->ReleaseVertexBuffer(m_nImGuiVbIdx[m_nCurrBufferIdx]);
        }

        m_nImGuiVbIdx[m_nCurrBufferIdx] = ResMgr->CreateVertexBuffer(m_pImGuiVf[m_nCurrBufferIdx], Math::Max(prevMaxVtxCount, (unsigned int)drawData->TotalVtxCount), m_pImGuiIb[m_nCurrBufferIdx]);
        m_pImGuiVb[m_nCurrBufferIdx] = ResMgr->GetVertexBuffer(m_nImGuiVbIdx[m_nCurrBufferIdx]);

        //cout << "UI vertex buffer reallocation: " << m_pImGuiVb[m_nCurrBufferIdx]->GetElementCount() << " vertices." << endl;
    }

    m_pImGuiIb[m_nCurrBufferIdx]->Lock(BL_WRITE_ONLY);
    m_pImGuiVb[m_nCurrBufferIdx]->Lock(BL_WRITE_ONLY);

    for (int n = 0, vtxOffset = 0, idxOffset = 0; n < drawData->CmdListsCount; n++)
    {
        const ImDrawList* const cmdList = drawData->CmdLists[n];
        const ImDrawVert* vtxSrc = cmdList->VtxBuffer.Data;

        m_pImGuiIb[m_nCurrBufferIdx]->SetIndices(cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size, idxOffset);
        idxOffset += cmdList->IdxBuffer.Size;

        for (int i = 0; i < cmdList->VtxBuffer.Size; i++)
        {
            const s3dByte R = ((vtxSrc->col & 0x000000FF) >> 0);
            const s3dByte G = ((vtxSrc->col & 0x0000FF00) >> 8);
            const s3dByte B = ((vtxSrc->col & 0x00FF0000) >> 16);
            const s3dByte A = ((vtxSrc->col & 0xFF000000) >> 24);// / (isUIInFocus ? 1 : 2);

            m_pImGuiVb[m_nCurrBufferIdx]->Position<Vec2f>(vtxOffset + i) = Vec2f(vtxSrc->pos.x, vtxSrc->pos.y);
            m_pImGuiVb[m_nCurrBufferIdx]->Color<s3dDword>(vtxOffset + i, 0) = (A << 24) | (R << 16) | (G << 8) | (B << 0); // RGBA --> ARGB for DirectX9
            m_pImGuiVb[m_nCurrBufferIdx]->TexCoord<Vec2f>(vtxOffset + i, 0) = Vec2f(vtxSrc->uv.x, vtxSrc->uv.y);

            vtxSrc++;
        }
        vtxOffset += cmdList->VtxBuffer.Size;
    }

    m_pImGuiIb[m_nCurrBufferIdx]->Update();
    m_pImGuiVb[m_nCurrBufferIdx]->Update();

    m_pImGuiIb[m_nCurrBufferIdx]->Unlock();
    m_pImGuiVb[m_nCurrBufferIdx]->Unlock();

    // Update draw data cache
    m_tDrawData[m_nCurrBufferIdx].CmdList.resize(drawData->CmdListsCount);
    for (unsigned int n = 0; n < m_tDrawData[m_nCurrBufferIdx].CmdList.size(); n++)
    {
        UIDrawData::UIDrawCommandList& cmdList = m_tDrawData[m_nCurrBufferIdx].CmdList[n];
        cmdList.DrawCmd.resize(drawData->CmdLists[n]->CmdBuffer.Size);
        for (unsigned int i = 0; i < cmdList.DrawCmd.size(); i++)
        {
            UIDrawData::UIDrawCommandList::UIDrawCommand& cmd = cmdList.DrawCmd[i];
            const ImVec4 clipRect = drawData->CmdLists[n]->CmdBuffer[i].ClipRect;
            cmd.ClipRect = Vec4f(clipRect.x, clipRect.y, clipRect.z, clipRect.w);
            cmd.ElemCount = drawData->CmdLists[n]->CmdBuffer[i].ElemCount;
            cmd.Texture = (Synesthesia3D::Texture*)(drawData->CmdLists[n]->CmdBuffer[i].TextureId);
        }
        cmdList.VtxBufferSize = drawData->CmdLists[n]->VtxBuffer.Size;
    }
}

void UIPass::RenderUI()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    RenderState* RSMgr = RenderContext->GetRenderStateManager();
    if (!RSMgr)
        return;

    if (!m_pImGuiVb[m_nCurrBufferIdx])
        return;

    // Setup render states
    const bool sRGBEnabled = RSMgr->GetSRGBWriteEnabled();
    const bool zWriteEnable = RSMgr->GetZWriteEnabled();
    const Cmp zFunc = RSMgr->GetZFunc();
    const ZBuffer zEnabled = RSMgr->GetZEnabled();
    const bool blendEnabled = RSMgr->GetColorBlendEnabled();
    const Blend dstBlend = RSMgr->GetColorDstBlend();
    const Blend srcBlend = RSMgr->GetColorSrcBlend();
    const bool scissorEnabled = RSMgr->GetScissorEnabled();
    const Cull cullMode = RSMgr->GetCullMode();

    RSMgr->SetSRGBWriteEnabled(false);
    RSMgr->SetZWriteEnabled(false);
    RSMgr->SetZFunc(CMP_ALWAYS);
    RSMgr->SetZEnabled(ZB_DISABLED);
    RSMgr->SetColorBlendEnabled(true);
    RSMgr->SetColorDstBlend(BLEND_INVSRCALPHA);
    RSMgr->SetColorSrcBlend(BLEND_SRCALPHA);
    RSMgr->SetScissorEnabled(true);
    RSMgr->SetCullMode(CULL_NONE);

    // Render geometry
    UIShader.Enable();
    for (unsigned int n = 0, vtxOffset = 0, idxOffset = 0; n < m_tDrawData[m_nCurrBufferIdx].CmdList.size(); n++)
    {
        const UIDrawData::UIDrawCommandList& cmdList = m_tDrawData[m_nCurrBufferIdx].CmdList[n];
        for (unsigned int i = 0; i < cmdList.DrawCmd.size(); i++)
        {
            const UIDrawData::UIDrawCommandList::UIDrawCommand& cmd = cmdList.DrawCmd[i];
            RSMgr->SetScissor(Vec2i(int(cmd.ClipRect[2] - cmd.ClipRect[0]), int(cmd.ClipRect[3] - cmd.ClipRect[1])), Vec2i(int(cmd.ClipRect[0]), int(cmd.ClipRect[1])));
            RSMgr->SetSRGBWriteEnabled(((Synesthesia3D::Texture*)cmd.Texture)->GetSRGBEnabled());

            const bool invalidateShaderConstants = (((Synesthesia3D::Texture*)cmd.Texture) != m_pFontTexture || HLSL::UI_Texture2D != m_nFontTextureIdx || HLSL::UIParams->TextureSwitch[0] != 2);

            if(((Synesthesia3D::Texture*)cmd.Texture) != m_pFontTexture)
            {
                RSMgr->SetColorBlendEnabled(false);
            }
            else
            {
                RSMgr->SetColorBlendEnabled(true);
            }

            switch (((Synesthesia3D::Texture*)cmd.Texture)->GetTextureType())
            {
            case TT_1D:
                HLSL::UIParams->TextureSwitch = 1;
                HLSL::UI_Texture1D = (Synesthesia3D::Texture*)cmd.Texture;
                break;
            case TT_2D:
                HLSL::UIParams->TextureSwitch = 2;
                HLSL::UI_Texture2D = (Synesthesia3D::Texture*)cmd.Texture;
                break;
            case TT_3D:
                HLSL::UIParams->TextureSwitch = 3;
                HLSL::UI_Texture3D = (Synesthesia3D::Texture*)cmd.Texture;
                break;
            case TT_CUBE:
                HLSL::UIParams->TextureSwitch = 4;
                HLSL::UI_TextureCube = (Synesthesia3D::Texture*)cmd.Texture;
                break;
            }
            
            if (invalidateShaderConstants)
                UIShader.CommitShaderInputs();

            RenderContext->DrawVertexBuffer(m_pImGuiVb[m_nCurrBufferIdx], vtxOffset, cmd.ElemCount / 3, cmdList.VtxBufferSize, idxOffset);

            idxOffset += cmd.ElemCount;
        }
        vtxOffset += cmdList.VtxBufferSize;
    }
    UIShader.Disable();

    // Revert render states
    RSMgr->SetSRGBWriteEnabled(sRGBEnabled);
    RSMgr->SetZWriteEnabled(zWriteEnable);
    RSMgr->SetZFunc(zFunc);
    RSMgr->SetZEnabled(zEnabled);
    RSMgr->SetColorBlendEnabled(blendEnabled);
    RSMgr->SetColorDstBlend(dstBlend);
    RSMgr->SetColorSrcBlend(srcBlend);
    RSMgr->SetScissorEnabled(scissorEnabled);
    RSMgr->SetCullMode(cullMode);

    // Seems redundant, but fixes RTSS overlay
    RSMgr->SetScissor(RenderContext->GetDisplayResolution());
}

void UIPass::SetupInput(gainput::InputManager* pInputManager)
{
    if (!pInputManager)
        return;

    m_pMouseDevice = pInputManager->CreateAndGetDevice<gainput::InputDeviceMouse>();
    m_pKeyboardDevice = pInputManager->CreateAndGetDevice<gainput::InputDeviceKeyboard>();

    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab]         = gainput::KeyTab;
    io.KeyMap[ImGuiKey_LeftArrow]   = gainput::KeyLeft;
    io.KeyMap[ImGuiKey_RightArrow]  = gainput::KeyRight;
    io.KeyMap[ImGuiKey_UpArrow]     = gainput::KeyUp;
    io.KeyMap[ImGuiKey_DownArrow]   = gainput::KeyDown;
    io.KeyMap[ImGuiKey_PageUp]      = gainput::KeyPageUp;
    io.KeyMap[ImGuiKey_PageDown]    = gainput::KeyPageDown;
    io.KeyMap[ImGuiKey_Home]        = gainput::KeyHome;
    io.KeyMap[ImGuiKey_End]         = gainput::KeyEnd;
    io.KeyMap[ImGuiKey_Delete]      = gainput::KeyDelete;
    io.KeyMap[ImGuiKey_Backspace]   = gainput::KeyBackSpace;
    io.KeyMap[ImGuiKey_Enter]       = gainput::KeyReturn;
    io.KeyMap[ImGuiKey_Escape]      = gainput::KeyEscape;
    io.KeyMap[ImGuiKey_A]           = gainput::KeyA;
    io.KeyMap[ImGuiKey_C]           = gainput::KeyC;
    io.KeyMap[ImGuiKey_V]           = gainput::KeyV;
    io.KeyMap[ImGuiKey_X]           = gainput::KeyX;
    io.KeyMap[ImGuiKey_Y]           = gainput::KeyY;
    io.KeyMap[ImGuiKey_Z]           = gainput::KeyZ;
}

void UIPass::AllocateResources()
{
#ifdef _DEBUG
    for (unsigned int i = 0; i < ArtistParameter::GetParameterCount() - 1; i++)
    {
        for (unsigned int j = i + 1; j < ArtistParameter::GetParameterCount(); j++)
        {
            // Due to a bug(?) in ImGui, we can't have a window with more than
            // one checkbox with the same labels, or the others won't work.
            assert(ArtistParameter::GetParameterByIdx(i)->GetName() != ArtistParameter::GetParameterByIdx(j)->GetName());
        }
    }
#endif

    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResMgr = RenderContext->GetResourceManager();
    if (!ResMgr)
        return;

    ImGuiIO& io = ImGui::GetIO();

    if (m_nFontTextureIdx == ~0u && !m_pFontTexture)
    {
        unsigned char* pixels;
        int width, height, bytes_per_pixel;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

        m_nFontTextureIdx = ResMgr->CreateTexture(PF_A8R8G8B8, TT_2D, width, height);
        m_pFontTexture = ResMgr->GetTexture(m_nFontTextureIdx);

        m_pFontTexture->Lock(0, BL_WRITE_ONLY);
        assert(m_pFontTexture->GetMipSizeBytes() == width * height * bytes_per_pixel);
        memcpy(m_pFontTexture->GetMipData(), pixels, width * height * bytes_per_pixel);
        m_pFontTexture->Update();
        m_pFontTexture->Unlock();

        m_pFontTexture->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);

        io.Fonts->TexID = m_pFontTexture;
    }

    for (unsigned int i = 0; i < UI_BUFFER_COUNT; i++)
    {
        m_nImGuiVfIdx[i] = ResMgr->CreateVertexFormat(
            3,
            VAS_POSITION, VAT_FLOAT2, 0,
            VAS_COLOR, VAT_UBYTE4, 0,
            VAS_TEXCOORD, VAT_FLOAT2, 0
        );
        m_pImGuiVf[i] = ResMgr->GetVertexFormat(m_nImGuiVfIdx[i]);
    }

    for (unsigned int paramIdx = 0; paramIdx < ArtistParameter::GetParameterCount(); paramIdx++)
    {
        const ArtistParameter* const param = ArtistParameter::GetParameterByIdx(paramIdx);

        if (param)
        {
            bool found = false;
            for (unsigned int i = 0; i < m_arrParamCategoryWindowStates.size(); i++)
            {
                if (m_arrParamCategoryWindowStates[i].categoryName == param->GetCategoryName())
                {
                    found = true;
                }
            }

            if (!found)
            {
                m_arrParamCategoryWindowStates.push_back(ParamCategoryWindowState());
                m_arrParamCategoryWindowStates.back().categoryName = param->GetCategoryName();
                m_arrParamCategoryWindowStates.back().windowOpen = false;
            }
        }
    }

    if (!m_pDummyTex1D)
    {
        m_nDummyTex1DIdx = ResMgr->CreateTexture(PF_A8, TT_1D, 1);
        m_pDummyTex1D = ResMgr->GetTexture(m_nDummyTex1DIdx);
    }

    if (!m_pDummyTex2D)
    {
        m_nDummyTex2DIdx = ResMgr->CreateTexture(PF_A8, TT_2D, 1, 1);
        m_pDummyTex2D = ResMgr->GetTexture(m_nDummyTex2DIdx);
    }

    if (!m_pDummyTex3D)
    {
        m_nDummyTex3DIdx = ResMgr->CreateTexture(PF_A8, TT_3D, 1, 1, 1);
        m_pDummyTex3D = ResMgr->GetTexture(m_nDummyTex3DIdx);
    }

    if (!m_pDummyTexCube)
    {
        m_nDummyTexCubeIdx = ResMgr->CreateTexture(PF_A8, TT_CUBE, 1, 1);
        m_pDummyTexCube = ResMgr->GetTexture(m_nDummyTexCubeIdx);
    }
}

void UIPass::ReleaseResources()
{
    Renderer* const RenderContext = Renderer::GetInstance();
    ResourceManager* const ResMgr = RenderContext ? RenderContext->GetResourceManager() : nullptr;
    ImGuiIO& io = ImGui::GetIO();

    if (!ResMgr)
        return;

    if (m_nFontTextureIdx != ~0u)
    {
        ResMgr->ReleaseTexture(m_nFontTextureIdx);
        m_nFontTextureIdx = ~0u;
        m_pFontTexture = nullptr;
        io.Fonts->TexID = nullptr;
    }

    for (unsigned int i = 0; i < UI_BUFFER_COUNT; i++)
    {
        if (m_nImGuiVfIdx[i] != ~0u)
        {
            ResMgr->ReleaseVertexFormat(m_nImGuiVfIdx[i]);
            m_nImGuiVfIdx[i] = ~0u;
            m_pImGuiVf[i] = nullptr;
        }

        if (m_nImGuiIbIdx[i] != ~0u)
        {
            ResMgr->ReleaseIndexBuffer(m_nImGuiIbIdx[i]);
            m_nImGuiIbIdx[i] = ~0u;
            m_pImGuiIb[i] = nullptr;

        }

        if (m_nImGuiVbIdx[i] != ~0u)
        {
            ResMgr->ReleaseVertexBuffer(m_nImGuiVbIdx[i]);
            m_nImGuiVbIdx[i] = ~0u;
            m_pImGuiVb[i] = nullptr;
        }
    }

    if (m_nDummyTex1DIdx != ~0u)
    {
        ResMgr->ReleaseTexture(m_nDummyTex1DIdx);
        m_pDummyTex1D = nullptr;
    }

    if (m_nDummyTex2DIdx != ~0u)
    {
        ResMgr->ReleaseTexture(m_nDummyTex2DIdx);
        m_pDummyTex2D = nullptr;
    }

    if (m_nDummyTex3DIdx != ~0u)
    {
        ResMgr->ReleaseTexture(m_nDummyTex3DIdx);
        m_pDummyTex3D = nullptr;
    }

    if (m_nDummyTexCubeIdx != ~0u)
    {
        ResMgr->ReleaseTexture(m_nDummyTexCubeIdx);
        m_pDummyTexCube = nullptr;
    }
}

void GPUProfileMarkerResultHistory::Update(const float fDeltaTime)
{
    m_fTimeAccum += fDeltaTime;

    if (m_fTimeAccum >= MARKER_RESULT_HISTORY)
    {
        m_fTimeAccum = 0.f;
        m_nCurrBufferIdx = (m_nCurrBufferIdx + 1) % 2;
        m_arrGPUProfileMarkerResultHistory[m_nCurrBufferIdx].clear();
    }
}

const float GPUProfileMarkerResultHistory::GetAverage(const char* const name) const
{
    const int historyBufferIdx = (m_nCurrBufferIdx + 1) % 2;
    float totalTime = 0.f;
    int markerCount = 0;

    for (unsigned int i = 0; i < m_arrGPUProfileMarkerResultHistory[historyBufferIdx].size(); i++)
    {
        if (m_arrGPUProfileMarkerResultHistory[historyBufferIdx][i].name == name)
        {
            totalTime += m_arrGPUProfileMarkerResultHistory[historyBufferIdx][i].timing;
            markerCount++;
        }
    }

    if (markerCount)
    {
        return totalTime / (float)markerCount;
    }
    else
    {
        return 0.f;
    }
}

void GPUProfileMarkerResultHistory::PushMarker(const GPUProfileMarkerResultCacheEntry& marker)
{
    m_arrGPUProfileMarkerResultHistory[m_nCurrBufferIdx].push_back(marker);
}
