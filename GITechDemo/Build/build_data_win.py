import os
import time
import subprocess
import logging
import sys

# Prevent compiling imported .py into .pyc
sys.dont_write_bytecode = True

import utils



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
		logging.info("Starting data build process...")
		logging.info("Data source directory: " + os.getcwd() + dataSrcDir)
		logging.info("")
		for script in dataBuildScript:
			logging.info("Running script: \"" + script + "\"")
			proc = subprocess.Popen( \
				['python.exe', '-u', os.getcwd() + dataSrcDir + script], \
				stdout = subprocess.PIPE, \
				stderr = subprocess.STDOUT, \
				cwd = os.getcwd() + dataSrcDir)
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
