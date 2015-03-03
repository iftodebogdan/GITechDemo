//////////////////////////////////////////////////////////////////////////
// This file is part of the "LibRenderer" 3D graphics library           //
//                                                                      //
// Copyright (C) 2014 - Iftode Bogdan-Marius <iftode.bogdan@gmail.com>  //
//                                                                      //
// This program is free software: you can redistribute it and/or modify //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// This program is distributed in the hope that it will be useful,      //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program. If not, see <http://www.gnu.org/licenses/>. //
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <IL/il.h>
#include <IL/ilu.h>

#include "Texture.h"
#include "TextureLoader.h"
using namespace LibRendererDll;

bool TextureLoader::bIsInitialized = false;
ILuint img = -1;

TextureLoader::ImageDesc TextureLoader::LoadImageFile(const char* const path, const bool convertToARGB)
{
	if (!bIsInitialized)
	{
		if (ilGetInteger(IL_VERSION_NUM) != IL_VERSION ||
			iluGetInteger(ILU_VERSION_NUM) != ILU_VERSION)
			assert(false);
		ilInit();
		iluInit();
	}

	if (img != -1)
		ilDeleteImage(img);

	img = ilGenImage();
	ilBindImage(img);
	ilLoadImage(path);
	
	assert(ilGetData());

	ImageDesc imgDesc;
	memset(&imgDesc, 0, sizeof(imgDesc));
	if (!ilGetData())
	{
		ilDeleteImage(img);
		return imgDesc;
	}

	imgDesc.width = ilGetInteger(IL_IMAGE_WIDTH);
	imgDesc.height = ilGetInteger(IL_IMAGE_HEIGHT);
	imgDesc.depth = ilGetInteger(IL_IMAGE_DEPTH);
	imgDesc.mipmaps = ilGetInteger(IL_NUM_MIPMAPS) + 1;
	
	if (ilGetInteger(IL_IMAGE_CUBEFLAGS) == IL_CUBEMAP_POSITIVEX)
		imgDesc.type = TT_CUBE;
	else
		if (imgDesc.height == 1)
			imgDesc.type = TT_1D;
		else
			imgDesc.type = TT_2D;

	if (convertToARGB)
	{
		if (imgDesc.type == TT_CUBE)
			for (unsigned int face = 0; face < 6; face++)
			{
				ilBindImage(img);
				ilActiveFace(face);
				ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);
			}
		else
			ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);
	}
	imgDesc.bpp = ilGetInteger(IL_IMAGE_BITS_PER_PIXEL);

	switch (ilGetInteger(IL_IMAGE_FORMAT))
	{
	case IL_ALPHA:
		switch (ilGetInteger(IL_IMAGE_TYPE))
		{
		case IL_BYTE:
		case IL_UNSIGNED_BYTE:
			imgDesc.format = PF_A8;
			break;
		default:
			assert(false);
		}
		break;

	case IL_RGB:
		switch (ilGetInteger(IL_IMAGE_TYPE))
		{
		case IL_BYTE:
		case IL_UNSIGNED_BYTE:
			imgDesc.format = PF_R8G8B8;
			break;
		case IL_SHORT:
		case IL_UNSIGNED_SHORT:
		case IL_INT:
		case IL_UNSIGNED_INT:
		case IL_FLOAT:
		case IL_DOUBLE:
		case IL_HALF:
		default:
			assert(false);
		}
		break;

	case IL_RGBA:
		switch (ilGetInteger(IL_IMAGE_TYPE))
		{
		case IL_BYTE:
		case IL_UNSIGNED_BYTE:
			imgDesc.format = PF_A8B8G8R8;
			break;
		case IL_SHORT:
		case IL_UNSIGNED_SHORT:
			imgDesc.format = PF_A16B16G16R16;
			break;
		case IL_FLOAT:
			imgDesc.format = PF_A32B32G32R32F;
			break;
		case IL_HALF:
			imgDesc.format = PF_A16B16G16R16F;
			break;
		case IL_INT:
		case IL_UNSIGNED_INT:
		case IL_DOUBLE:
		default:
			assert(false);
		}
		break;

	case IL_BGR:
		switch (ilGetInteger(IL_IMAGE_TYPE))
		{
		case IL_BYTE:
		case IL_UNSIGNED_BYTE:
			imgDesc.format = PF_R8G8B8;
			break;
		case IL_SHORT:
		case IL_UNSIGNED_SHORT:
			imgDesc.format = PF_A16B16G16R16;
			break;
		case IL_FLOAT:
			imgDesc.format = PF_A32B32G32R32F;
			break;
		case IL_HALF:
			imgDesc.format = PF_A16B16G16R16F;
			break;
		case IL_INT:
		case IL_UNSIGNED_INT:
		case IL_DOUBLE:
		default:
			assert(false);
		}
		break;

	case IL_BGRA:
		switch (ilGetInteger(IL_IMAGE_TYPE))
		{
		case IL_BYTE:
		case IL_UNSIGNED_BYTE:
			imgDesc.format = PF_A8R8G8B8;
			break;
		case IL_SHORT:
		case IL_UNSIGNED_SHORT:
			imgDesc.format = PF_A16B16G16R16;
			break;
		case IL_FLOAT:
			imgDesc.format = PF_A32B32G32R32F;
			break;
		case IL_HALF:
			imgDesc.format = PF_A16B16G16R16F;
			break;
		case IL_INT:
		case IL_UNSIGNED_INT:
		case IL_DOUBLE:
		default:
			assert(false);
		}
		break;

	case IL_LUMINANCE:
		switch (ilGetInteger(IL_IMAGE_TYPE))
		{
		case IL_SHORT:
		case IL_UNSIGNED_SHORT:
			imgDesc.format = PF_L8;
			break;
		case IL_INT:
		case IL_UNSIGNED_INT:
			imgDesc.format = PF_L16;
			break;
		case IL_BYTE:
		case IL_UNSIGNED_BYTE:
		case IL_FLOAT:
		case IL_HALF:
		case IL_DOUBLE:
		default:
			assert(false);
		}
		break;

	case IL_LUMINANCE_ALPHA:
		switch (ilGetInteger(IL_IMAGE_TYPE))
		{
		case IL_SHORT:
		case IL_UNSIGNED_SHORT:
			imgDesc.format = PF_A8L8;
			break;
		case IL_INT:
		case IL_UNSIGNED_INT:
		case IL_BYTE:
		case IL_UNSIGNED_BYTE:
		case IL_FLOAT:
		case IL_HALF:
		case IL_DOUBLE:
		default:
			assert(false);
		}
		break;

	default:
		assert(false);
	}

	return imgDesc;
}

