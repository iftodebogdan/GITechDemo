#=============================================================================
# This file is part of the "GITechDemo" application
# Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
#
#       File:   build_tools_win.py
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
import sys
import time
import logging

# Prevent compiling imported .py into .pyc
sys.dont_write_bytecode = True

import utils



#############################################################################
#       Arguments:                                                          #
#---------------------------------------------------------------------------#
#   'win32' to build the 32 bit version of the tools                        #
#   'x64' to build the 64 bit version of the tools (default)                #
#############################################################################



def Run():
    #################
    # Configuration #
    #################
    
    # List of .sln files for tools that are required for building data
    toolsName = [
        "Synesthesia3DTools"
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
    elif(os.getenv("VS140COMNTOOLS")):  # Visual Studio 2015
        pathToTools = os.getenv("VS140COMNTOOLS")
        defaultPlatformToolset = "vs140"
    elif(os.getenv("VS120COMNTOOLS")):  # Visual Studio 2013
        pathToTools = os.getenv("VS120COMNTOOLS")
        defaultPlatformToolset = "vs120"
    elif(os.getenv("VS110COMNTOOLS")):  # Visual Studio 2012
        pathToTools = os.getenv("VS110COMNTOOLS")
        defaultPlatformToolset = "vs110"
    elif(os.getenv("VS100COMNTOOLS")):  # Visual Studio 2010
        pathToTools = os.getenv("VS100COMNTOOLS")
        defaultPlatformToolset = "vs100"
        envSetupBat = "vsvars32.bat"
    #elif(os.getenv("VS90COMNTOOLS")):  # Visual Studio 2008
    #   pathToTools = os.getenv("VS90COMNTOOLS")
    #   defaultPlatformToolset = "vs90"
    #   envSetupBat = "vsvars32.bat"
    #elif(os.getenv("VS80COMNTOOLS")):  # Visual Studio 2005
    #   pathToTools = os.getenv("VS80COMNTOOLS")
    #   defaultPlatformToolset = "vs80"
    #   envSetupBat = "vsvars32.bat"
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
