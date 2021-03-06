#include "stdafx.h"

#include <IL/il.h>
#include <IL/ilu.h>
//#include <IL/ilut.h>

#include <External/lz4/lz4hc.h>

#include <Renderer.h>
#include <Texture.h>
#include <ResourceManager.h>
using namespace Synesthesia3D;

#include "../Common/Logging.h"
#include "TextureCompiler.h"
#include "../Utility/ColorUtility.h"
using namespace Synesthesia3DTools;

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
            case 4:
                return PF_X8R8G8B8;
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
            // info.Bpp appears to be broken, since it is reporting 4 instead of 16 Bpp,
            // although info.Data points to memory with 32bit floating point data
            //if (info.Bpp == 16)
            return PF_A32B32G32R32F;
            //else
            //{
            //  assert(false);
            //  return PF_NONE;
            //}
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
        if (info.Type == IL_UNSIGNED_BYTE)
            switch (info.Bpp)
            {
            case 2:
                return PF_R5G6B5;
            case 3:
                return PF_R8G8B8;
            case 4:
                return PF_X8R8G8B8;
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
    char outputLogDirPath[1024] = "";
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

                if (_stricmp(argv[arg], "X8R8G8B8") == 0)
                {
                    format = PF_X8R8G8B8;
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

            if (_stricmp(argv[arg], "-log") == 0)
            {
                arg++;
                strcpy_s(outputLogDirPath, argv[arg]);
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
        cout << "-log output/dir/\tOverride default log output directory (output/dir/ must exist!)" << endl;
        cout << "-mip n\tNumber of generated mip levels: 0 (default) = all, 1 = none, etc." << endl << endl;

        cout << "Pixel formats:" << endl;
        cout << "*small-bit color formats: R5G6B5, A1R5G5B5, A4R4G4B4" << endl;
        cout << "*8-bit integer formats: A8, L8, A8L8, R8G8B8, X8R8G8B8, A8R8G8B8, A8B8G8R8" << endl;
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

    if (strlen(outputLogDirPath) == 0)
        strcpy_s(outputLogDirPath, "Logs");

    if (!(CreateDirectoryA(outputLogDirPath, NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
    {
        cout << "TextureCompiler requires write permission into the current directory";
        return;
    }

    sprintf_s(logName, 1024, "%s\\TextureCompiler_%s_%s.log", outputLogDirPath, time, fileName);
    mstream Log(logName, ofstream::trunc, !bQuiet);

    const unsigned long long startTick = GetTickCount64();

    if (ilGetInteger(IL_VERSION_NUM) != IL_VERSION ||
        iluGetInteger(ILU_VERSION_NUM) != ILU_VERSION /*||
        ilutGetInteger(ILUT_VERSION_NUM) != ILUT_VERSION*/)
    {
        Log << "[ERROR] Mismatch between DevIL binaries version and expected version\n";
        return;
    }

    Log << "Compiling: \"" << argv[argc - 1] << "\"\n";

    ilInit();
    iluInit();
    //ilutInit();

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

    TextureType texType = TT_2D;
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

        Log << "\tPixel format: " << Renderer::GetEnumString(srcFormat) << "\n";
        Log << "\tTexture type: " << Renderer::GetEnumString(texType) << "\n";
        Log << "\tWidth: " << info.Width << "\n";
        Log << "\tHeight: " << info.Height << "\n";
        Log << "\tDepth: " << info.Depth << "\n";
        Log << "\tSize of data: " << info.SizeOfData << "\n";
        Log << "\tMip count: " << texDst->GetMipCount() << "\n";

        if (texDst->GetTextureType() == TT_CUBE)
        {
            for (CubeFace face = FACE_XNEG; face < FACE_MAX; face = (CubeFace)(face + 1))
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

        Log << "\tSource pixel format: " << Renderer::GetEnumString(srcFormat) << "\n";
        Log << "\tDestination pixel format: " << Renderer::GetEnumString(format) << "\n";
        Log << "\tTexture type: " << Renderer::GetEnumString(texType) << "\n";
        Log << "\tWidth: " << info.Width << "\n";
        Log << "\tHeight: " << info.Height << "\n";
        Log << "\tDepth: " << info.Depth << "\n";
        Log << "\tSize of source data: " << info.SizeOfData << "\n";
        Log << "\tSize of destination data: " << texDst->GetSize() << "\n";
        Log << "\tMip count: " << texDst->GetMipCount() << "\n";

        Vec4f* tmpData = nullptr;
        tmpData = new Vec4f[info.Width * info.Height * info.Depth];

        Log << "\t[INFO] Converting from " << Renderer::GetEnumString(srcFormat) << " to " << Renderer::GetEnumString(format) << "\n";

        const unsigned long long convertStart = GetTickCount64();
        if (texDst->GetTextureType() == TT_CUBE)
        {
            for (CubeFace face = FACE_XNEG; face < FACE_MAX; face = (CubeFace)(face + 1))
            {
                ilBindImage(imgSrc);
                ilActiveFace(face);

                ILinfo faceInfo;
                iluGetImageInfo(&faceInfo);

                assert(ilGetInteger(IL_IMAGE_CUBEFLAGS) == IL_CUBEMAP_POSITIVEX * powf(2.f, (float)face));

                ColorUtility::ConvertFrom[srcFormat](faceInfo.Data, tmpData, faceInfo.Width, faceInfo.Height, faceInfo.Depth);
                ColorUtility::ConvertTo[texDst->GetPixelFormat()](tmpData, texDst->GetMipData(face, 0), texDst->GetWidth(), texDst->GetHeight(), texDst->GetDepth());
            }
        }
        else
        {
            ColorUtility::ConvertFrom[srcFormat](info.Data, tmpData, info.Width, info.Height, info.Depth);
            ColorUtility::ConvertTo[texDst->GetPixelFormat()](tmpData, texDst->GetMipData(), texDst->GetWidth(), texDst->GetHeight(), texDst->GetDepth());
        }

        Log << "\t[INFO] Conversion finished in " << (float)(GetTickCount64() - convertStart) << " ms\n";

        delete[] tmpData;
        tmpData = nullptr;
    }

    if (mipCount != 1)
    {
        Log << "\t[INFO] Generating mipmaps...\n";
        const unsigned long long mipStart = GetTickCount64();
        if (texDst->GenerateMips())
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
    outFilePath += ".s3dtex";

    ostringstream rawTextureBuffer;
    rawTextureBuffer << *texDst;

    const unsigned int uncompressedSize = (unsigned int)rawTextureBuffer.str().size();
    const unsigned int compressedSizeMax = (unsigned int)LZ4_compressBound(uncompressedSize);
    char* const compressedBuffer = new char[compressedSizeMax];
    const int compressedSize = LZ4_compress_HC(rawTextureBuffer.str().c_str(), compressedBuffer, (int)rawTextureBuffer.str().size(), compressedSizeMax, LZ4HC_CLEVEL_DEFAULT);
    //const int compressedSize = LZ4_compress_default(rawTextureBuffer.str().c_str(), compressedBuffer, (int)rawTextureBuffer.str().size(), compressedSizeMax);

    ofstream outTexture;
    outTexture.open(outFilePath.c_str(), ofstream::trunc | ofstream::binary);
#ifdef _DEBUG
    outTexture.setf(ios_base::unitbuf);
#endif
    outTexture.write(S3D_TEXTURE_FILE_HEADER, S3D_TEXTURE_FILE_HEADER_SIZE);
    const unsigned int fileVersion = S3D_TEXTURE_FILE_VERSION;
    outTexture.write((char*)&fileVersion, sizeof(unsigned int));
    outTexture.write((char*)&compressedSize, sizeof(unsigned int));
    outTexture.write((char*)&uncompressedSize, sizeof(unsigned int));
    outTexture.write(compressedBuffer, compressedSize);
    outTexture.close();

    delete[] compressedBuffer;

#ifdef _DEBUG
    texIdx = resMan->CreateTexture(outFilePath.c_str());
    Texture* texIn = resMan->GetTexture(texIdx);

    assert(texIn->GetPixelFormat() == texDst->GetPixelFormat());
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