void TextureLoader::CopyImageData(Texture* outTex)
{
	assert(outTex && img != -1);

	if (!outTex || img == -1)
		return;

	if (outTex->IsLocked())
		outTex->Unlock();

	if (outTex->GetTextureType() == TT_CUBE)
	{
		for (unsigned int face = 0; face < 6; face++)
		{
			for (unsigned int mip = 0; mip < outTex->GetMipmapLevelCount(); mip++)
			{
				ilBindImage(img);
				ilActiveFace(face);
				if ((unsigned int)ilGetInteger(IL_NUM_MIPMAPS) + 1 < outTex->GetMipmapLevelCount())
					iluBuildMipmaps();
				ilActiveMipmap(mip);

				const unsigned int width = ilGetInteger(IL_IMAGE_WIDTH);
				const unsigned int height = ilGetInteger(IL_IMAGE_HEIGHT);
				const unsigned int depth = ilGetInteger(IL_IMAGE_DEPTH);
				const unsigned int bpp = ilGetInteger(IL_IMAGE_BITS_PER_PIXEL);

				outTex->Lock(face, mip, BL_WRITE_ONLY);
				memcpy(
					outTex->GetMipmapLevelData(face, mip),
					ilGetData(),
					width * height * depth * (bpp / 8)
					);
				outTex->Update();
				outTex->Unlock();
			}
		}
	}
	else
	{
		for (unsigned int mip = 0; mip < outTex->GetMipmapLevelCount(); mip++)
		{
			ilBindImage(img);
			if ((unsigned int)ilGetInteger(IL_NUM_MIPMAPS) + 1 < outTex->GetMipmapLevelCount())
				iluBuildMipmaps();
			ilActiveMipmap(mip);
			const unsigned int width = ilGetInteger(IL_IMAGE_WIDTH);
			const unsigned int height = ilGetInteger(IL_IMAGE_HEIGHT);
			const unsigned int depth = ilGetInteger(IL_IMAGE_DEPTH);
			const unsigned int bpp = ilGetInteger(IL_IMAGE_BITS_PER_PIXEL);
			outTex->Lock(mip, BL_WRITE_ONLY);
			memcpy(
				outTex->GetMipmapLevelData(mip),
				ilGetData(),
				width * height * depth * (bpp / 8)
				);
			outTex->Update();
			outTex->Unlock();
		}
	}
}

void TextureLoader::UnloadImageFile()
{
	 if (img != -1)
		 ilDeleteImage(img);
	 img = -1;
}