import os
import sys
import time
import logging
import subprocess

# Prevent compiling imported .py into .pyc
sys.dont_write_bytecode = True

import utils
import build_tools_win
import build_data_win



#############################################################################
#		Arguments:															#
#---------------------------------------------------------------------------#
#	'rebuild' to force rebuilding the solution								#
#	'release' to build on the Release configuration (default)				#
#	'profile' to build on the Profile configuration							#
#	'win32' to build for 32 bit architecture								#
#	'x64' to build for 64 bit architecture (default)						#
#	'vs20XX' to build against Visual Studio 20XX toolset (default: vs2010)	#
#############################################################################



def Run():
	#################
	# Configuration #
	#################
	
	# Name of .sln file for the current project
	projectName = "GITechDemo"
	
	defaultForceRebuild = False
	defaultBuildConfiguration = "Release"
	defaultArchitecture = "x64"
	defaultVSBuildTools = "VS100COMNTOOLS"
	defaultPlatformToolset = "v100"
	
	#################



	########
	# Main #
	########

	#os.system('reg query "HKLM\SOFTWARE\Microsoft\VisualStudio\SxS\VS7"')
	#os.system('reg delete "HKLM\SOFTWARE\Microsoft\VisualStudio\SxS\VS7" /v "12.0"')
	#os.system("reg add \"HKLM\SOFTWARE\Microsoft\VisualStudio\SxS\VS7\" /v \"12.0\" /t REG_SZ /d \"C:\Program Files (x86)\Microsoft Visual Studio 12.0\\\\\"")
	
	# Start timer
	start = time.clock()
	
	# Process command arguments
	for opt in sys.argv:
		if(opt.lower() == "rebuild"):
			defaultForceRebuild = True
		if(opt.lower() == "release"):
			defaultBuildConfiguration = "Release"
		if(opt.lower() == "profile"):
			defaultBuildConfiguration = "Profile"
		if(opt.lower() == "x64"):
			defaultArchitecture = "x64"
		if(opt.lower() == "win32"):
			defaultArchitecture = "Win32"
		#if(opt.lower() == "vs2005"):
		#	defaultVSBuildTools = "VS80COMNTOOLS"
		#	defaultPlatformToolset = "vs80"
		#if(opt.lower() == "vs2008"):
		#	defaultVSBuildTools = "VS90COMNTOOLS"
		#	defaultPlatformToolset = "vs90"
		if(opt.lower() == "vs2010"):
			defaultVSBuildTools = "VS100COMNTOOLS"
			defaultPlatformToolset = "vs100"
		if(opt.lower() == "vs2012"):
			defaultVSBuildTools = "VS110COMNTOOLS"
			defaultPlatformToolset = "vs110"
		if(opt.lower() == "vs2013"):
			defaultVSBuildTools = "VS120COMNTOOLS"
			defaultPlatformToolset = "vs120"
		if(opt.lower() == "vs2015"):
			defaultVSBuildTools = "VS140COMNTOOLS"
			defaultPlatformToolset = "vs140"
	
	
	
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
	
	
	
	# Compile code (tools too, if required)
	build_tools_win.Run()
	logging.info("")
		
	logging.info("Starting project build process...")
	logging.info("Force rebuild: " + str(defaultForceRebuild))
	logging.info("Build configuration: " + defaultBuildConfiguration)
	logging.info("Architecture: " + defaultArchitecture)
	logging.info("Platform toolset: " + defaultPlatformToolset)
	logging.info("")
	
	startProjectBuild = time.clock()
	if utils.BuildSLN(projectName, pathToTools, defaultPlatformToolset, envSetupBat, defaultArchitecture, defaultBuildConfiguration, defaultForceRebuild) != 0:
		logging.error("Could not complete project build process")
		exit()
	else:
		logging.info( \
			"Done building " + projectName + \
			" (" + defaultBuildConfiguration + "|" + defaultArchitecture + ") in " + \
			str(time.clock() - startProjectBuild) + " seconds.")
		logging.info("")
	
	
	
	# Compile data
	build_data_win.Run()
	logging.info("")
	
	
	
	# Copy and organize build files
	logging.info("Configuring build:")
	
	# Create directory structure
	logging.info("Creating directory structure...")
	rootBuildDir = "Windows/" + projectName
	utils.MakeDir(rootBuildDir + "/bin/" +
		("" if (defaultBuildConfiguration == "Release") else (defaultBuildConfiguration + "/")) +
		defaultArchitecture)
	utils.MakeDir(rootBuildDir + "/data")
	
	# Copy binaries
	logging.info("Copying binaries...")
	pathToBinaries = [
		"../Bin/" + defaultArchitecture + "/" + defaultBuildConfiguration + "/" + projectName + "/",
		rootBuildDir + "/bin/" +
		("" if (defaultBuildConfiguration == "Release") else (defaultBuildConfiguration + "/")) +
		defaultArchitecture
		]
	utils.CopyFiles(pathToBinaries[0], pathToBinaries[1], "*.exe")
	utils.CopyFiles(pathToBinaries[0], pathToBinaries[1], "*.dll")
	
	# Copy data
	logging.info("Copying data...")
	utils.CopyTree("../Data/", rootBuildDir + "/data")
	
	# Create batch file
	logging.info("Creating batch file...")
	startBat = open(
		rootBuildDir + "/run_" + 
		("" if (defaultBuildConfiguration == "Release") else (defaultBuildConfiguration.lower() + "_")) +
		defaultArchitecture.lower() + ".bat", "w"
	)
	
	startBat.write("\
	@echo off\n\
	:A\n\
	cls\n\
	cd %~dp0/data\n\
	start %1../bin/" +
	("" if (defaultBuildConfiguration == "Release") else (defaultBuildConfiguration + "/")) +
	defaultArchitecture + "/" +
	projectName + ".exe\n\
	exit"
	)
	startBat.close()
	
	logging.info("")
	
	
	
	# Done! Print some info.
	logging.info(
		"Done building project " + projectName +
		" (" + defaultBuildConfiguration + "|" + defaultArchitecture + ") in " +
		str(time.clock() - start) + " seconds."
	)
	
	return 0

########



##############
# Run script #
##############

if __name__ == "__main__":
	utils.SetupLogging("ProjectBuild")
	Run()
