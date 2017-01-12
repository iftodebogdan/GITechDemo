/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   GBufferPass.cpp
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

#include <Renderer.h>
#include <RenderState.h>
#include <ResourceManager.h>
#include <Texture.h>
#include <RenderTarget.h>
#include <Profiler.h>
using namespace Synesthesia3D;

#include "App.h"

#include "GBufferPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

namespace GITechDemoApp
{
    /* G-Buffer generation */
    bool GBUFFER_Z_PREPASS = false;
    int DIFFUSE_ANISOTROPY = MAX_ANISOTROPY;
    bool GBUFFER_USE_NORMAL_MAPS = true;
}

GBufferPass::GBufferPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

GBufferPass::~GBufferPass()
{}

void GBufferPass::Update(const float fDeltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetColorBuffer(0))->SetAddressingMode(SAM_CLAMP);
    ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetColorBuffer(1))->SetAddressingMode(SAM_CLAMP);
    ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetColorBuffer(2))->SetAddressingMode(SAM_CLAMP);
    ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetColorBuffer(3))->SetAddressingMode(SAM_CLAMP);
    ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetDepthBuffer())->SetAddressingMode(SAM_CLAMP);

    f44WorldViewMat = f44ViewMat * f44WorldMat;
    f44WorldViewProjMat = f44ProjMat * f44WorldViewMat;

    // In order to have better low-intensity precision in the G-Buffer (which is in LDR format, obviously)
    // we need to delay the gamma correction of input textures right up until we need them to be linear,
    // which is at the lighting stage. To achieve this, we need to keep the G-buffer in sRGB format.
    // The rest of the rendering pipeline supports HDR, so we keep color information in linear format
    // up until the tone mapping pass, where we output a LDR sRGB texture.
    // Kudos to VladC of FUN labs for pointing this out!
    ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetColorBuffer(0))->SetSRGBEnabled(true);

    texSource = GBuffer.GetRenderTarget()->GetDepthBuffer();
    f4TexSize = Vec4f(
        (float)GBuffer.GetRenderTarget()->GetWidth(),
        (float)GBuffer.GetRenderTarget()->GetHeight(),
        1.f / (float)GBuffer.GetRenderTarget()->GetWidth(),
        1.f / (float)GBuffer.GetRenderTarget()->GetHeight()
    );
    bDepthDownsample = true;
    f2DepthHalfTexelOffset = Vec2f(
        0.5f / (float)GBuffer.GetRenderTarget()->GetWidth(),
        0.5f / (float)GBuffer.GetRenderTarget()->GetHeight()
    );

    ResourceMgr->GetTexture(LinearFullDepthBuffer.GetRenderTarget()->GetColorBuffer())->SetAddressingMode(SAM_CLAMP);
    ResourceMgr->GetTexture(LinearQuarterDepthBuffer.GetRenderTarget()->GetColorBuffer())->SetAddressingMode(SAM_CLAMP);
    ResourceMgr->GetTexture(HyperbolicQuarterDepthBuffer.GetRenderTarget()->GetColorBuffer())->SetAddressingMode(SAM_CLAMP);
    ResourceMgr->GetTexture(LinearFullDepthBuffer.GetRenderTarget()->GetColorBuffer())->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(LinearQuarterDepthBuffer.GetRenderTarget()->GetColorBuffer())->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(HyperbolicQuarterDepthBuffer.GetRenderTarget()->GetColorBuffer())->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

    DIFFUSE_ANISOTROPY = Math::clamp(DIFFUSE_ANISOTROPY, 1, (int)MAX_ANISOTROPY);
    nBRDFModel = gmtl::Math::clamp(nBRDFModel.GetCurrentValue(), (int)BLINN_PHONG, (int)BRDF_MODEL_MAX - 1);
}

