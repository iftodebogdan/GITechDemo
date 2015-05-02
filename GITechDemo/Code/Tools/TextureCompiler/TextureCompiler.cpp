#include "stdafx.h"

#include <Renderer.h>
using namespace LibRendererDll;

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include "../Common/Logging.h"
#include "TextureCompiler.h"
#include "../Utility/ColorUtility.h"
using namespace LibRendererTools;

#define ERROR_OK 0
#define ERROR_FATAL 1

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

const char * LibRendererTools::TextureCompiler::GetEnumString(PixelFormat val)
{
	switch (val)
	{
	case PF_R5G6B5:
		return "R5G6B5";
	case PF_A1R5G5B5:
		return "A1R5G5B5";
	case PF_A4R4G4B4:
		return "A4R4G4B4";
	case PF_A8:
		return "A8";
	case PF_L8:
		return "L8";
	case PF_A8L8:
		return "A8L8";
	case PF_R8G8B8:
		return "R8G8B8";
	case PF_A8R8G8B8:
		return "A8R8G8B8";
	case PF_A8B8G8R8:
		return "A8B8G8R8";
	case PF_L16:
		return "L16";
	case PF_G16R16:
		return "G16R16";
	case PF_A16B16G16R16:
		return "A16B16G16R16";
	case PF_R16F:
		return "R16F";
	case PF_G16R16F:
		return "G16R16F";
	case PF_A16B16G16R16F:
		return "A16B16G16R16F";
	case PF_R32F:
		return "R32F";
	case PF_G32R32F:
		return "G32R32F";
	case PF_A32B32G32R32F:
		return "A32B32G32R32F";
	case PF_DXT1:
		return "DXT1";
	case PF_DXT3:
		return "DXT3";
	case PF_DXT5:
		return "DXT5";
	default:
		assert(false);
		return "";
	}
}

const char * LibRendererTools::TextureCompiler::GetEnumString(TexType val)
{
	switch (val)
	{
	case TT_1D:
		return "1D";
	case TT_2D:
		return "2D";
	case TT_3D:
		return "3D";
	case TT_CUBE:
		return "Cubemap";
	default:
		return "";
	}
}

bool TextureCompiler::HandleDevilErrors(mstream& logStream)
{
	bool shouldExit = ERROR_OK;
	ILenum err = ilGetError();
	if (err != IL_NO_ERROR)
	{
		do
		{
			logStream << "[ERROR] " << iluErrorString(err) << "\n";
			shouldExit = ERROR_FATAL;
			err = ilGetError();
		} while (err != IL_NO_ERROR);
	}
	return shouldExit;
}

PixelFormat TextureCompiler::GetPixelFormat(const ILinfo & info, bool& swizzle)
{
	swizzle = false;
	switch (info.Format)
	{
	case IL_ALPHA:
		if (info.Type == IL_UNSIGNED_BYTE && info.Bpp == 1)
			return PF_A8;
		else
		{
			assert(false);
			return PF_NONE;
		}

	case IL_RGB:
		swizzle = true;
		if (info.Type == IL_UNSIGNED_BYTE)
			switch (info.Bpp)
			{
			case 2:
				return PF_R5G6B5;
			case 3:
				return PF_R8G8B8;
			default:
				assert(false);
				return PF_NONE;
			}
		else
		{
			assert(false);
			return PF_NONE;
		}

	case IL_RGBA:
		switch (info.Type)
		{
		case IL_UNSIGNED_BYTE:
			if (info.Bpp == 4)
				return PF_A8B8G8R8;
			else
			{
				assert(false);
				return PF_NONE;
			}
		case IL_FLOAT:
			if (info.Bpp == 16)
				return PF_A32B32G32R32F;
			else
			{
				assert(false);
				return PF_NONE;
			}
		case IL_HALF:
			if (info.Bpp == 8)
				return PF_A16B16G16R16F;
			else
			{
				assert(false);
				return PF_NONE;
			}
		default:
			assert(false);
			return PF_NONE;
		}

	case IL_BGR:
		if(info.Type == IL_UNSIGNED_BYTE)
			switch (info.Bpp)
			{
			case 2:
				return PF_R5G6B5;
			case 3:
				return PF_R8G8B8;
			default:
				assert(false);
				return PF_NONE;
			}
		else
		{
			assert(false);
			return PF_NONE;
		}

	case IL_BGRA:
		if (info.Type == IL_UNSIGNED_BYTE)
			switch (info.Bpp)
			{
			case 2:
				return PF_A1R5G5B5;
				//return PF_A4R4G4B4;
			case 4:
				return PF_A8R8G8B8;
			default:
				assert(false);
				return PF_NONE;
			}
		else
		{
			assert(false);
			return PF_NONE;
		}

	case IL_LUMINANCE:
		if (info.Type == IL_UNSIGNED_BYTE && info.Bpp == 1)
			return PF_L8;
		else
		{
			assert(false);
			return PF_NONE;
		}

	case IL_LUMINANCE_ALPHA:
		if (info.Type == IL_UNSIGNED_BYTE && info.Bpp == 2)
			return PF_A8L8;
		else
		{
			assert(false);
			return PF_NONE;
		}

	default:
		assert(false);
		return PF_NONE;
	}
}

