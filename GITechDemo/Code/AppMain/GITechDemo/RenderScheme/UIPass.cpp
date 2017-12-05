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

#define STEP_FAST (10.f)
#define BUTTON_WIDTH (100.f)
#define ALPHA_PER_SECOND (5.f)
#define ALPHA_MIN (0.25f)

UIPass::UIPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
    , m_pKeyboardDevice(nullptr)
    , m_pMouseDevice(nullptr)
    , m_pFontTexture(nullptr)
    , m_nFontTextureIdx(~0u)
    , m_nImGuiVfIdx(~0u)
    , m_pImGuiVf(nullptr)
    , m_nImGuiIbIdx(~0u)
    , m_pImGuiIb(nullptr)
    , m_nImGuiVbIdx(~0u)
    , m_pImGuiVb(nullptr)
    , m_bShowAllParameters(false)
    , m_bShowProfiler(ENABLE_PROFILE_MARKERS)
    , m_fAlpha(0.f)
{}

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
}

void UIPass::Draw()
{
    SetupUI();
    GenerateDrawData();
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
        if (level > 0)
        {
            const GPUProfileMarkerResult* const marker = RenderContext->GetProfiler()->RetrieveGPUProfileMarker(pass->GetPassName());
            float timing = marker ? marker->GetTiming() : 0.f;
            float start = marker ? marker->GetStart() : 0.f;
            float end = marker ? marker->GetEnd() : 0.f;

            const GPUProfileMarkerResult* const rootMarker = RenderContext->GetProfiler()->RetrieveGPUProfileMarker(RenderScheme::GetRootPass().GetPassName());
            float rootTiming = rootMarker ? rootMarker->GetTiming() : 0.f;
            float rootStart = rootMarker ? rootMarker->GetStart() : 0.f;
            float rootEnd = rootMarker ? rootMarker->GetEnd() : 0.f;

            if (timing == 0.f)
                return;

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

            const unsigned int passNameHash = S3DHASH(pass->GetPassName());
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

        ImGui::Text("%s: %f ms", pass->GetPassName(), timing);

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
    //bool show_test_window = false;
    //ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
    //ImGui::ShowTestWindow(&show_test_window);

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

        ImGui::MenuItem(m_bShowProfiler ? "Hide profiler" : "Show profiler", "Debug/Profile only", &m_bShowProfiler,
        #if ENABLE_PROFILE_MARKERS
            true
        #else
            false
        #endif
        );

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
        const float mainMenuBarHeight = ImGui::GetFontSize() + style.FramePadding.y * 2.f;
        const ImVec2 window_pos = ImVec2(io.DisplaySize.x - style.WindowPadding.x, mainMenuBarHeight + style.WindowPadding.y);
        const ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f);

        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - style.WindowPadding.x * 2.f, io.DisplaySize.y - mainMenuBarHeight - style.WindowPadding.y * 2.f));

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        if (ImGui::Begin("GPU Profiler Bars", &m_bShowProfiler, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs))
        {
            DrawGPUProfileBars();
            ImGui::End();
        }
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.75f));
        if (ImGui::Begin("GPU Profiler Details", &m_bShowProfiler, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs))
        {
            ImGui::SetWindowPos(ImVec2(io.DisplaySize.x - style.WindowPadding.x - ImGui::GetWindowWidth(), io.DisplaySize.y - style.WindowPadding.y - ImGui::GetWindowHeight()));
            DrawGPUProfileDetails();
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

    const bool isUIInFocus = ((GITechDemo*)AppMain)->IsUIInFocus();

    const bool needsIbReallocation = !m_pImGuiIb || m_pImGuiIb->GetElementCount() < (unsigned int)drawData->TotalIdxCount;
    if (needsIbReallocation)
    {
        if (m_nImGuiIbIdx != ~0u)
        {
            ResMgr->ReleaseIndexBuffer(m_nImGuiIbIdx);
        }

        m_nImGuiIbIdx = ResMgr->CreateIndexBuffer(Math::Max(drawData->TotalIdxCount, 1), sizeof(ImDrawIdx) == 2 ? IBF_INDEX16 : IBF_INDEX32);
        m_pImGuiIb = ResMgr->GetIndexBuffer(m_nImGuiIbIdx);
        assert(sizeof(ImDrawIdx) == m_pImGuiIb->GetElementSize());
        assert(m_pImGuiIb->GetElementCount() <= Math::pow(2.f, sizeof(ImDrawIdx) * 8.f));

        //cout << "UI index buffer reallocation: " << m_pImGuiIb->GetElementCount() << " indices." << endl;
    }

    const bool needsVbReallocation = !m_pImGuiVb || m_pImGuiVb->GetElementCount() < (unsigned int)drawData->TotalVtxCount;
    if (needsIbReallocation || needsVbReallocation)
    {
        const unsigned int prevMaxVtxCount = m_pImGuiVb ? m_pImGuiVb->GetElementCount() : 1;
        if (m_nImGuiVbIdx != ~0u)
        {
            ResMgr->ReleaseVertexBuffer(m_nImGuiVbIdx);
        }

        m_nImGuiVbIdx = ResMgr->CreateVertexBuffer(m_pImGuiVf, Math::Max(prevMaxVtxCount, (unsigned int)drawData->TotalVtxCount), m_pImGuiIb);
        m_pImGuiVb = ResMgr->GetVertexBuffer(m_nImGuiVbIdx);

        //cout << "UI vertex buffer reallocation: " << m_pImGuiVb->GetElementCount() << " vertices." << endl;
    }

    m_pImGuiIb->Lock(BL_WRITE_ONLY);
    m_pImGuiVb->Lock(BL_WRITE_ONLY);

    for (int n = 0, vtxOffset = 0, idxOffset = 0; n < drawData->CmdListsCount; n++)
    {
        const ImDrawList* const cmdList = drawData->CmdLists[n];
        const ImDrawVert* vtxSrc = cmdList->VtxBuffer.Data;

        m_pImGuiIb->SetIndices(cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size, idxOffset);
        idxOffset += cmdList->IdxBuffer.Size;

        for (int i = 0; i < cmdList->VtxBuffer.Size; i++)
        {
            const s3dByte R = ((vtxSrc->col & 0x000000FF) >> 0);
            const s3dByte G = ((vtxSrc->col & 0x0000FF00) >> 8);
            const s3dByte B = ((vtxSrc->col & 0x00FF0000) >> 16);
            const s3dByte A = ((vtxSrc->col & 0xFF000000) >> 24);// / (isUIInFocus ? 1 : 2);

            m_pImGuiVb->Position<Vec2f>(vtxOffset + i) = Vec2f(vtxSrc->pos.x, vtxSrc->pos.y);
            m_pImGuiVb->Color<s3dDword>(vtxOffset + i, 0) = (A << 24) | (R << 16) | (G << 8) | (B << 0); // RGBA --> ARGB for DirectX9
            m_pImGuiVb->TexCoord<Vec2f>(vtxOffset + i, 0) = Vec2f(vtxSrc->uv.x, vtxSrc->uv.y);

            vtxSrc++;
        }
        vtxOffset += cmdList->VtxBuffer.Size;
    }

    m_pImGuiIb->Update();
    m_pImGuiVb->Update();

    m_pImGuiIb->Unlock();
    m_pImGuiVb->Unlock();
}