void GBufferPass::Draw()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    GBuffer.Enable();

    RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

    const bool zWriteEnabled = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
    const Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
    bool red, blue, green, alpha;
    RenderContext->GetRenderStateManager()->GetColorWriteEnabled(red, green, blue, alpha);

    // A depth prepass is useful if we have expensive pixel shaders in our
    // G-Buffer generation process. It allows us to avoid shading pixels
    // that eventually get overwritten by other pixels that have smaller
    // depth values. We are not fill-rate bound, so the depth prepass is disabled.
    if (GBUFFER_Z_PREPASS)
    {
        PUSH_PROFILE_MARKER("Z prepass");

        RenderContext->GetRenderStateManager()->SetColorWriteEnabled(false, false, false, false);

        DepthPassShader.Enable();

        for (unsigned int mesh = 0; mesh < SponzaScene.GetModel()->arrMesh.size(); mesh++)
        {
            PUSH_PROFILE_MARKER(SponzaScene.GetModel()->arrMaterial[SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx]->szName.c_str());
            RenderContext->DrawVertexBuffer(SponzaScene.GetModel()->arrMesh[mesh]->pVertexBuffer);
            POP_PROFILE_MARKER();
        }

        DepthPassShader.Disable();

        RenderContext->GetRenderStateManager()->SetColorWriteEnabled(red, green, blue, alpha);
        RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
        RenderContext->GetRenderStateManager()->SetZFunc(CMP_EQUAL);

        POP_PROFILE_MARKER();

        PUSH_PROFILE_MARKER("Capture");
    }

    // A visibility test would be useful if we were CPU bound (or vertex bound).
    // However, there isn't a reason to do such an optimization for now, since the
    // scene isn't very big and we are mostly pixel bound.
    for (unsigned int mesh = 0; mesh < SponzaScene.GetModel()->arrMesh.size(); mesh++)
    {
        PUSH_PROFILE_MARKER(SponzaScene.GetModel()->arrMaterial[SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx]->szName.c_str());

        const unsigned int diffuseTexIdx = SponzaScene.GetTexture(Synesthesia3D::Model::TextureDesc::TT_DIFFUSE, SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx);
        const unsigned int normalTexIdx = SponzaScene.GetTexture(Synesthesia3D::Model::TextureDesc::TT_HEIGHT, SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx);
        const unsigned int specTexIdx = SponzaScene.GetTexture(Synesthesia3D::Model::TextureDesc::TT_SPECULAR, SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx);
        const unsigned int matTexIdx = SponzaScene.GetTexture(Synesthesia3D::Model::TextureDesc::TT_AMBIENT, SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx);
        const unsigned int roughnessTexIdx = SponzaScene.GetTexture(Synesthesia3D::Model::TextureDesc::TT_SHININESS, SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx);

        if (diffuseTexIdx != ~0u && ((matTexIdx != ~0u && roughnessTexIdx != ~0u) || nBRDFModel == BLINN_PHONG))
        {
            RenderContext->GetResourceManager()->GetTexture(diffuseTexIdx)->SetAnisotropy((unsigned int)DIFFUSE_ANISOTROPY);

            texDiffuse = diffuseTexIdx;
            texNormal = normalTexIdx;
            bHasNormalMap = (texNormal != -1) && GBUFFER_USE_NORMAL_MAPS;

            // For Blinn-Phong BRDF
            texSpec = (nBRDFModel == BLINN_PHONG ? specTexIdx : ~0u);
            bHasSpecMap = (texSpec != -1);
            fSpecIntensity = SponzaScene.GetModel()->arrMaterial[SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx]->fShininessStrength;

            // For Cook-Torrance BRDF
            texMatType = (nBRDFModel == COOK_TORRANCE_GGX || nBRDFModel == COOK_TORRANCE_BECKMANN ? matTexIdx : ~0u);
            texRoughness = (nBRDFModel == COOK_TORRANCE_GGX || nBRDFModel == COOK_TORRANCE_BECKMANN ? roughnessTexIdx : ~0u);

            GBufferGenerationShader.Enable();
            RenderContext->DrawVertexBuffer(SponzaScene.GetModel()->arrMesh[mesh]->pVertexBuffer);
            GBufferGenerationShader.Disable();
        }

        POP_PROFILE_MARKER();
    }

    if (GBUFFER_Z_PREPASS)
    {
        RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWriteEnabled);
        RenderContext->GetRenderStateManager()->SetZFunc(zFunc);
        POP_PROFILE_MARKER();
    }

    GBuffer.Disable();

    // Generate linear depth buffer and quarter-resolution versions of the hyperbolic and linear depth buffers
    const bool blendEnable = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

    bReconstructDepth = false;
    nDownsampleFactor = 4;
    DownsampleShader.Enable();
    HyperbolicQuarterDepthBuffer.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    HyperbolicQuarterDepthBuffer.Disable();
    DownsampleShader.Disable();

    bReconstructDepth = true;
    nDownsampleFactor = 1;
    DownsampleShader.Enable();
    LinearFullDepthBuffer.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    LinearFullDepthBuffer.Disable();
    DownsampleShader.Disable();

    bReconstructDepth = true;
    nDownsampleFactor = 4;
    DownsampleShader.Enable();
    LinearQuarterDepthBuffer.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    LinearQuarterDepthBuffer.Disable();
    DownsampleShader.Disable();

    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);
}
