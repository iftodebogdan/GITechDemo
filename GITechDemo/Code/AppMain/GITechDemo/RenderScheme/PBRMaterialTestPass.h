/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   PBRMaterialTestPass.h
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

#ifndef PBR_MATERIAL_TEST_PASS_H_
#define PBR_MATERIAL_TEST_PASS_H_

#include "RenderPass.h"
#include "RenderResource.h"

namespace GITechDemoApp
{
    struct PBRMaterialDescription
    {
        enum PBRTextureType
        {
            PBRTT_ALBEDO = 0,
            PBRTT_NORMAL,
            PBRTT_ROUGHNESS,
            PBRTT_MATERIAL,
            PBRTT_MAX
        };

        std::string  szMaterialName;
        std::string  szFolderName;
        Texture*     arrTextures[PBRTT_MAX];

        PBRMaterialDescription(const char* const materialName, const char* const folderName);
        ~PBRMaterialDescription();
    };

    class PBRMaterialTestPass : public RenderPass
    {
        IMPLEMENT_RENDER_PASS(PBRMaterialTestPass)

    public:
        static Matrix44f CalculateWorldMatrixForSphereIdx(const unsigned int idx);
    };
}

#endif // PBR_MATERIAL_TEST_PASS_H_
