﻿#=============================================================================
# This file is part of the "GITechDemo" application
# Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
#
#       File:   build_project.py
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
import subprocess

# Prevent compiling imported .py into .pyc
sys.dont_write_bytecode = True

import utils
import build_tools
import build_data



#############################################################################
#       Arguments:                                                          #
#---------------------------------------------------------------------------#
#   'rebuild' to force rebuilding the solution                              #
#   'release' to build on the Release configuration (default)               #
#   'profile' to build on the Profile configuration                         #
#   'x86' to build for 32 bit architecture                                  #
#   'x64' to build for 64 bit architecture (default)                        #
#   'windows' to generate a Windows build                                   #
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
    defaultPlatform = "Windows"

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
        if(opt.lower() == "release"):
            defaultBuildConfiguration = "Release"
        if(opt.lower() == "profile"):
            defaultBuildConfiguration = "Profile"
        if(opt.lower() == "x64"):
            defaultArchitecture = "x64"
        if(opt.lower() == "x86"):
            defaultArchitecture = "x86"
        if(opt.lower() == "windows"):
            defaultPlatform = "Windows"



    # Compile tools, if required
    build_tools.Run()
    logging.info("")



    # Compile data, if required
    build_data.Run()
    logging.info("")



    # Setup build tools
    buildEnvPath = utils.FindBuildEnvironment()
    if buildEnvPath == "":
        logging.error("No compatible version of Visual Studio found!")
        return 1



    # Compile project
    logging.info("Starting project build process...")
    logging.info("Force rebuild: " + str(defaultForceRebuild))
    logging.info("Build configuration: " + defaultBuildConfiguration)
    logging.info("Architecture: " + defaultArchitecture)
    logging.info("")

    startProjectBuild = time.clock()
    if utils.BuildSLN(projectName, buildEnvPath, defaultPlatform + "_" + defaultArchitecture, defaultBuildConfiguration, defaultForceRebuild) != 0:
        logging.error("Could not complete project build process")
        return 1
    else:
        logging.info( \
            "Done building " + projectName + \
            " (" + defaultBuildConfiguration + "|" + defaultArchitecture + ") in " + \
            str(time.clock() - startProjectBuild) + " seconds.")
        logging.info("")



    # Copy and organize build files
    logging.info("Configuring build:")

    # Create directory structure
    logging.info("Creating directory structure...")
    rootBuildDir = os.path.realpath(utils.GetScriptAbsolutePath() + "/" + defaultPlatform + "/" + projectName + "/")
    utils.MakeDir(rootBuildDir)

    # Copy binaries
    logging.info("Copying binaries...")
    pathToBinaries = [
        utils.GetScriptAbsolutePath() +
        "/../Bin/" + defaultArchitecture + "/" + defaultBuildConfiguration + "/" + projectName + "/",
        rootBuildDir
        ]
    utils.CopyFiles(pathToBinaries[0], pathToBinaries[1], "*.exe")
    utils.CopyFiles(pathToBinaries[0], pathToBinaries[1], "*.dll")

    # Copy data
    logging.info("Copying data...")
    utils.CopyTree(utils.GetScriptAbsolutePath() + "/../Data/", rootBuildDir)

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
    sys.exit(Run())
