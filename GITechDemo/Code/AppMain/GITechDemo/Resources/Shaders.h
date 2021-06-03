/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   Shaders.h
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

#ifndef SHADERS_H_
#define SHADERS_H_
namespace GITechDemoApp
{
    namespace HLSL
    {
        #include "BokehDoF.hlsl"
        #include "DirectionalLightVolume.hlsl"
        #include "NearestDepthUpscale.hlsl"
        #include "LumaCapture.hlsl"
        #include "Downsample.hlsl"
        #include "MotionBlur.hlsl"
        #include "LumaAdapt.hlsl"
        #include "Bloom.hlsl"
        #include "GBufferGeneration.hlsl"
        #include "DepthPassAlphaTest.hlsl"
        #include "DepthPass.hlsl"
        #include "BilateralBlur.hlsl"
        #include "DepthCopy.hlsl"
        #include "RSMApply.hlsl"
        #include "RSMCapture.hlsl"
        #include "RSMUpscale.hlsl"
        #include "ScreenSpaceReflection.hlsl"
        #include "DirectionalLight.hlsl"
        #include "Skybox.hlsl"
        #include "SphericalLensFlareFeatures.hlsl"
        #include "SSAO.hlsl"
        #include "AnamorphicLensFlareBlur.hlsl"
        #include "AnamorphicLensFlareFeatures.hlsl"
        #include "LensFlareApply.hlsl"
        #include "HDRToneMapping.hlsl"
        #include "FXAA.hlsl"
        #include "ColorCopy.hlsl"
        #include "UI.hlsl"
        #include "ASCIIEffect.hlsl"
    }
}
#endif // SHADERS_H_
