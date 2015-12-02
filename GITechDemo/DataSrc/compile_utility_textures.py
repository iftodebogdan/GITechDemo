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

# Set paths
pathToTextureFiles = "textures/"
outputPath = "../Data/textures/"
textureCompilerExe = "../Bin/" + defaultArchitecture + "/Release/LibRendererTools/TextureCompiler.exe"

# Set custom arguments for individual files
customArgs = defaultdict(lambda: "-q -f A8R8G8B8", \
{
	"sky.dds"					: "-q -f A16B16G16R16F",
	"LensFlareDirt.png"			: "-q -f A8R8G8B8 -mip 1",
	"LensFlareStarBurst.png"	: "-q -f A8R8G8B8 -mip 1",
	"bayer_matrix.dds"			: "-q -f L8 -mip 1",
	"noise.dds"					: "-q -f L8 -mip 1"
})

# Detect modification of texture compiler executable
if os.path.getmtime(textureCompilerExe) > os.path.getmtime(os.path.realpath(__file__)):
	print "Texture compiler (\"" + textureCompilerExe + "\") modification detected. Forcing rebuild of all texture assets"
	defaultForceRebuild = True

start = time.clock()

# Compile textures
for root, dir, files in os.walk(pathToTextureFiles):
	for name in files:
		if name != 'Thumbs.db':
			sourceFileIsNewer = os.path.getmtime(os.path.join(root, name)) > os.path.getmtime(os.path.realpath(__file__))
			compiledFileExists = os.path.isfile(outputPath + os.path.splitext(name)[0] + ".lrt")
			if sourceFileIsNewer or not compiledFileExists or defaultForceRebuild:
				print "Compiling texture \"" + os.path.join(root, name) + "\""
				subprocess.call(textureCompilerExe + " " + customArgs[name] + " -d " + outputPath + " " + os.path.join(root, name))
			else:
				print "Texture \"" + os.path.join(root, name) + "\" is up-to-date"

print "Done in " + str(time.clock() - start) + " seconds.";

os.utime(os.path.realpath(__file__), None)
