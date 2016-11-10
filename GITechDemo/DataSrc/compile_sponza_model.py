#=============================================================================
# This file is part of the "GITechDemo" application
# Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
#
#       File:   compile_sponza_model.py
#       Author: Bogdan Iftode
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#=============================================================================

import os
import subprocess
import time
import sys
from collections import defaultdict

#############################################################################
#       Arguments:                                                          #
#---------------------------------------------------------------------------#
#   'win32' to use the 32 bit version of the tools                          #
#   'x64' to use the 64 bit version of the tools (default)                  #
#   'rebuild' to force rebuilding all data assets                           #
#############################################################################

defaultArchitecture = "x64"
defaultForceRebuild = False

# Retrieve absolute path of this script
scriptAbsPath = os.path.abspath(os.path.dirname(os.path.realpath(__file__)));

# Process command arguments
for opt in sys.argv:
    if(opt.lower() == "x64"):
        defaultArchitecture = "x64"
    if(opt.lower() == "win32"):
        defaultArchitecture = "Win32"
    if(opt.lower() == "rebuild"):
        defaultForceRebuild = True

# Set paths for model compilation
rootModelDir = scriptAbsPath + "/models/sponza/"
modelFiles = ["sponza.obj"]
modelOutputPath = scriptAbsPath + "/../Data/models/sponza/"
modelCompilerExe = scriptAbsPath + "/../Bin/" + defaultArchitecture + "/Release/Synesthesia3DTools/ModelCompiler.exe"

# Set paths for texture compilation
pathToTextureFiles = scriptAbsPath + "/models/sponza/textures/"
textureOutputPath = scriptAbsPath + "/../Data/models/sponza/textures/"
textureCompilerExe = scriptAbsPath + "/../Bin/" + defaultArchitecture + "/Release/Synesthesia3DTools/TextureCompiler.exe"

# Set custom arguments for individual texture files
customArgs = defaultdict(lambda: "-q -f A8R8G8B8", \
{
    (file, "-q -f L8") for root, dir, files in os.walk(pathToTextureFiles) for file in files if file.lower().find("_metallic.") != -1 or file.lower().find("_roughness.") != -1
})

# Detect modification of model compiler executable
forceRebuildModels = defaultForceRebuild
if os.path.getmtime(modelCompilerExe) > os.path.getmtime(os.path.realpath(__file__)):
    print "Model compiler (\"" + modelCompilerExe.replace(scriptAbsPath + "/", "") + "\") modification detected. Forcing rebuild of all model assets"
    forceRebuildModels = True

# Detect modification of texture compiler executable
forceRebuildTextures = defaultForceRebuild
if os.path.getmtime(textureCompilerExe) > os.path.getmtime(os.path.realpath(__file__)):
    print "Texture compiler (\"" + textureCompilerExe.replace(scriptAbsPath + "/", "") + "\") modification detected. Forcing rebuild of all texture assets"
    forceRebuildTextures = True

start = time.clock()

#############
#   Models  #
#############

# Compile models
for file in modelFiles:
    sourceFileIsNewer = os.path.getmtime(rootModelDir + file) > os.path.getmtime(os.path.realpath(__file__))
    compiledFileExists = os.path.isfile(modelOutputPath + os.path.splitext(file)[0] + ".s3dmdl")
    if sourceFileIsNewer or not compiledFileExists or forceRebuildModels:
        print "Compiling model \"" + rootModelDir.replace(scriptAbsPath + "/", "") + file + "\""
        subprocess.call([modelCompilerExe, "-q", "-d", modelOutputPath, "-log", scriptAbsPath + "/Logs", rootModelDir + file])
    else:
        print "Model \"" + rootModelDir.replace(scriptAbsPath + "/", "") + file + "\" is up-to-date"

##########################################

#################
#   Textures    #
#################

# Compile textures
for root, dir, files in os.walk(pathToTextureFiles):
    for name in files:
        if name != 'Thumbs.db':
            sourceFileIsNewer = os.path.getmtime(os.path.join(root, name)) > os.path.getmtime(os.path.realpath(__file__))
            compiledFileExists = os.path.isfile(textureOutputPath + os.path.splitext(name)[0] + ".s3dtex")
            if sourceFileIsNewer or not compiledFileExists or forceRebuildTextures:
                print "Compiling texture \"" + os.path.join(root, name).replace(scriptAbsPath + "/", "") + "\""
                subprocess.call(textureCompilerExe + " " + customArgs[name] + " -d " + textureOutputPath + " -log " + scriptAbsPath + "/Logs " + os.path.join(root, name))
            else:
                print "Texture \"" + os.path.join(root, name).replace(scriptAbsPath + "/", "") + "\" is up-to-date"

##########################################

print "Done in " + str(time.clock() - start) + " seconds.";

os.utime(os.path.realpath(__file__), None)
