#ifndef TEXTURECOMPILER_H
#define TEXTURECOMPILER_H

struct ILinfo;

namespace Synesthesia3D
{
    enum PixelFormat;
}

namespace Synesthesia3DTools
{
    class mstream;

    class TextureCompiler
    {
        static bool HandleDevilErrors(mstream& logStream);
        static Synesthesia3D::PixelFormat GetPixelFormat(const ILinfo& info, bool& swizzle);
    public:
        void Run(int argc, char* argv[]);
    };
}

#endif // TEXTURECOMPILER_H