#ifndef MODELCOMPILER_H
#define MODELCOMPILER_H

#include <ResourceData.h>
using namespace LibRendererDll;

namespace LibRendererTools
{
	class ModelCompiler
	{
		static const char* GetEnumString(Model::Material::ShadingModel val);
		static const char* GetEnumString(Model::Material::BlendMode val);
		static const char* GetEnumString(Model::TextureDesc::TextureType val);
		static const char* GetEnumString(Model::TextureDesc::TextureOp val);
		static const char* GetEnumString(Model::TextureDesc::TextureMapping val);
		static const char* GetEnumString(Model::TextureDesc::TextureMappingMode val);

	public:
		void Run(int argc, char* argv[]);
	};
}

#endif // MODELCOMPILER_H