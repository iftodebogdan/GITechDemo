import os
import shutil
import fnmatch
import errno
import sys

# Arguments:
#	'rebuild' to force rebuilding the solution
#	'profile' to build on the Profile configuration

PROJECT_NAME = "GITechDemo"
DEFAULT_VSCOMNTOOLS = "VS100COMNTOOLS"
FORCE_REBUILD = False
BUILD_CONFIGURATION = "Release"

def copyfiles(srcdir, dstdir, filepattern):
	def failed(exc):
		raise exc
	for dirpath, dirs, files in os.walk(srcdir, topdown=True, onerror=failed):
		for file in fnmatch.filter(files, filepattern):
			shutil.copy2(os.path.join(dirpath, file), dstdir)
		break # no recursion

def makedir(path):
	try:
		os.makedirs(path)
	except OSError as exception:
		if exception.errno != errno.EEXIST:
			raise

def copytree(src, dst, symlinks = False, ignore = None):
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
			copytree(s, d, symlinks, ignore)
		else:
			shutil.copy2(s, d)

def buildsln(pathToTools, envSetupBat, platform, buildConfig):
	os.environ["PATH"] += os.pathsep + pathToTools
	cmd = envSetupBat + " && MSBuild.exe /maxcpucount /p:Configuration=" + buildConfig + " /p:Platform=" + platform
	if(FORCE_REBUILD):
		cmd += " /t:rebuild "
	else:
		cmd += " "
	cmd += "\"" + os.getcwd() + "/../Code/Solutions/"
	cmd += PROJECT_NAME + ".sln\""
	#print(cmd)
	os.system(cmd)

#os.system('reg query "HKLM\SOFTWARE\Microsoft\VisualStudio\SxS\VS7"')
#os.system('reg delete "HKLM\SOFTWARE\Microsoft\VisualStudio\SxS\VS7" /v "12.0"')
#os.system("reg add \"HKLM\SOFTWARE\Microsoft\VisualStudio\SxS\VS7\" /v \"12.0\" /t REG_SZ /d \"C:\Program Files (x86)\Microsoft Visual Studio 12.0\\\\\"")

for opt in sys.argv:
	if(opt == "rebuild"):
		FORCE_REBUILD = True
	if(opt == "profile"):
		BUILD_CONFIGURATION = "Profile"

print("\nStarting build process...\n")

envSetupBat = "VsDevCmd.bat"

if(os.getenv(DEFAULT_VSCOMNTOOLS)):
	pathToTools = os.getenv(DEFAULT_VSCOMNTOOLS)
	if(
		DEFAULT_VSCOMNTOOLS == "VS100COMNTOOLS" or
		DEFAULT_VSCOMNTOOLS == "VS90COMNTOOLS" or
		DEFAULT_VSCOMNTOOLS == "VS80COMNTOOLS"
	):
		envSetupBat = "vsvars32.bat"
elif(os.getenv("VS140COMNTOOLS")):	# Visual Studio 2015
	pathToTools = os.getenv("VS140COMNTOOLS")
elif(os.getenv("VS120COMNTOOLS")):	# Visual Studio 2013
	pathToTools = os.getenv("VS120COMNTOOLS")
elif(os.getenv("VS110COMNTOOLS")):	# Visual Studio 2012
	pathToTools = os.getenv("VS110COMNTOOLS")
elif(os.getenv("VS100COMNTOOLS")):	# Visual Studio 2010
	pathToTools = os.getenv("VS100COMNTOOLS")
	envSetupBat = "vsvars32.bat"
#elif(os.getenv("VS90COMNTOOLS")):	# Visual Studio 2008
#	pathToTools = os.getenv("VS90COMNTOOLS")
#	envSetupBat = "vsvars32.bat"
#elif(os.getenv("VS80COMNTOOLS")):	# Visual Studio 2005
#	pathToTools = os.getenv("VS80COMNTOOLS")
#	envSetupBat = "vsvars32.bat"
else:
	print("No compatible version of Visual Studio found!\n")

if(pathToTools):
	buildsln(pathToTools, envSetupBat, "x86", BUILD_CONFIGURATION)
	buildsln(pathToTools, envSetupBat, "x64", BUILD_CONFIGURATION)

print("\nConfiguring build...\n")

# Create directory structure
rootBuildDir = "Windows/" + BUILD_CONFIGURATION + "/" + PROJECT_NAME
makedir("Windows")
makedir(rootBuildDir + "/bin")
makedir(rootBuildDir + "/bin/x64")
makedir(rootBuildDir + "/bin/x86")
makedir(rootBuildDir + "/data")

# Copy 64bit binaries
copyfiles("../Bin/x64/" + BUILD_CONFIGURATION + "/" + PROJECT_NAME + "/", rootBuildDir + "/bin/x64", "*.exe")
copyfiles("../Bin/x64/" + BUILD_CONFIGURATION + "/" + PROJECT_NAME + "/", rootBuildDir + "/bin/x64", "*.dll")

# Copy 32bit binaries
copyfiles("../Bin/x86/" + BUILD_CONFIGURATION + "/" + PROJECT_NAME + "/", rootBuildDir + "/bin/x86", "*.exe")
copyfiles("../Bin/x86/" + BUILD_CONFIGURATION + "/" + PROJECT_NAME + "/", rootBuildDir + "/bin/x86", "*.dll")

# Copy data
copytree("../Data/", rootBuildDir + "/data")

# Create x64 batch files
x64bat = open(rootBuildDir + "/run_x64.bat", "w")
x64bat.write("\
@echo off\n\
:A\n\
cls\n\
cd %~dp0/data\n\
start %1../bin/x64/" + PROJECT_NAME + ".exe\n\
exit")
x64bat.close()

# Create x86 batch files
x86bat = open(rootBuildDir + "/run_x86.bat", "w")
x86bat.write("\
@echo off\n\
:A\n\
cls\n\
cd %~dp0/data\n\
start %1../bin/x86/" + PROJECT_NAME + ".exe\n\
exit")
x86bat.close()

print("DONE!")
