#ifndef TEXTURECOMPILER_H
#define TEXTURECOMPILER_H

struct ILinfo;

namespace LibRendererDll
{
	enum PixelFormat;
}

namespace LibRendererTools
{
	class mstream;

	class TextureCompiler
	{
		static const char* GetEnumString(PixelFormat val);
		static const char* GetEnumString(TexType val);
		static bool HandleDevilErrors(mstream& logStream);
		static LibRendererDll::PixelFormat GetPixelFormat(const ILinfo& info, bool& swizzle);
	public:
		void Run(int argc, char* argv[]);
	};
}

#endif // TEXTURECOMPILER_H