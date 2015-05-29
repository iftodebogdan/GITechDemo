import os
import shutil
import fnmatch
import errno
import sys

PROJECT_NAME = "GITechDemo"
DEFAULT_VSCOMNTOOLS = "VS120COMNTOOLS"
FORCE_REBUILD = False

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

def buildsln(pathToTools, platform):
	cmd = "\"" + pathToTools + "VsDevCmd.bat\" && "
	cmd += "MSBuild.exe /maxcpucount /p:Configuration=Release /p:Platform=" + platform
	if(FORCE_REBUILD):
		cmd += " /t:rebuild "
	else:
		cmd += " "
	cmd += os.getcwd() + "/../Code/Solutions/"
	cmd += PROJECT_NAME + ".sln"
	#print(cmd)
	os.system(cmd)

#os.system('reg query "HKLM\SOFTWARE\Microsoft\VisualStudio\SxS\VS7"')
#os.system('reg delete "HKLM\SOFTWARE\Microsoft\VisualStudio\SxS\VS7" /v "12.0"')
#os.system("reg add \"HKLM\SOFTWARE\Microsoft\VisualStudio\SxS\VS7\" /v \"12.0\" /t REG_SZ /d \"C:\Program Files (x86)\Microsoft Visual Studio 12.0\\\\\"")

if(len(sys.argv) > 1 and sys.argv[1] == "rebuild"):
	FORCE_REBUILD = True;

print("\nStarting build process...\n")

if(os.getenv(DEFAULT_VSCOMNTOOLS)):
	pathToTools = os.getenv(DEFAULT_VSCOMNTOOLS)
elif(os.getenv("VS140COMNTOOLS")):	# Visual Studio 2015
	pathToTools = os.getenv("VS140COMNTOOLS")
elif(os.getenv("VS120COMNTOOLS")):	# Visual Studio 2013
	pathToTools = os.getenv("VS120COMNTOOLS")
#elif(os.getenv("VS110COMNTOOLS")):	# Visual Studio 2012
#	pathToTools = os.getenv("VS110COMNTOOLS")
#elif(os.getenv("VS100COMNTOOLS")):	# Visual Studio 2010
#	pathToTools = os.getenv("VS100COMNTOOLS")
#elif(os.getenv("VS90COMNTOOLS")):	# Visual Studio 2008
#	pathToTools = os.getenv("VS90COMNTOOLS")
#elif(os.getenv("VS80COMNTOOLS")):	# Visual Studio 2005
#	pathToTools = os.getenv("VS80COMNTOOLS")
else:
	print("No compatible version of Visual Studio found!\n")

if(pathToTools):
	buildsln(pathToTools, "x86")
	buildsln(pathToTools, "x64")

print("\nConfiguring build...\n");

# Create directory structure
makedir("Windows")
makedir("Windows/bin")
makedir("Windows/bin/x64")
makedir("Windows/bin/x86")
makedir("Windows/data")

# Copy 64bit binaries
copyfiles("../Bin/x64/Release/" + PROJECT_NAME + "/", "./Windows/bin/x64", "*.exe")
copyfiles("../Bin/x64/Release/" + PROJECT_NAME + "/", "./Windows/bin/x64", "*.dll")

# Copy 32bit binaries
copyfiles("../Bin/Win32/Release/" + PROJECT_NAME + "/", "./Windows/bin/x86", "*.exe")
copyfiles("../Bin/Win32/Release/" + PROJECT_NAME + "/", "./Windows/bin/x86", "*.dll")

# Copy data
copytree("../Data/", "./Windows/data")

# Create x64 batch files
x64bat = open("./Windows/run_x64.bat", "w")
x64bat.write("\
@echo off\n\
:A\n\
cls\n\
cd %~dp0/data\n\
start %1../bin/x64/" + PROJECT_NAME + ".exe\n\
exit")
x64bat.close()

# Create x86 batch files
x86bat = open("./Windows/run_x86.bat", "w")
x86bat.write("\
@echo off\n\
:A\n\
cls\n\
cd %~dp0/data\n\
start %1../bin/x86/" + PROJECT_NAME + ".exe\n\
exit")
x86bat.close()

print("DONE!");