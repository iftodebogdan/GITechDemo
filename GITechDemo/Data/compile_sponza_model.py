import os
import subprocess
import time

#pathToModelFile = "sponza_scene/"
pathToModelFile = "crytek-sponza/"
modelFile = "sponza.obj"
outputPath = "sponza/"
modelCompilerExe = "../Bin/x64/Release/ModelCompiler/ModelCompiler.exe"

start = time.clock()

print "Compiling model \"" + pathToModelFile + modelFile + "\""
subprocess.call([modelCompilerExe, "-q", "-d", outputPath, pathToModelFile + modelFile])

print "Done in " + str(time.clock() - start) + " seconds";
