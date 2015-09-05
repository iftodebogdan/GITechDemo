import os
import subprocess
import time

#############
#	Model	#
#############

pathToModelFile = "models/sponza/"
modelFile = "sponza.obj"
outputPath = "../Data/models/sponza/"
modelCompilerExe = "../Bin/x64/Release/LibRendererTools/ModelCompiler.exe"

start = time.clock()

print "Compiling model \"" + pathToModelFile + modelFile + "\""
subprocess.call([modelCompilerExe, "-q", "-d", outputPath, pathToModelFile + modelFile])

##########################################

#################
#	Textures	#
#################

pathToTextureFiles = "models/sponza/textures/"
outputPath = "../Data/models/sponza/textures/"
textureCompilerExe = "../Bin/x64/Release/LibRendererTools/TextureCompiler.exe"

for root, dir, files in os.walk(pathToTextureFiles):
	for name in files:
		if name != 'Thumbs.db':
			print "Compiling texture \"" + os.path.join(root, name) + "\""
			if name.lower().find("_metallic.") != -1 or name.lower().find("_roughness.") != -1:
				subprocess.call([textureCompilerExe, "-q", "-f", "L8", "-d", outputPath, os.path.join(root, name)])
			else:
				subprocess.call([textureCompilerExe, "-q", "-f", "A8R8G8B8", "-d", outputPath, os.path.join(root, name)])

print "Done in " + str(time.clock() - start) + " seconds";

##########################################
