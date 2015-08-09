import os
import subprocess
import time

pathToTextureFiles = "textures/"
outputPath = "../Data/textures/"
textureCompilerExe = "../Bin/x64/Release/LibRendererTools/TextureCompiler.exe"

start = time.clock()

for root, dir, files in os.walk(pathToTextureFiles):
	for name in files:
		if name != 'Thumbs.db':
			print "Compiling texture \"" + os.path.join(root, name) + "\""
			if name == 'sky.dds':
				subprocess.call([textureCompilerExe, "-q", "-f", "A16B16G16R16F", "-d", outputPath, os.path.join(root, name)])
			else:
				subprocess.call([textureCompilerExe, "-q", "-f", "A8R8G8B8", "-d", outputPath, os.path.join(root, name)])

print "Done in " + str(time.clock() - start) + " seconds";
