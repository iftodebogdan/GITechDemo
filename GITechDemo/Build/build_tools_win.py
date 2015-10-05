import os
import sys
import time
import logging

# Prevent compiling imported .py into .pyc
sys.dont_write_bytecode = True

import utils



#############################################################################
#		Arguments:															#
#---------------------------------------------------------------------------#
#	'win32' to build the 32 bit version of the tools						#
#	'x64' to build the 64 bit version of the tools (default)				#
#############################################################################



def Run():
	#################
	# Configuration #
	#################
	
	# List of .sln files for tools that are required for building data
	toolsName = [
		"LibRendererTools"
		]
	
	defaultForceRebuild = False
	defaultArchitecture = "x64"
	defaultVSBuildTools = "VS100COMNTOOLS"
	defaultPlatformToolset = "v100"
	
	#################
	
	
	
	########
	# Main #
	########

	# Start timer
	start = time.clock()

	# Process command arguments
	for opt in sys.argv:
		if(opt.lower() == "rebuild"):
			defaultForceRebuild = True
		if(opt.lower() == "x64"):
			defaultArchitecture = "x64"
		if(opt.lower() == "win32"):
			defaultArchitecture = "Win32"



	# Setup build tools
	envSetupBat = "VsDevCmd.bat"
	if(os.getenv(defaultVSBuildTools)):
		pathToTools = os.getenv(defaultVSBuildTools)
		if(
			defaultVSBuildTools == "VS100COMNTOOLS" or
			defaultVSBuildTools == "VS90COMNTOOLS" or
			defaultVSBuildTools == "VS80COMNTOOLS"
		):
			envSetupBat = "vsvars32.bat"
	elif(os.getenv("VS140COMNTOOLS")):	# Visual Studio 2015
		pathToTools = os.getenv("VS140COMNTOOLS")
		defaultPlatformToolset = "vs140"
	elif(os.getenv("VS120COMNTOOLS")):	# Visual Studio 2013
		pathToTools = os.getenv("VS120COMNTOOLS")
		defaultPlatformToolset = "vs120"
	elif(os.getenv("VS110COMNTOOLS")):	# Visual Studio 2012
		pathToTools = os.getenv("VS110COMNTOOLS")
		defaultPlatformToolset = "vs110"
	elif(os.getenv("VS100COMNTOOLS")):	# Visual Studio 2010
		pathToTools = os.getenv("VS100COMNTOOLS")
		defaultPlatformToolset = "vs100"
		envSetupBat = "vsvars32.bat"
	#elif(os.getenv("VS90COMNTOOLS")):	# Visual Studio 2008
	#	pathToTools = os.getenv("VS90COMNTOOLS")
	#	defaultPlatformToolset = "vs90"
	#	envSetupBat = "vsvars32.bat"
	#elif(os.getenv("VS80COMNTOOLS")):	# Visual Studio 2005
	#	pathToTools = os.getenv("VS80COMNTOOLS")
	#	defaultPlatformToolset = "vs80"
	#	envSetupBat = "vsvars32.bat"
	else:
		logging.error("No compatible version of Visual Studio found!")
		exit()



	# Compile code
	logging.info("Starting tools build process...")
	logging.info("Force rebuild: " + str(defaultForceRebuild))
	logging.info("Architecture: " + defaultArchitecture)
	logging.info("Platform toolset: " + defaultPlatformToolset)
	logging.info("")
	for dep in toolsName:
		startPerTool = time.clock()
		if utils.BuildSLN(dep, pathToTools, defaultPlatformToolset, envSetupBat, defaultArchitecture, "Release", defaultForceRebuild) != 0:
			logging.error("Could not complete tool build process")
			exit()
		# Done! Print some info.
		infoStr = "Done building"
		infoStr += " " + dep
		logging.info(infoStr + " (" + defaultArchitecture + ") in " + str(time.clock() - startPerTool) + " seconds.")
		logging.info("")

	logging.info("Done building tools in " + str(time.clock() - start) + " seconds.")
	
	return 0

########



##############
# Run script #
##############

if __name__ == "__main__":
	utils.SetupLogging("ToolsBuild")
	Run()
