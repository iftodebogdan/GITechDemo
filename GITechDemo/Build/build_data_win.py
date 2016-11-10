#=============================================================================
# This file is part of the "GITechDemo" application
# Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
#
#       File:   build_data_win.py
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
import time
import subprocess
import logging
import sys

# Prevent compiling imported .py into .pyc
sys.dont_write_bytecode = True

import utils



#############################################################################
#       Arguments:                                                          #
#---------------------------------------------------------------------------#
#   'win32' to use the 32 bit version of the tools                          #
#   'x64' to use the 64 bit version of the tools (default)                  #
#   'rebuild' to force rebuilding all data assets                           #
#############################################################################



def Run():
    #################
    # Configuration #
    #################
    
    # Path to folder with data build scripts (relative to this script's folder)
    dataSrcDir = "/../DataSrc/"
    
    # List of scripts to run for building data
    dataBuildScript = [
        "compile_sponza_model.py",
        "compile_utility_textures.py"
        ]
    
    #################



    # Start timer
    start = time.clock()

    # Compile data
    if(dataBuildScript):
        absDataSrcDir = os.path.realpath(utils.GetScriptAbsolutePath() + dataSrcDir)
        logging.info("Starting data build process...")
        logging.info("Data source directory: " + absDataSrcDir)
        logging.info("")
        for script in dataBuildScript:
            logging.info("Running script: \"" + script + "\"")
            proc = subprocess.Popen( \
                ['python.exe', '-u', absDataSrcDir + "/" + script] + sys.argv[1:], \
                stdout = subprocess.PIPE, \
                stderr = subprocess.STDOUT, \
                cwd = absDataSrcDir)
            for line in iter(proc.stdout.readline, ""):
                logging.info(line.replace('\n', '').replace('\r', ''))
            if proc.wait() != 0:
                logging.info(script + " has failed!")
                exit()
            logging.info("")



    # Done! Print some info.
    logging.info("Done compiling data in " + str(time.clock() - start) + " seconds.")
    
    return 0

########



##############
# Run script #
##############

if __name__ == "__main__":
    utils.SetupLogging("DataBuild")
    Run()