void UIPass::RenderUI()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    RenderState* RSMgr = RenderContext->GetRenderStateManager();
    if (!RSMgr)
        return;

    ImDrawData* drawData = ImGui::GetDrawData();
    if (!drawData)
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

    // Setup shader inputs
    texFont = m_nFontTextureIdx;

    ImGuiIO& io = ImGui::GetIO();
    const float L = 0.5f, R = io.DisplaySize.x + 0.5f, T = 0.5f, B = io.DisplaySize.y + 0.5f;
    f44UIProjMat.GetCurrentValue().set(
        2.0f / (R - L), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (T - B), 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        (L + R) / (L - R), (T + B) / (B - T), 0.5f, 1.0f
    );
    gmtl::transpose(f44UIProjMat.GetCurrentValue());

    // Render geometry
    UIShader.Enable();
    for (int n = 0, vtxOffset = 0, idxOffset = 0; n < drawData->CmdListsCount; n++)
    {
        const ImDrawList* cmdList = drawData->CmdLists[n];
        for (int i = 0; i < cmdList->CmdBuffer.Size; i++)
        {
            const ImDrawCmd* cmd = &cmdList->CmdBuffer[i];
            RSMgr->SetScissor(Vec2i(int(cmd->ClipRect.z - cmd->ClipRect.x), int(cmd->ClipRect.w - cmd->ClipRect.y)), Vec2i(int(cmd->ClipRect.x), int(cmd->ClipRect.y)));
            RenderContext->DrawVertexBuffer(m_pImGuiVb, vtxOffset, cmd->ElemCount / 3, cmdList->VtxBuffer.Size, idxOffset);
            idxOffset += cmd->ElemCount;
        }
        vtxOffset += cmdList->VtxBuffer.Size;
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

    m_nImGuiVfIdx = ResMgr->CreateVertexFormat(
        3,
        VAS_POSITION, VAT_FLOAT2, 0,
        VAS_COLOR, VAT_UBYTE4, 0,
        VAS_TEXCOORD, VAT_FLOAT2, 0
    );
    m_pImGuiVf = ResMgr->GetVertexFormat(m_nImGuiVfIdx);

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

    if (m_nImGuiVfIdx != ~0u)
    {
        ResMgr->ReleaseVertexFormat(m_nImGuiVfIdx);
        m_nImGuiVfIdx = ~0u;
        m_pImGuiVf = nullptr;
    }

    if (m_nImGuiIbIdx != ~0u)
    {
        ResMgr->ReleaseIndexBuffer(m_nImGuiIbIdx);
        m_nImGuiIbIdx = ~0u;
        m_pImGuiIb = nullptr;

    }

    if (m_nImGuiVbIdx != ~0u)
    {
        ResMgr->ReleaseVertexBuffer(m_nImGuiVbIdx);
        m_nImGuiVbIdx = ~0u;
        m_pImGuiVb = nullptr;
    }
}