void TextureCompiler::Run(int argc, char* argv[])
{
	bool bValidCmdParams = false;
	bool bQuiet = false;
	PixelFormat format = PF_NONE;
	char outputDirPath[1024] = "";
	unsigned int mipCount = 0;

	for (unsigned int arg = 1; arg < (unsigned int)argc; arg++)
	{
		if (arg != argc - 1)
		{
			if (_stricmp(argv[arg], "-q") == 0)
			{
				bQuiet = true;
				continue;
			}

			if (_stricmp(argv[arg], "-f") == 0)
			{
				arg++;

				if (_stricmp(argv[arg], "R5G6B5") == 0)
				{
					format = PF_R5G6B5;
					continue;
				}

				if (_stricmp(argv[arg], "A1R5G5B5") == 0)
				{
					format = PF_A1R5G5B5;
					continue;
				}

				if (_stricmp(argv[arg], "A4R4G4B4") == 0)
				{
					format = PF_A4R4G4B4;
					continue;
				}

				if (_stricmp(argv[arg], "A8") == 0)
				{
					format = PF_A8;
					continue;
				}

				if (_stricmp(argv[arg], "L8") == 0)
				{
					format = PF_L8;
					continue;
				}

				if (_stricmp(argv[arg], "A8L8") == 0)
				{
					format = PF_A8L8;
					continue;
				}

				if (_stricmp(argv[arg], "R8G8B8") == 0)
				{
					format = PF_R8G8B8;
					continue;
				}

				if (_stricmp(argv[arg], "A8R8G8B8") == 0)
				{
					format = PF_A8R8G8B8;
					continue;
				}

				if (_stricmp(argv[arg], "A8B8G8R8") == 0)
				{
					format = PF_A8B8G8R8;
					continue;
				}

				if (_stricmp(argv[arg], "L16") == 0)
				{
					format = PF_L16;
					continue;
				}

				if (_stricmp(argv[arg], "G16R16") == 0)
				{
					format = PF_G16R16;
					continue;
				}

				if (_stricmp(argv[arg], "A16B16G16R16") == 0)
				{
					format = PF_A16B16G16R16;
					continue;
				}

				if (_stricmp(argv[arg], "R16F") == 0)
				{
					format = PF_R16F;
					continue;
				}

				if (_stricmp(argv[arg], "G16R16F") == 0)
				{
					format = PF_G16R16F;
					continue;
				}

				if (_stricmp(argv[arg], "A16B16G16R16F") == 0)
				{
					format = PF_A16B16G16R16F;
					continue;
				}

				if (_stricmp(argv[arg], "R32F") == 0)
				{
					format = PF_R32F;
					continue;
				}

				if (_stricmp(argv[arg], "G32R32F") == 0)
				{
					format = PF_G32R32F;
					continue;
				}

				if (_stricmp(argv[arg], "A32B32G32R32F") == 0)
				{
					format = PF_A32B32G32R32F;
					continue;
				}

				if (_stricmp(argv[arg], "DXT1") == 0)
				{
					format = PF_DXT1;
					continue;
				}

				if (_stricmp(argv[arg], "DXT3") == 0)
				{
					format = PF_DXT3;
					continue;
				}

				if (_stricmp(argv[arg], "DXT5") == 0)
				{
					format = PF_DXT5;
					continue;
				}
			}

			if (_stricmp(argv[arg], "-d") == 0)
			{
				arg++;
				strcpy_s(outputDirPath, argv[arg]);
				continue;
			}

			if (_stricmp(argv[arg], "-mip") == 0)
			{
				arg++;
				mipCount = atoi(argv[arg]);
				continue;
			}

			break;
		}
		else
		{
			if (argv[arg][0] == '-')
				break;
			else
				bValidCmdParams = true;
		}
	}

	if (!bValidCmdParams)
	{
		cout << "Usage: TextureCompiler [options] Path\\To\\texture_file.ext" << endl << endl;
		cout << "Options:" << endl;
		cout << "-q\t\tQuiet. Does not produce output to the console window" << endl;
		cout << "-f format\tOptional pixel format (see below) for conversion" << endl;
		cout << "-d output/dir/\tOverride default output directory (output/dir/ must exist!)" << endl;
		cout << "-mip n\tNumber of generated mip levels: 0 (default) = all, 1 = none, etc." << endl << endl;
		
		cout << "Pixel formats:" << endl;
		cout << "*small-bit color formats: R5G6B5, A1R5G5B5, A4R4G4B4" << endl;
		cout << "*8-bit integer formats: A8, L8, A8L8, R8G8B8, A8R8G8B8, A8B8G8R8" << endl;
		cout << "*16-bit integer formats: L16, G16R16, A16B16G16R16" << endl;
		cout << "*16-bit floating-point formats: R16F, G16R16F, A16B16G16R16F" << endl;
		cout << "*32-bit floating-point formats: R32F, G32R32F, A32B32G32R32F" << endl;
		cout << "*DXT compressed formats: DXT1, DXT3, DXT5" << endl << endl;
		return;
	}

	char dirPath[1024];
	char fileName[256];
	char time[80];
	char logName[1024];

	_splitpath_s(argv[argc - 1], (char*)nullptr, 0, dirPath, 1024, fileName, 256, (char*)nullptr, 0);

	std::time_t rawtime;
	std::tm* timeinfo = new std::tm;
	std::time(&rawtime);
	localtime_s(timeinfo, &rawtime);
	std::strftime(time, 80, "%Y%m%d%H%M%S", timeinfo);
	delete timeinfo;

	if (!(CreateDirectoryA("Logs", NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
	{
		cout << "TextureCompiler requires write permission into the current directory";
		return;
	}

	sprintf_s(logName, 1024, "Logs\\TextureCompiler_%s_%s.log", time, fileName);
	mstream Log(logName, ofstream::trunc, !bQuiet);

	const unsigned long long startTick = GetTickCount64();

	if (ilGetInteger(IL_VERSION_NUM) != IL_VERSION ||
		iluGetInteger(ILU_VERSION_NUM) != ILU_VERSION ||
		ilutGetInteger(ILUT_VERSION_NUM) != ILUT_VERSION)
	{
		Log << "[ERROR] Mismatch between DevIL binaries version and expected version\n";
		return;
	}

	Log << "Compiling: \"" << argv[argc - 1] << "\"\n";

	ilInit();
	iluInit();
	ilutInit();

	//ilEnable(IL_ORIGIN_SET);
	//ilSetInteger(IL_ORIGIN_MODE, IL_ORIGIN_UPPER_LEFT);

	if (HandleDevilErrors(Log) != ERROR_OK)
		return;

	ILuint imgSrc = -1;
	ilLoadImage(argv[argc - 1]);
	if (HandleDevilErrors(Log) != ERROR_OK)
		return;
	imgSrc = ilGetInteger(IL_CUR_IMAGE);

	if (!ilGetData())
	{
		Log << "[ERROR] Could not retrieve pixel data from source image!\n";
		assert(false);
		return;
	}

	Renderer::CreateInstance(API_NULL);
	Renderer* renderer = Renderer::GetInstance();
	renderer->Initialize(nullptr);
	ResourceManager* const resMan = renderer->GetResourceManager();

	if (!renderer || !resMan)
	{
		Log << "[ERROR] Could not initialize renderer or resource manager!\n";
		return;
	}

	Log << "\n[TEXTURE]\n";

	ILinfo info;
	iluGetImageInfo(&info);

	unsigned int texIdx = -1;
	Texture* texDst = nullptr;
	bool swizzle = false;

	TexType texType = TT_2D;
	if (ilGetInteger(IL_IMAGE_CUBEFLAGS) == IL_CUBEMAP_POSITIVEX) // info.CubeFlags is broken
		texType = TT_CUBE;
	else if (info.Height == 1)
		texType = TT_1D;
	else if (info.Depth == 1)
		texType = TT_2D;
	else
		texType = TT_3D;

	PixelFormat srcFormat = GetPixelFormat(info, swizzle);
	
	if (swizzle)
	{
		Log << "\t[INFO] Unsupported pixel ordering detected: swizzling...\n";
		const unsigned long long swizzleStart = GetTickCount64();
		if (iluSwapColours())
			Log << "\t[INFO] Swizzling done in " << (float)(GetTickCount64() - swizzleStart) << " ms\n";
		else
			Log << "\t[INFO] Swizzling could not be performed\n";
	}

	if (info.Origin == IL_ORIGIN_LOWER_LEFT)
	{
		Log << "\t[INFO] Source image has a lower left origin: flipping...\n";
		const unsigned long long flipStart = GetTickCount64();
		if (iluFlipImage())
			Log << "\t[INFO] Flipping done in " << (float)(GetTickCount64() - flipStart) << " ms\n";
		else
			Log << "\t[INFO] Flipping could not be performed\n";
	}
	
	if (format == PF_NONE || format == srcFormat)
	{
		format = srcFormat;
		texIdx = resMan->CreateTexture(format, texType, info.Width, info.Height, info.Depth, mipCount, BU_TEXTURE);
		
		if (texIdx != -1)
		{
			texDst = resMan->GetTexture(texIdx);
		}
		else
		{
			Log << "[ERROR] Could not create texture resource!\n";
			Log << "[/TEXTURE]\n";
			Renderer::DestroyInstance();
			assert(false);
			return;
		}
		
		Log << "\tPixel format: " << GetEnumString(srcFormat) << "\n";
		Log << "\tTexture type: " << GetEnumString(texType) << "\n";
		Log << "\tWidth: " << info.Width << "\n";
		Log << "\tHeight: " << info.Height << "\n";
		Log << "\tDepth: " << info.Depth << "\n";
		Log << "\tSize of data: " << info.SizeOfData << "\n";
		Log << "\tMip count: " << texDst->GetMipCount() << "\n";

		if (texDst->GetTextureType() == TT_CUBE)
		{
			for (unsigned int face = 0; face < 6; face++)
			{
				ilBindImage(imgSrc);
				ilActiveFace(face);

				ILinfo faceInfo;
				iluGetImageInfo(&faceInfo);

				assert(ilGetInteger(IL_IMAGE_CUBEFLAGS) == IL_CUBEMAP_POSITIVEX * powf(2.f, (float)face));
				assert(faceInfo.SizeOfData == texDst->GetMipSizeBytes());

				memcpy(texDst->GetMipData(face, 0), faceInfo.Data, texDst->GetMipSizeBytes(0));
			}
		}
		else
		{
			assert(info.SizeOfData == texDst->GetMipSizeBytes());
			memcpy(texDst->GetMipData(), info.Data, texDst->GetMipSizeBytes());
		}
	}
	else
	{
		texIdx = resMan->CreateTexture(format, texType, info.Width, info.Height, info.Depth, mipCount, BU_TEXTURE);

		if (texIdx != -1)
		{
			texDst = resMan->GetTexture(texIdx);
		}
		else
		{
			Log << "[ERROR] Could not create texture resource!\n";
			Log << "[/TEXTURE]\n";
			Renderer::DestroyInstance();
			assert(false);
			return;
		}

		Log << "\tSource pixel format: " << GetEnumString(srcFormat) << "\n";
		Log << "\tDestination pixel format: " << GetEnumString(format) << "\n";
		Log << "\tTexture type: " << GetEnumString(texType) << "\n";
		Log << "\tWidth: " << info.Width << "\n";
		Log << "\tHeight: " << info.Height << "\n";
		Log << "\tDepth: " << info.Depth << "\n";
		Log << "\tSize of source data: " << info.SizeOfData << "\n";
		Log << "\tSize of destination data: " << texDst->GetSize() << "\n";
		Log << "\tMip count: " << texDst->GetMipCount() << "\n";

		Vec4f* tmpData = nullptr;
		tmpData = new Vec4f[info.Width * info.Height * info.Depth];

		Log << "\t[INFO] Converting from " << GetEnumString(srcFormat) << " to " << GetEnumString(format) << "\n";
		
		const unsigned long long convertStart = GetTickCount64();
		if (texDst->GetTextureType() == TT_CUBE)
		{
			for (unsigned int face = 0; face < 6; face++)
			{
				ilBindImage(imgSrc);
				ilActiveFace(face);

				ILinfo faceInfo;
				iluGetImageInfo(&faceInfo);

				assert(ilGetInteger(IL_IMAGE_CUBEFLAGS) == IL_CUBEMAP_POSITIVEX * powf(2.f, (float)face));

				ColorUtility::ConvertFrom[srcFormat](faceInfo.Data, tmpData, faceInfo.Width, faceInfo.Height, faceInfo.Depth);
				ColorUtility::ConvertTo[texDst->GetTextureFormat()](tmpData, texDst->GetMipData(face, 0), texDst->GetWidth(), texDst->GetHeight(), texDst->GetDepth());
			}
		}
		else
		{
			ColorUtility::ConvertFrom[srcFormat](info.Data, tmpData, info.Width, info.Height, info.Depth);
			ColorUtility::ConvertTo[texDst->GetTextureFormat()](tmpData, texDst->GetMipData(), texDst->GetWidth(), texDst->GetHeight(), texDst->GetDepth());
		}

		Log << "\t[INFO] Conversion finished in " << (float)(GetTickCount64() - convertStart) << " ms\n";

		delete[] tmpData;
		tmpData = nullptr;
	}

	if (mipCount != 1)
	{
		Log << "\t[INFO] Generating mipmaps...\n";
		const unsigned long long mipStart = GetTickCount64();
		if (texDst->GenerateMipmaps())
			Log << "\t[INFO] Mipmaps generated in " << (float)(GetTickCount64() - mipStart) << " ms\n";
		else
		{
			Log << "[ERROR] Could not generate mipmaps!\n";
			Log << "[/TEXTURE]\n";
			Renderer::DestroyInstance();
			assert(false);
			return;
		}
	}

	Log << "[/TEXTURE]\n";

	Log << "\nCompilation of \"" << argv[argc - 1] << "\" finished in " << (float)(GetTickCount64() - startTick) / 1000.f << " seconds\n";

	string outFilePath = outputDirPath;
	outFilePath = ReplaceAll(outFilePath, "/", "\\");
	if (strlen(outputDirPath))
		system(("if not exist " + outFilePath + " mkdir " + outFilePath).c_str()); // crude, but effective for recursive directory creation
	else
	{
		CreateDirectoryA("Out", NULL);
		outFilePath = "Out\\Textures\\";
		CreateDirectoryA(outFilePath.c_str(), NULL);
		outFilePath += fileName;
		CreateDirectoryA(outFilePath.c_str(), NULL);
	}
	outFilePath += "\\";
	outFilePath += fileName;
	outFilePath += ".lrt";

	ofstream outTexture;
	outTexture.open(outFilePath.c_str(), ofstream::trunc | ofstream::binary);
#ifdef _DEBUG
	outTexture.setf(ios_base::unitbuf);
#endif
	outTexture << *texDst;
	outTexture.close();

#ifdef _DEBUG
	texIdx = resMan->CreateTexture(outFilePath.c_str());
	Texture* texIn = resMan->GetTexture(texIdx);

	assert(texIn->GetTextureFormat() == texDst->GetTextureFormat());
	assert(texIn->GetTextureType() == texDst->GetTextureType());
	assert(texIn->GetMipCount() == texDst->GetMipCount());
	assert(texIn->GetDimensionCount() == texDst->GetDimensionCount());
	assert(((Buffer*)texIn)->GetSize() == ((Buffer*)texDst)->GetSize());
	assert(memcmp(((Buffer*)texIn)->GetData(), ((Buffer*)texDst)->GetData(), ((Buffer*)texDst)->GetSize()) == 0);
#endif

	Renderer::DestroyInstance();
}

int main(int argc, char* argv[])
{
	TextureCompiler tc;
	tc.Run(argc, argv);

	return 0;
}
