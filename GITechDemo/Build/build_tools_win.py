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
        if(opt.lower() == "win32" or opt.lower() == "x86"):
            defaultArchitecture = "Win32"



    # Setup build tools
    buildEnvPath = utils.FindBuildEnvironment()
    if buildEnvPath == "":
        logging.error("No compatible version of Visual Studio found!")
        return 1



    # Compile code
    logging.info("Starting tools build process...")
    logging.info("Force rebuild: " + str(defaultForceRebuild))
    logging.info("Architecture: " + defaultArchitecture)
    logging.info("")
    for dep in toolsName:
        startPerTool = time.clock()
        if utils.BuildSLN(dep, buildEnvPath, defaultArchitecture, "Release", defaultForceRebuild) != 0:
            logging.error("Could not complete tool build process")
            return 1
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
    sys.exit(Run())
