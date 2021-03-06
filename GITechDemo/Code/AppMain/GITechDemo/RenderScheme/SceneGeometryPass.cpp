/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   SceneGeometryPass.cpp
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
#include <Profiler.h>
using namespace Synesthesia3D;

#include "SceneGeometryPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

SceneGeometryPass::SceneGeometryPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

SceneGeometryPass::~SceneGeometryPass()
{}

void SceneGeometryPass::Update(const float fDeltaTime)
{}

void SceneGeometryPass::Draw()
{
    Renderer* RenderContext = Renderer::GetInstance();
    ResourceManager* const ResMgr = RenderContext ? RenderContext->GetResourceManager() : nullptr;
    if (!RenderContext || !ResMgr)
        return;

    const bool zWriteEnabled = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
    const Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
    bool red, blue, green, alpha;
    RenderContext->GetRenderStateManager()->GetColorWriteEnabled(red, green, blue, alpha);

    // A depth prepass is useful if we have expensive pixel shaders in our
    // G-Buffer generation process. It allows us to avoid shading pixels
    // that eventually get overwritten by other pixels that have smaller
    // depth values. We are not fill-rate bound, so the depth prepass is disabled.
    if (RenderConfig::GBuffer::ZPrepass)
    {
        PUSH_PROFILE_MARKER("Z prepass");

        RenderContext->GetRenderStateManager()->SetColorWriteEnabled(false, false, false, false);

        // Draw solid objects
        DepthPassShader.Enable();

        for (unsigned int mesh = 0; mesh < SponzaScene.GetModel()->arrMesh.size(); mesh++)
        {
            const unsigned int matIdx = SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx;
            const s3dSampler2D diffuseTexIdx = SponzaScene.GetTexture(Synesthesia3D::Model::TextureDesc::TT_DIFFUSE, matIdx);
            Synesthesia3D::Texture* const diffuseTex = ResMgr->GetTexture(diffuseTexIdx);
            const s3dByte* const texDiffuseData = diffuseTex->GetMipData();

            assert(diffuseTex->GetPixelFormat() == PF_X8R8G8B8 || diffuseTex->GetPixelFormat() == PF_A8R8G8B8);

            if (SponzaScene.GetModel()->arrMaterial[matIdx]->fOpacity >= 1.f)
            {
                PUSH_PROFILE_MARKER(SponzaScene.GetModel()->arrMaterial[SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx]->szName.c_str());
                RenderContext->DrawVertexBuffer(SponzaScene.GetModel()->arrMesh[mesh]->pVertexBuffer);
                POP_PROFILE_MARKER();
            }
        }

        DepthPassShader.Disable();

        if (RenderConfig::GBuffer::DrawAlphaTestGeometry)
        {
            // Draw alpha test objects
            DepthPassAlphaTestShader.Enable();

            for (unsigned int mesh = 0; mesh < SponzaScene.GetModel()->arrMesh.size(); mesh++)
            {
                const unsigned int matIdx = SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx;
                const s3dSampler2D diffuseTexIdx = SponzaScene.GetTexture(Synesthesia3D::Model::TextureDesc::TT_DIFFUSE, matIdx);
                Synesthesia3D::Texture* const diffuseTex = ResMgr->GetTexture(diffuseTexIdx);
                const s3dByte* const texDiffuseData = diffuseTex->GetMipData();

                if (SponzaScene.GetModel()->arrMaterial[matIdx]->fOpacity < 1.f)
                {
                    HLSL::DepthPassAlphaTest_Diffuse = diffuseTexIdx;
                    DepthPassAlphaTestShader.CommitShaderInputs();

                    PUSH_PROFILE_MARKER(SponzaScene.GetModel()->arrMaterial[SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx]->szName.c_str());
                    RenderContext->DrawVertexBuffer(SponzaScene.GetModel()->arrMesh[mesh]->pVertexBuffer);
                    POP_PROFILE_MARKER();
                }
            }

            DepthPassAlphaTestShader.Disable();
        }

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

        if (diffuseTexIdx != ~0u && ((matTexIdx != ~0u && roughnessTexIdx != ~0u) || RenderConfig::DirectionalLight::BRDFModel == HLSL::BRDF::BlinnPhong))
        {
            RenderContext->GetResourceManager()->GetTexture(diffuseTexIdx)->SetAnisotropy((unsigned int)RenderConfig::GBuffer::DiffuseAnisotropy);

            // Normally, we'd set diffuse textures to linearize sRGB when sampling in the GBuffer generation shader, but actually we want to keep non-linear sRGB data in the GBuffer.
            // The reason for this is that the albedo buffer is 8bpc and banding is a very real possibility. Keeping sRGB data up until we're actually using the data in calculations
            // (lighting code) allows us to maintain our light accumulation buffer free of quantization artifacts. We also need to set the albedo buffer as sRGB so that we actually
            // linearize the data when sampling from the GBuffer during the lighting phase. That is done in GBufferPass.cpp in the Update() function.
            // Kudos to VladC of FUN labs for pointing this out!

            // RenderContext->GetResourceManager()->GetTexture(diffuseTexIdx)->SetSRGBEnabled(true);

            HLSL::GBufferGeneration_Diffuse = diffuseTexIdx;
            HLSL::GBufferGeneration_Normal = normalTexIdx;
            HLSL::GBufferGenerationParams->HasNormalMap = (HLSL::GBufferGeneration_Normal != -1) && RenderConfig::GBuffer::UseNormalMaps;

            // For Blinn-Phong BRDF
            HLSL::GBufferGeneration_Spec = specTexIdx;
            HLSL::GBufferGenerationParams->HasSpecMap = (HLSL::GBufferGeneration_Spec != -1);
            HLSL::GBufferGenerationParams->SpecIntensity = SponzaScene.GetModel()->arrMaterial[SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx]->fShininessStrength;

            // For Cook-Torrance BRDF
            HLSL::GBufferGeneration_MatType = matTexIdx;
            HLSL::GBufferGeneration_Roughness = roughnessTexIdx;

            GBufferGenerationShader.Enable();
            RenderContext->DrawVertexBuffer(SponzaScene.GetModel()->arrMesh[mesh]->pVertexBuffer);
            GBufferGenerationShader.Disable();
        }

        POP_PROFILE_MARKER();
    }

    if (RenderConfig::GBuffer::ZPrepass)
    {
        RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWriteEnabled);
        RenderContext->GetRenderStateManager()->SetZFunc(zFunc);
        POP_PROFILE_MARKER();
    }
}

void SceneGeometryPass::AllocateResources()
{

}

void SceneGeometryPass::ReleaseResources()
{

}
