import os
import subprocess
import time

#pathToTextureFiles = "sponza_scene/textures/"
pathToTextureFiles = "crytek-sponza/textures/"
outputPath = "sponza/textures/"
textureCompilerExe = "../Bin/x64/Release/TextureCompiler/TextureCompiler.exe"

start = time.clock()

for root, dir, files in os.walk(pathToTextureFiles):
   for name in files:
      if name != 'Thumbs.db':
         print "Compiling texture \"" + os.path.join(root, name) + "\""
         subprocess.call([textureCompilerExe, "-q", "-f", "A8R8G8B8", "-d", outputPath, os.path.join(root, name)])

print "Done in " + str(time.clock() - start) + " seconds";
