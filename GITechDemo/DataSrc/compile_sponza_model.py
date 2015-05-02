import os
import subprocess
import time

#pathToModelFile = "sponza_scene/"
pathToModelFile = "crytek-sponza/"
modelFile = "sponza.obj"
outputPath = "../Data/models/sponza/"
modelCompilerExe = "../Bin/x64/Release/LibRendererTools/ModelCompiler.exe"

start = time.clock()

print "Compiling model \"" + pathToModelFile + modelFile + "\""
subprocess.call([modelCompilerExe, "-q", "-d", outputPath, pathToModelFile + modelFile])

print "Done in " + str(time.clock() - start) + " seconds";
