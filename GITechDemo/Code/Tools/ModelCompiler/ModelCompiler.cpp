#include "stdafx.h"

#include <External/lz4/lz4hc.h>

#include <Renderer.h>
#include <ResourceManager.h>
#include <VertexBuffer.h>
#include <VertexFormat.h>
#include <IndexBuffer.h>
#include <ResourceData.h>
using namespace Synesthesia3D;

#include "../Common/Logging.h"
#include "ModelCompiler.h"
using namespace Synesthesia3DTools;

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

void ModelCompiler::Run(int argc, char* argv[])
{
    bool bValidCmdParams = false;
    bool bQuiet = false;
    char outputDirPath[1024] = "";
    char outputLogDirPath[1024] = "";

    for (unsigned int arg = 1; arg < (unsigned int)argc; arg++)
    {
        if (arg != argc - 1)
        {
            if (_stricmp(argv[arg], "-q") == 0)
            {
                bQuiet = true;
                continue;
            }

            if (_stricmp(argv[arg], "-d") == 0)
            {
                arg++;
                strcpy_s(outputDirPath, argv[arg]);
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
        cout << "Usage: ModelCompiler [options] Path\\To\\model_file.ext" << endl << endl;
        cout << "Options:" << endl;
        cout << "-q\t\tQuiet. Does not produce output to the console window" << endl;
        cout << "-d output/dir/\tOverride default output directory (output/dir/ must exist!)" << endl;
        cout << "-log output/dir/\tOverride default log output directory (output/dir/ must exist!)" << endl;
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
        cout << "ModelCompiler requires write permission into the current directory";
        return;
    }

    sprintf_s(logName, 1024, "%s\\ModelCompiler_%s_%s.log", outputLogDirPath, time, fileName);
    mstream Log(logName, ofstream::trunc, !bQuiet);

    Log << "Compiling: \"" << argv[argc - 1] << "\"\n";

    unsigned long long startTick = GetTickCount64();

    Assimp::Importer importer;
    const aiScene* scene = nullptr;

    unsigned int ppFlags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded | aiProcess_OptimizeGraph;
    ppFlags &= ~aiProcess_FindDegenerates;

    scene = importer.ReadFile(argv[argc - 1], ppFlags);

    if (!scene)
    {
        Log << "[ERROR] " << importer.GetErrorString() << "\n";
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

    Model model;
    unsigned int modelVertexCount = 0;
    unsigned int modelTexRefCount = 0;

    Log << "\nMesh count: " << scene->mNumMeshes << "\n";

    for (unsigned int meshIdx = 0; meshIdx < scene->mNumMeshes; meshIdx++)
    {
        Log << "[MESH]" << "\n";

        model.arrMesh.push_back(new Model::Mesh);

        model.arrMesh.back()->szName = scene->mMeshes[meshIdx]->mName.C_Str();
        Log << "\tName: " << model.arrMesh.back()->szName.c_str() << "\n";
        Log << "\tMesh index: " << meshIdx << "\n";

        std::vector<VertexAttributeSemantic> arrVAS;
        unsigned int countUVComponents[AI_MAX_NUMBER_OF_TEXTURECOORDS] = { 0 };
        unsigned int maxUVChannels = 0;
        unsigned int maxColorChannels = 0;
        unsigned int totalIndexCount = scene->mMeshes[meshIdx]->mNumFaces * 3;
        unsigned int skippedIndices = 0;
        unsigned int meshVertexCount = scene->mMeshes[meshIdx]->mNumVertices;
        modelVertexCount += meshVertexCount;

        Log << "\tVertex count: " << meshVertexCount << "\n";
        Log << "\tIndex count: " << totalIndexCount << "\n";

        Log << "\t[VERTEX FORMAT]" << "\n";

        if (scene->mMeshes[meshIdx]->HasPositions())
            if (std::find(arrVAS.begin(), arrVAS.end(), VAS_POSITION) == arrVAS.end())
            {
                arrVAS.push_back(VAS_POSITION);
                Log << "\t\tVAS_POSITION" << "\n";
            }

        if (scene->mMeshes[meshIdx]->HasNormals())
            if (std::find(arrVAS.begin(), arrVAS.end(), VAS_NORMAL) == arrVAS.end())
            {
                arrVAS.push_back(VAS_NORMAL);
                Log << "\t\tVAS_NORMAL" << "\n";
            }

        bool hasTexCoords = false;
        for (unsigned int tcIdx = 0; tcIdx < AI_MAX_NUMBER_OF_TEXTURECOORDS; tcIdx++)
            if (scene->mMeshes[meshIdx]->HasTextureCoords(tcIdx))
            {
                hasTexCoords = true;
                maxUVChannels = tcIdx + 1;
                countUVComponents[tcIdx] = scene->mMeshes[meshIdx]->mNumUVComponents[tcIdx];
                Log << "\t\tVAS_TEXCOORD" << "\n";
                Log << "\t\t\tChannel: " << tcIdx << "\n";
                Log << "\t\t\tFormat: VAT_FLOAT" << countUVComponents[tcIdx] << "\n";
            }
        if (hasTexCoords)
        {
            if (std::find(arrVAS.begin(), arrVAS.end(), VAS_TEXCOORD) == arrVAS.end())
                arrVAS.push_back(VAS_TEXCOORD);
        }

        bool hasVertexColors = false;
        for (unsigned int colorIdx = 0; colorIdx < AI_MAX_NUMBER_OF_COLOR_SETS; colorIdx++)
            if (scene->mMeshes[meshIdx]->HasVertexColors(colorIdx))
            {
                hasVertexColors = true;
                maxColorChannels = colorIdx + 1;
                Log << "\t\tVAS_COLOR" << "\n";
                Log << "\t\t\tChannel: " << colorIdx << "\n";
            }
        if (hasVertexColors)
            if (std::find(arrVAS.begin(), arrVAS.end(), VAS_COLOR) == arrVAS.end())
                arrVAS.push_back(VAS_COLOR);

        if (scene->mMeshes[meshIdx]->HasTangentsAndBitangents())
            if (std::find(arrVAS.begin(), arrVAS.end(), VAS_TANGENT) == arrVAS.end())
            {
                arrVAS.push_back(VAS_TANGENT);
                arrVAS.push_back(VAS_BINORMAL);
                Log << "\t\tVAS_TANGENT" << "\n";
                Log << "\t\tVAS_BINORMAL" << "\n";
            }

        Log << "\t[/VERTEX FORMAT]" << "\n";

        // Create the vertex format
        const unsigned int vfIdx = resMan->CreateVertexFormat((unsigned int)arrVAS.size()
            + (maxUVChannels ? maxUVChannels - 1 : 0)
            + (maxColorChannels ? maxColorChannels - 1 : 0));
        model.arrMesh.back()->pVertexFormat = resMan->GetVertexFormat(vfIdx);
        for (unsigned int vauIdx = 0, attribIdx = 0, offset = 0; vauIdx < arrVAS.size(); vauIdx++)
        {
            VertexAttributeType type = VAT_NONE;
            switch (arrVAS[vauIdx])
            {
            case VAS_POSITION:
            case VAS_NORMAL:
            case VAS_TANGENT:
            case VAS_BINORMAL:
                type = VAT_FLOAT3;
                break;
            case VAS_TEXCOORD:
                break;
            case VAS_COLOR:
                type = VAT_UBYTE4;
                break;
            default:
                assert(false);
            }

            if (arrVAS[vauIdx] == VAS_TEXCOORD)
                for (unsigned int uvCh = 0; uvCh < maxUVChannels; uvCh++)
                {
                    switch (countUVComponents[uvCh])
                    {
                    case 1:
                        type = VAT_FLOAT1;
                        break;
                    case 2:
                        type = VAT_FLOAT2;
                        break;
                    case 3:
                        type = VAT_FLOAT3;
                        break;
                    default:
                        assert(false);
                    }
                    model.arrMesh.back()->pVertexFormat->SetAttribute(attribIdx++, offset, arrVAS[vauIdx], type, uvCh);
                    offset += VertexFormat::GetAttributeTypeSize(type);
                }
            else
                if (arrVAS[vauIdx] == VAS_COLOR)
                    for (unsigned int clrCh = 0; clrCh < maxColorChannels; clrCh++)
                    {
                        model.arrMesh.back()->pVertexFormat->SetAttribute(attribIdx++, offset, arrVAS[vauIdx], type, clrCh);
                        offset += VertexFormat::GetAttributeTypeSize(type);
                    }
                else
                {
                    model.arrMesh.back()->pVertexFormat->SetAttribute(attribIdx++, offset, arrVAS[vauIdx], type, 0);
                    offset += VertexFormat::GetAttributeTypeSize(type);
                }
        }
        model.arrMesh.back()->pVertexFormat->SetStride(model.arrMesh.back()->pVertexFormat->CalculateStride());
        model.arrMesh.back()->pVertexFormat->Update();

        const unsigned int ibIdx = resMan->CreateIndexBuffer(totalIndexCount, totalIndexCount > 65535 ? IBF_INDEX32 : IBF_INDEX16);
        model.arrMesh.back()->pIndexBuffer = resMan->GetIndexBuffer(ibIdx);
        const unsigned int vbIdx = resMan->CreateVertexBuffer(model.arrMesh.back()->pVertexFormat, meshVertexCount, model.arrMesh.back()->pIndexBuffer);
        model.arrMesh.back()->pVertexBuffer = resMan->GetVertexBuffer(vbIdx);

        model.arrMesh.back()->pIndexBuffer->Lock(BL_WRITE_ONLY);
        model.arrMesh.back()->pVertexBuffer->Lock(BL_WRITE_ONLY);

        unsigned int iterIndices = 0, iterVertices = 0;

        // Populate IB
        for (unsigned int faceIdx = 0; faceIdx < scene->mMeshes[meshIdx]->mNumFaces; faceIdx++)
        {
            //assert(scene->mMeshes[meshIdx]->mFaces[faceIdx].mNumIndices == 3);
            if (scene->mMeshes[meshIdx]->mFaces[faceIdx].mNumIndices != 3)
            {
                Log << "\t[WARNING] Mesh " << meshIdx << "(name: \"" << scene->mMeshes[meshIdx]->mName.C_Str() <<
                    "\") contains a face (" << faceIdx << ") with " << scene->mMeshes[meshIdx]->mFaces[faceIdx].mNumIndices << " indices\n";
                skippedIndices += 3; // we would have expected 3 indices here, but we don't have exactly that many
                continue;
            }

            for (unsigned int vertIdx = 0; vertIdx < 3; vertIdx++)
                model.arrMesh.back()->pIndexBuffer->SetIndex(iterIndices++, scene->mMeshes[meshIdx]->mFaces[faceIdx].mIndices[vertIdx]);
        }

        // Populate VB
        for (unsigned int vertIdx = 0; vertIdx < scene->mMeshes[meshIdx]->mNumVertices; vertIdx++)
        {
            if (model.arrMesh.back()->pVertexBuffer->HasPosition())
                model.arrMesh.back()->pVertexBuffer->Position<Vec3f>(iterVertices) = Vec3f(
                    scene->mMeshes[meshIdx]->mVertices[vertIdx].x,
                    scene->mMeshes[meshIdx]->mVertices[vertIdx].y,
                    scene->mMeshes[meshIdx]->mVertices[vertIdx].z);

            if (model.arrMesh.back()->pVertexBuffer->HasNormal())
                model.arrMesh.back()->pVertexBuffer->Normal<Vec3f>(iterVertices) = Vec3f(
                    scene->mMeshes[meshIdx]->mNormals[vertIdx].x,
                    scene->mMeshes[meshIdx]->mNormals[vertIdx].y,
                    scene->mMeshes[meshIdx]->mNormals[vertIdx].z);

            if (model.arrMesh.back()->pVertexBuffer->HasTangent())
                model.arrMesh.back()->pVertexBuffer->Tangent<Vec3f>(iterVertices) = Vec3f(
                    scene->mMeshes[meshIdx]->mTangents[vertIdx].x,
                    scene->mMeshes[meshIdx]->mTangents[vertIdx].y,
                    scene->mMeshes[meshIdx]->mTangents[vertIdx].z);

            if (model.arrMesh.back()->pVertexBuffer->HasBinormal())
                model.arrMesh.back()->pVertexBuffer->Binormal<Vec3f>(iterVertices) = Vec3f(
                    scene->mMeshes[meshIdx]->mBitangents[vertIdx].x,
                    scene->mMeshes[meshIdx]->mBitangents[vertIdx].y,
                    scene->mMeshes[meshIdx]->mBitangents[vertIdx].z);

            for (unsigned int tcIdx = 0; tcIdx < AI_MAX_NUMBER_OF_TEXTURECOORDS; tcIdx++)
                if (model.arrMesh.back()->pVertexBuffer->HasTexCoord(tcIdx) && scene->mMeshes[meshIdx]->HasTextureCoords(tcIdx))
                    switch (scene->mMeshes[meshIdx]->mNumUVComponents[tcIdx])
                    {
                    case 1:
                        model.arrMesh.back()->pVertexBuffer->TexCoord<float>(iterVertices, tcIdx) = scene->mMeshes[meshIdx]->mTextureCoords[tcIdx][vertIdx].x;
                        break;
                    case 2:
                        model.arrMesh.back()->pVertexBuffer->TexCoord<Vec2f>(iterVertices, tcIdx) = Vec2f(
                            scene->mMeshes[meshIdx]->mTextureCoords[tcIdx][vertIdx].x,
                            scene->mMeshes[meshIdx]->mTextureCoords[tcIdx][vertIdx].y);
                        break;
                    case 3:
                        model.arrMesh.back()->pVertexBuffer->TexCoord<Vec3f>(iterVertices, tcIdx) = Vec3f(
                            scene->mMeshes[meshIdx]->mTextureCoords[tcIdx][vertIdx].x,
                            scene->mMeshes[meshIdx]->mTextureCoords[tcIdx][vertIdx].y,
                            scene->mMeshes[meshIdx]->mTextureCoords[tcIdx][vertIdx].z);
                        break;
                    default:
                        assert(false);
                    }

            for (unsigned int colorIdx = 0; colorIdx < AI_MAX_NUMBER_OF_COLOR_SETS; colorIdx++)
                if (model.arrMesh.back()->pVertexBuffer->HasColor(colorIdx) && scene->mMeshes[meshIdx]->HasVertexColors(colorIdx))
                    model.arrMesh.back()->pVertexBuffer->Color<DWORD>(iterVertices, colorIdx) =
                    ((((DWORD)(scene->mMeshes[meshIdx]->mColors[colorIdx][vertIdx].a * 255.f)) & 0xff) << 24) |
                    ((((DWORD)(scene->mMeshes[meshIdx]->mColors[colorIdx][vertIdx].r * 255.f)) & 0xff) << 16) |
                    ((((DWORD)(scene->mMeshes[meshIdx]->mColors[colorIdx][vertIdx].g * 255.f)) & 0xff) << 8) |
                    ((((DWORD)(scene->mMeshes[meshIdx]->mColors[colorIdx][vertIdx].b * 255.f)) & 0xff));

            iterVertices++;
        }
        assert(iterIndices + skippedIndices == totalIndexCount && iterVertices == meshVertexCount);

        model.arrMesh.back()->pIndexBuffer->Update();
        model.arrMesh.back()->pIndexBuffer->Unlock();
        model.arrMesh.back()->pVertexBuffer->Update();
        model.arrMesh.back()->pVertexBuffer->Unlock();

        if (scene->HasMaterials())
            model.arrMesh.back()->nMaterialIdx = scene->mMeshes[meshIdx]->mMaterialIndex;

        Log << "\tMaterial index: " << model.arrMesh.back()->nMaterialIdx << "\n";

        Log << "[/MESH]" << "\n";
    }

    Log << "\nMaterial count: " << scene->mNumMaterials << "\n";

    for (unsigned int matIdx = 0; matIdx < scene->mNumMaterials; matIdx++)
    {
        Log << "[MATERIAL]" << "\n";

        model.arrMaterial.push_back(new Model::Material);

        aiMaterial* mat = scene->mMaterials[matIdx];

        aiString szMatName("");
        mat->Get(AI_MATKEY_NAME, szMatName);
        model.arrMaterial.back()->szName = szMatName.C_Str();
        Log << "\tName: " << szMatName.C_Str() << "\n";
        Log << "\tMaterial index: " << matIdx << "\n";

        int nTwoSided = 0;
        if (mat->Get(AI_MATKEY_TWOSIDED, nTwoSided) == AI_SUCCESS)
            Log << "\tTwo sided: " << nTwoSided << "\n";
        model.arrMaterial.back()->bTwoSided = nTwoSided != 0;

        aiShadingMode eShadingModel = aiShadingMode_Gouraud;
        if (mat->Get(AI_MATKEY_SHADING_MODEL, eShadingModel) == AI_SUCCESS)
            Log << "\tShading model: " << GetEnumString((Model::Material::ShadingModel)eShadingModel) << "\n";
        model.arrMaterial.back()->eShadingModel = (Model::Material::ShadingModel)eShadingModel;

        int bEnableWireframe = 0;
        if (mat->Get(AI_MATKEY_ENABLE_WIREFRAME, bEnableWireframe) == AI_SUCCESS)
            Log << "\tEnable wireframe: " << bEnableWireframe << "\n";
        model.arrMaterial.back()->bEnableWireframe = bEnableWireframe != 0;

        aiBlendMode eBlendMode = aiBlendMode_Default;
        if (mat->Get(AI_MATKEY_BLEND_FUNC, eBlendMode) == AI_SUCCESS)
            Log << "\tBlend mode: " << GetEnumString((Model::Material::BlendMode)eBlendMode) << "\n";
        model.arrMaterial.back()->eBlendMode = (Model::Material::BlendMode)eBlendMode;

        float fOpacity = 1.f;
        if (mat->Get(AI_MATKEY_OPACITY, fOpacity) == AI_SUCCESS)
            Log << "\tOpacity: " << fOpacity << "\n";
        model.arrMaterial.back()->fOpacity = fOpacity;

        float fBumpScaling = 1.f;
        if (mat->Get(AI_MATKEY_BUMPSCALING, fBumpScaling) == AI_SUCCESS)
            Log << "\tBump scaling: " << fBumpScaling << "\n";
        model.arrMaterial.back()->fBumpScaling = fBumpScaling;

        float fShininess = 0.f;
        if (mat->Get(AI_MATKEY_SHININESS, fShininess) == AI_SUCCESS)
            Log << "\tShininess: " << fShininess << "\n";
        model.arrMaterial.back()->fShininess = fShininess;

        float fReflectivity = 1.f;
        if (mat->Get(AI_MATKEY_REFLECTIVITY, fReflectivity) == AI_SUCCESS)
            Log << "\tRefectivity: " << fReflectivity << "\n";
        model.arrMaterial.back()->fReflectivity = fReflectivity;

        float fShininessStrength = 1.f;
        if (mat->Get(AI_MATKEY_SHININESS_STRENGTH, fShininessStrength) == AI_SUCCESS)
            Log << "\tShininess strength: " << fShininessStrength << "\n";
        model.arrMaterial.back()->fShininessStrength = fShininessStrength;

        float fRefractIndex = 1.f;
        if (mat->Get(AI_MATKEY_REFRACTI, fRefractIndex) == AI_SUCCESS)
            Log << "\tRefraction index: " << fRefractIndex << "\n";
        model.arrMaterial.back()->fRefractIndex = fRefractIndex;

        aiColor3D vColorDiffuse(0.f, 0.f, 0.f);
        if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, vColorDiffuse) == AI_SUCCESS)
            Log << "\tColor diffuse: " << "RGB(" << vColorDiffuse.r << ", " << vColorDiffuse.g << ", " << vColorDiffuse.b << ")" << "\n";
        model.arrMaterial.back()->vColorDiffuse = Vec3f(vColorDiffuse.r, vColorDiffuse.g, vColorDiffuse.b);

        aiColor3D vColorAmbient(0.f, 0.f, 0.f);
        if (mat->Get(AI_MATKEY_COLOR_AMBIENT, vColorAmbient) == AI_SUCCESS)
            Log << "\tColor ambient: " << "RGB(" << vColorAmbient.r << ", " << vColorAmbient.g << ", " << vColorAmbient.b << ")" << "\n";
        model.arrMaterial.back()->vColorAmbient = Vec3f(vColorAmbient.r, vColorAmbient.g, vColorAmbient.b);

        aiColor3D vColorSpecular(0.f, 0.f, 0.f);
        if (mat->Get(AI_MATKEY_COLOR_SPECULAR, vColorSpecular) == AI_SUCCESS)
            Log << "\tColor specular: " << "RGB(" << vColorSpecular.r << ", " << vColorSpecular.g << ", " << vColorSpecular.b << ")" << "\n";
        model.arrMaterial.back()->vColorSpecular = Vec3f(vColorSpecular.r, vColorSpecular.g, vColorSpecular.b);

        aiColor3D vColorEmissive(0.f, 0.f, 0.f);
        if (mat->Get(AI_MATKEY_COLOR_EMISSIVE, vColorEmissive) == AI_SUCCESS)
            Log << "\tColor emissive: " << "RGB(" << vColorEmissive.r << ", " << vColorEmissive.g << ", " << vColorEmissive.b << ")" << "\n";
        model.arrMaterial.back()->vColorEmissive = Vec3f(vColorEmissive.r, vColorEmissive.g, vColorEmissive.b);

        aiColor3D vColorTransparent(0.f, 0.f, 0.f);
        if (mat->Get(AI_MATKEY_COLOR_TRANSPARENT, vColorTransparent) == AI_SUCCESS)
            Log << "\tColor transparent: " << "RGB(" << vColorTransparent.r << ", " << vColorTransparent.g << ", " << vColorTransparent.b << ")" << "\n";
        model.arrMaterial.back()->vColorTransparent = Vec3f(vColorTransparent.r, vColorTransparent.g, vColorTransparent.b);

        aiColor3D vColorReflective(0.f, 0.f, 0.f);
        if (mat->Get(AI_MATKEY_COLOR_REFLECTIVE, vColorReflective) == AI_SUCCESS)
            Log << "\tColor reflective: " << "RGB(" << vColorReflective.r << ", " << vColorReflective.g << ", " << vColorReflective.b << ")" << "\n";
        model.arrMaterial.back()->vColorReflective = Vec3f(vColorReflective.r, vColorReflective.g, vColorReflective.b);

        for (unsigned int type = aiTextureType_NONE; type <= aiTextureType_UNKNOWN; type++)
        {
            aiString szTexturePath("");
            unsigned int nTexIdx = 0;

            while (mat->Get(AI_MATKEY_TEXTURE(type, nTexIdx), szTexturePath) == AI_SUCCESS)
            {
                modelTexRefCount++;
                Log << "\t[TEXTURE]" << "\n";

                model.arrMaterial.back()->arrTexture.push_back(new Model::TextureDesc());

                model.arrMaterial.back()->arrTexture.back()->szFilePath = szTexturePath.C_Str();
                Log << "\t\tFile: " << szTexturePath.C_Str() << "\n";

                model.arrMaterial.back()->arrTexture.back()->eTexType = (Model::TextureDesc::TextureType)type;
                model.arrMaterial.back()->arrTexture.back()->nTexIndex = nTexIdx;
                Log << "\t\tType: " << GetEnumString((Model::TextureDesc::TextureType)type) << "\n";

                int nUVChannel = -1;
                if (mat->Get(AI_MATKEY_UVWSRC(type, nTexIdx), nUVChannel) == AI_SUCCESS)
                    Log << "\t\tUV channel: " << nUVChannel << "\n";
                model.arrMaterial.back()->arrTexture.back()->nUVChannel = nUVChannel;

                aiTextureOp eTexOp = aiTextureOp_Add;
                if (mat->Get(AI_MATKEY_TEXOP(type, nTexIdx), eTexOp) == AI_SUCCESS)
                {
                    Log << "\t\tOp index: " << nTexIdx << "\n";
                    Log << "\t\tOp: " << GetEnumString((Model::TextureDesc::TextureOp)eTexOp) << "\n";
                }
                model.arrMaterial.back()->arrTexture.back()->eTexOp = (Model::TextureDesc::TextureOp)eTexOp;

                float fTexBlend = 1.f;
                if (mat->Get(AI_MATKEY_TEXBLEND(type, nTexIdx), fTexBlend) == AI_SUCCESS)
                    Log << "\t\tBlend: " << fTexBlend << "\n";
                model.arrMaterial.back()->arrTexture.back()->fTexBlend = fTexBlend;

                aiTextureMapping eTexMapping = aiTextureMapping_OTHER;
                if (mat->Get(AI_MATKEY_MAPPING(type, nTexIdx), eTexMapping) == AI_SUCCESS)
                    Log << "\t\tMapping: " << GetEnumString((Model::TextureDesc::TextureMapping)eTexMapping) << "\n";
                model.arrMaterial.back()->arrTexture.back()->eTexMapping = (Model::TextureDesc::TextureMapping)eTexMapping;

                aiTextureMapMode eTexMapModeU = aiTextureMapMode_Wrap;
                aiTextureMapMode eTexMapModeV = aiTextureMapMode_Wrap;
                if (mat->Get(AI_MATKEY_MAPPINGMODE_U(type, nTexIdx), eTexMapModeU) == AI_SUCCESS)
                    Log << "\t\tMapping mode U: " << GetEnumString((Model::TextureDesc::TextureMappingMode)eTexMapModeU) << "\n";
                if (mat->Get(AI_MATKEY_MAPPINGMODE_V(type, nTexIdx), eTexMapModeV) == AI_SUCCESS)
                    Log << "\t\tMapping mode V: " << GetEnumString((Model::TextureDesc::TextureMappingMode)eTexMapModeV) << "\n";
                model.arrMaterial.back()->arrTexture.back()->eTexMapModeU = (Model::TextureDesc::TextureMappingMode)eTexMapModeU;
                model.arrMaterial.back()->arrTexture.back()->eTexMapModeV = (Model::TextureDesc::TextureMappingMode)eTexMapModeV;

                nTexIdx++;

                Log << "\t[/TEXTURE]" << "\n";
            }
        }

        Log << "[/MATERIAL]" << "\n";
    }

    Log << "\nCompilation of \"" << argv[argc - 1] << "\" finished in " << (float)(GetTickCount64() - startTick) / 1000.f << " seconds\n";
    Log << "Total vertex count: " << modelVertexCount << " vertices\n";
    Log << "Total mesh count: " << scene->mNumMeshes << " meshes\n";
    Log << "Total material count: " << scene->mNumMaterials << " materials\n";
    Log << "Total texture reference count: " << modelTexRefCount << " textures\n";

    string outFilePath = outputDirPath;
    outFilePath = ReplaceAll(outFilePath, "/", "\\");
    if (strlen(outputDirPath))
        system(("if not exist " + outFilePath + " mkdir " + outFilePath).c_str()); // crude, but effective for recursive directory creation
    else
    {
        CreateDirectoryA("Out", NULL);
        outFilePath = "Out\\Models\\";
        CreateDirectoryA(outFilePath.c_str(), NULL);
        outFilePath += fileName;
        CreateDirectoryA(outFilePath.c_str(), NULL);
    }
    outFilePath += "\\";
    outFilePath += fileName;
    outFilePath += ".s3dmdl";

    ostringstream rawModelBuffer;
    rawModelBuffer << model;

    const unsigned int uncompressedSize = (unsigned int)rawModelBuffer.str().size();
    const unsigned int compressedSizeMax = (unsigned int)LZ4_compressBound(uncompressedSize);
    char* const compressedBuffer = new char[compressedSizeMax];
    const int compressedSize = LZ4_compress_HC(rawModelBuffer.str().c_str(), compressedBuffer, (int)rawModelBuffer.str().size(), compressedSizeMax, LZ4HC_CLEVEL_DEFAULT);
    //const int compressedSize = LZ4_compress_default(rawModelBuffer.str().c_str(), compressedBuffer, (int)rawModelBuffer.str().size(), compressedSizeMax);

    ofstream outModel;
    outModel.open(outFilePath.c_str(), ofstream::trunc | ofstream::binary);
#ifdef _DEBUG
    outModel.setf(ios_base::unitbuf);
#endif
    outModel.write(S3D_MODEL_FILE_HEADER, S3D_MODEL_FILE_HEADER_SIZE);
    const unsigned int fileVersion = S3D_MODEL_FILE_VERSION;
    outModel.write((char*)&fileVersion, sizeof(unsigned int));
    outModel.write((char*)&compressedSize, sizeof(unsigned int));
    outModel.write((char*)&uncompressedSize, sizeof(unsigned int));
    outModel.write(compressedBuffer, compressedSize);
    outModel.close();

    delete[] compressedBuffer;

#ifdef _DEBUG
    unsigned int modelIdx = resMan->CreateModel(outFilePath.c_str());
    Model* modelIn = resMan->GetModel(modelIdx);

    assert(model.szName == modelIn->szName);
    assert(model.arrMaterial.size() == modelIn->arrMaterial.size());
    assert(model.arrMesh.size() == modelIn->arrMesh.size());
#endif

    Renderer::DestroyInstance();
}

const char* ModelCompiler::GetEnumString(Model::Material::ShadingModel val)
{
    switch (val)
    {
    case Model::Material::SM_FLAT:
        return "SM_FLAT";
    case Model::Material::SM_GOURAUD:
        return "SM_GOURAUD";
    case Model::Material::SM_PHONG:
        return "SM_PHONG";
    case Model::Material::SM_BLINN:
        return "SM_BLINN";
    case Model::Material::SM_TOON:
        return "SM_TOON";
    case Model::Material::SM_ORENNAYAR:
        return "SM_ORENNAYAR";
    case Model::Material::SM_MINNAERT:
        return "SM_MINNAERT";
    case Model::Material::SM_COOKTORRANCE:
        return "SM_COOKTORRANCE";
    case Model::Material::SM_NOSHADING:
        return "SM_NOSHADING";
    case Model::Material::SM_FRESNEL:
        return "SM_FRESNEL";
    default:
        return "";
    }
}

const char* ModelCompiler::GetEnumString(Model::Material::BlendMode val)
{
    switch (val)
    {
    case Model::Material::BM_DEFAULT:
        return "BM_DEFAULT";
    case Model::Material::BM_ADDITIVE:
        return "BM_ADDITIVE";
    default:
        return "";
    }
}

const char* ModelCompiler::GetEnumString(Model::TextureDesc::TextureType val)
{
    switch (val)
    {
    case Model::TextureDesc::TT_NONE:
        return "TT_NONE";
    case Model::TextureDesc::TT_DIFFUSE:
        return "TT_DIFFUSE";
    case Model::TextureDesc::TT_SPECULAR:
        return "TT_SPECULAR";
    case Model::TextureDesc::TT_AMBIENT:
        return "TT_AMBIENT";
    case Model::TextureDesc::TT_EMISSIVE:
        return "TT_EMISSIVE";
    case Model::TextureDesc::TT_HEIGHT:
        return "TT_HEIGHT";
    case Model::TextureDesc::TT_NORMALS:
        return "TT_NORMALS";
    case Model::TextureDesc::TT_SHININESS:
        return "TT_SHININESS";
    case Model::TextureDesc::TT_OPACITY:
        return "TT_OPACITY";
    case Model::TextureDesc::TT_DISPLACEMENT:
        return "TT_DISPLACEMENT";
    case Model::TextureDesc::TT_LIGHTMAP:
        return "TT_LIGHTMAP";
    case Model::TextureDesc::TT_REFLECTION:
        return "TT_REFLECTION";
    case Model::TextureDesc::TT_UNKNOWN:
        return "TT_UNKNOWN";
    default:
        return "";
    }
}

const char* ModelCompiler::GetEnumString(Model::TextureDesc::TextureOp val)
{
    switch (val)
    {
    case Model::TextureDesc::TO_MULTIPLY:
        return "TO_MULTIPLY";
    case Model::TextureDesc::TO_ADD:
        return "TO_ADD";
    case Model::TextureDesc::TO_SUBTRACT:
        return "TO_SUBTRACT";
    case Model::TextureDesc::TO_DIVIDE:
        return "TO_DIVIDE";
    case Model::TextureDesc::TO_SMOOTHADD:
        return "TO_SMOOTHADD";
    case Model::TextureDesc::TO_SIGNEDADD:
        return "TO_SIGNEDADD";
    default:
        return "";
    }
}

const char* ModelCompiler::GetEnumString(Model::TextureDesc::TextureMapping val)
{
    switch (val)
    {
    case Model::TextureDesc::TM_UV:
        return "TM_UV";
    case Model::TextureDesc::TM_SPHERE:
        return "TM_SPHERE";
    case Model::TextureDesc::TM_CYLINDER:
        return "TM_CYLINDER";
    case Model::TextureDesc::TM_BOX:
        return "TM_BOX";
    case Model::TextureDesc::TM_PLANE:
        return "TM_PLANE";
    case Model::TextureDesc::TM_OTHER:
        return "TM_OTHER";
    default:
        return "";
    }
}

const char* ModelCompiler::GetEnumString(Model::TextureDesc::TextureMappingMode val)
{
    switch (val)
    {
    case Model::TextureDesc::TMM_WRAP:
        return "TMM_WRAP";
    case Model::TextureDesc::TMM_CLAMP:
        return "TMM_CLAMP";
    case Model::TextureDesc::TMM_DECAL:
        return "TMM_DECAL";
    case Model::TextureDesc::TMM_MIRROR:
        return "TMM_MIRROR";
    default:
        return "";
    }
}

int main(int argc, char* argv[])
{
    ModelCompiler mc;
    mc.Run(argc, argv);

    return 0;
}
