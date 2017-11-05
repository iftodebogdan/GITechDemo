#=============================================================================
# This file is part of the "GITechDemo" application
# Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
#
#       File:   build_utils.py
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



def GetScriptPath():
    return os.path.dirname(os.path.realpath(sys.argv[0]))

def GetScriptAbsolutePath():
    return os.path.abspath(os.path.dirname(os.path.realpath(sys.argv[0])))



def FindBuildEnvironment():
    proc = subprocess.Popen( \
        "..\\Tools\\vswhere\\vswhere.exe", \
        stdout = subprocess.PIPE, \
        stderr = subprocess.STDOUT)
    for line in iter(proc.stdout.readline, ""):
        result = line.strip().split(": ", 1)
        if result[0] == "installationPath":
            return result[1] + "\\Common7\\Tools"
    return ""


def BuildSLN(slnName, pathToTools, platform, buildConfig, forceRebuild):
    os.environ["PATH"] += os.pathsep + pathToTools
    cmd = \
        "VsDevCmd.bat" + \
        " && MSBuild.exe /maxcpucount /p:Configuration=" + buildConfig + \
        " /p:Platform=" + platform
    if forceRebuild:
        cmd += " /t:rebuild "
    else:
        cmd += " "
    cmd += "\"" + os.path.realpath(GetScriptAbsolutePath() + "/../Code/Solutions/") + "/"
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
    logDirPath = GetScriptAbsolutePath() + "/Logs/"
    MakeDir(logDirPath)
    logging.basicConfig( \
        filename = logDirPath + logName + "_" + \
        "{:%Y%m%d%H%M%S}".format(datetime.datetime.now()) + \
        ".log", level = logging.INFO, \
        format = '%(asctime)s - %(levelname)s: %(message)s', \
        datefmt = '%d.%m.%Y %H:%M:%S')  
    logging.getLogger().addHandler(logging.StreamHandler(sys.stdout))

#####################
