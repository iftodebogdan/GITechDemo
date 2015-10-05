import os
import shutil
import fnmatch
import errno
import logging
import datetime
import sys
import subprocess

#####################
# Utility functions #
#####################

def CopyFiles(srcdir, dstdir, filepattern):
	def failed(exc):
		raise exc
	for dirpath, dirs, files in os.walk(srcdir, topdown=True, onerror=failed):
		for file in fnmatch.filter(files, filepattern):
			shutil.copy2(os.path.join(dirpath, file), dstdir)
		break # no recursion



def MakeDir(path):
	try:
		os.makedirs(path)
	except OSError as exception:
		if exception.errno != errno.EEXIST:
			raise



def CopyTree(src, dst, symlinks = False, ignore = None):
	if not os.path.exists(dst):
		os.makedirs(dst)
		shutil.copystat(src, dst)
	lst = os.listdir(src)
	if ignore:
		excl = ignore(src, lst)
		lst = [x for x in lst if x not in excl]
	for item in lst:
		s = os.path.join(src, item)
		d = os.path.join(dst, item)
		if symlinks and os.path.islink(s):
			if os.path.lexists(d):
				os.remove(d)
			os.symlink(os.readlink(s), d)
			try:
				st = os.lstat(s)
				mode = stat.S_IMODE(st.st_mode)
				os.lchmod(d, mode)
			except:
				pass # lchmod not available
		elif os.path.isdir(s):
			CopyTree(s, d, symlinks, ignore)
		else:
			shutil.copy2(s, d)



def BuildSLN(slnName, pathToTools, platformToolset, envSetupBat, platform, buildConfig, forceRebuild):
	os.environ["PATH"] += os.pathsep + pathToTools
	cmd = \
		envSetupBat + \
		" && MSBuild.exe /maxcpucount /p:Configuration=" + buildConfig + \
		" /p:Platform=" + platform + \
		" /p:PlatformToolset=" + platformToolset
	if forceRebuild:
		cmd += " /t:rebuild "
	else:
		cmd += " "
	cmd += "\"" + os.getcwd() + "/../Code/Solutions/"
	cmd += slnName + ".sln\""
	
	proc = subprocess.Popen( \
		cmd, \
		stdout = subprocess.PIPE, \
		stderr = subprocess.STDOUT)
	for line in iter(proc.stdout.readline, ""):
		logging.info(line.replace('\n', '').replace('\r', ''))
	logging.info("")
	return proc.wait()



def SetupLogging(logName):
	MakeDir("Logs")
	logging.basicConfig( \
		filename = "Logs/" + logName + "_" + \
		"{:%Y%m%d%H%M%S}".format(datetime.datetime.now()) + \
		".log", level = logging.INFO, \
		format = '%(asctime)s - %(levelname)s: %(message)s', \
		datefmt = '%d.%m.%Y %H:%M:%S')	
	logging.getLogger().addHandler(logging.StreamHandler(sys.stdout))

#####################
