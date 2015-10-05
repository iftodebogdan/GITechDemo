import os
import subprocess
import time
import sys

#############################################################################
#		Arguments:															#
#---------------------------------------------------------------------------#
#	'win32' to use the 32 bit version of the tools							#
#	'x64' to use the 64 bit version of the tools (default)					#
#############################################################################

defaultArchitecture = "x64"

# Process command arguments
for opt in sys.argv:
	if(opt.lower() == "x64"):
		defaultArchitecture = "x64"
	if(opt.lower() == "win32"):
		defaultArchitecture = "Win32"

pathToTextureFiles = "textures/"
outputPath = "../Data/textures/"
textureCompilerExe = "../Bin/" + defaultArchitecture + "/Release/LibRendererTools/TextureCompiler.exe"

start = time.clock()

for root, dir, files in os.walk(pathToTextureFiles):
	for name in files:
		if name != 'Thumbs.db':
			print "Compiling texture \"" + os.path.join(root, name) + "\""
			if name == 'sky.dds':
				subprocess.call([textureCompilerExe, "-q", "-f", "A16B16G16R16F", "-d", outputPath, os.path.join(root, name)])
			else:
				subprocess.call([textureCompilerExe, "-q", "-f", "A8R8G8B8", "-d", outputPath, os.path.join(root, name)])

print "Done in " + str(time.clock() - start) + " seconds.";
