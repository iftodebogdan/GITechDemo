import os
import subprocess
import time
import sys
from collections import defaultdict

#############################################################################
#		Arguments:															#
#---------------------------------------------------------------------------#
#	'win32' to use the 32 bit version of the tools							#
#	'x64' to use the 64 bit version of the tools (default)					#
#	'rebuild' to force rebuilding all data assets							#
#############################################################################

defaultArchitecture = "x64"
defaultForceRebuild = False

# Process command arguments
for opt in sys.argv:
	if(opt.lower() == "x64"):
		defaultArchitecture = "x64"
	if(opt.lower() == "win32"):
		defaultArchitecture = "Win32"
	if(opt.lower() == "rebuild"):
		defaultForceRebuild = True

# Set paths for model compilation
rootModelDir = "models/sponza/"
modelFiles = ["sponza.obj"]
modelOutputPath = "../Data/models/sponza/"
modelCompilerExe = "../Bin/" + defaultArchitecture + "/Release/LibRendererTools/ModelCompiler.exe"

# Set paths for texture compilation
pathToTextureFiles = "models/sponza/textures/"
textureOutputPath = "../Data/models/sponza/textures/"
textureCompilerExe = "../Bin/" + defaultArchitecture + "/Release/LibRendererTools/TextureCompiler.exe"

# Set custom arguments for individual texture files
customArgs = defaultdict(lambda: "-q -f A8R8G8B8", \
{
	(file, "-q -f L8") for root, dir, files in os.walk(pathToTextureFiles) for file in files if file.lower().find("_metallic.") != -1 or file.lower().find("_roughness.") != -1
})

# Detect modification of model compiler executable
forceRebuildModels = defaultForceRebuild
if os.path.getmtime(modelCompilerExe) > os.path.getmtime(os.path.realpath(__file__)):
	print "Model compiler (\"" + modelCompilerExe + "\") modification detected. Forcing rebuild of all model assets"
	forceRebuildModels = True

# Detect modification of texture compiler executable
forceRebuildTextures = defaultForceRebuild
if os.path.getmtime(textureCompilerExe) > os.path.getmtime(os.path.realpath(__file__)):
	print "Texture compiler (\"" + textureCompilerExe + "\") modification detected. Forcing rebuild of all texture assets"
	forceRebuildTextures = True

start = time.clock()

#############
#	Models	#
#############

# Compile models
for file in modelFiles:
	sourceFileIsNewer = os.path.getmtime(rootModelDir + file) > os.path.getmtime(os.path.realpath(__file__))
	compiledFileExists = os.path.isfile(modelOutputPath + os.path.splitext(file)[0] + ".lrm")
	if sourceFileIsNewer or not compiledFileExists or forceRebuildModels:
		print "Compiling model \"" + rootModelDir + file + "\""
		subprocess.call([modelCompilerExe, "-q", "-d", modelOutputPath, rootModelDir + file])
	else:
		print "Model \"" + rootModelDir + file + "\" is up-to-date"

##########################################

#################
#	Textures	#
#################

# Compile textures
for root, dir, files in os.walk(pathToTextureFiles):
	for name in files:
		if name != 'Thumbs.db':
			sourceFileIsNewer = os.path.getmtime(os.path.join(root, name)) > os.path.getmtime(os.path.realpath(__file__))
			compiledFileExists = os.path.isfile(textureOutputPath + os.path.splitext(name)[0] + ".lrt")
			if sourceFileIsNewer or not compiledFileExists or forceRebuildTextures:
				print "Compiling texture \"" + os.path.join(root, name) + "\""
				subprocess.call(textureCompilerExe + " " + customArgs[name] + " -d " + textureOutputPath + " " + os.path.join(root, name))
			else:
				print "Texture \"" + os.path.join(root, name) + "\" is up-to-date"

##########################################

print "Done in " + str(time.clock() - start) + " seconds.";

os.utime(os.path.realpath(__file__), None)
