#=============================================================================
# This file is part of the "GITechDemo" application
# Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
#
#       File:   freetype/Makefile
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

#######################
# Build configuration #
#######################

# Overridable

#PLATFORM ?= i686-linux-gnu
PLATFORM ?= x86_64-linux-gnu

#CONFIGURATION ?= Debug
#CONFIGURATION ?= Profile
CONFIGURATION ?= Release

# Static

APP_NAME := GITechDemo
PROJECT_NAME := freetype
BIN_TYPE := a

CPP_FILES :=

C_FILES := \
    src/autofit/autofit.c \
    src/base/ftbase.c \
    src/base/ftbbox.c \
    src/base/ftbitmap.c \
    src/base/ftfntfmt.c \
    src/base/ftfstype.c \
    src/base/ftgasp.c \
    src/base/ftglyph.c \
    src/base/ftgxval.c \
    src/base/ftinit.c \
    src/base/ftlcdfil.c \
    src/base/ftmm.c \
    src/base/ftotval.c \
    src/base/ftpatent.c \
    src/base/ftpfr.c \
    src/base/ftstroke.c \
    src/base/ftsynth.c \
    src/base/ftsystem.c \
    src/base/fttype1.c \
    src/base/ftwinfnt.c \
    src/bdf/bdf.c \
    src/cache/ftcache.c \
    src/cff/cff.c \
    src/cid/type1cid.c \
    src/gzip/ftgzip.c \
    src/lzw/ftlzw.c \
    src/pcf/pcf.c \
    src/pfr/pfr.c \
    src/psaux/psaux.c \
    src/pshinter/pshinter.c \
    src/psnames/psmodule.c \
    src/raster/raster.c \
    src/sfnt/sfnt.c \
    src/smooth/smooth.c \
    src/truetype/truetype.c \
    src/type1/type1.c \
    src/type42/type42.c \
    src/windows/ftdebug.c \
    src/winfonts/winfnt.c

INCLUDE_DIRS := \
    -Iinclude

BIN_DIR := ../../../Bin/$(PLATFORM)/$(CONFIGURATION)/$(APP_NAME)
BINTEMP_DIR := ../../../BinTemp/$(APP_NAME)/$(PROJECT_NAME)/$(PLATFORM)/$(CONFIGURATION)

DEFINES := -DFT2_BUILD_LIBRARY
LIBRARIES :=

##################################################



##################
# Build pipeline #
##################

-include ../../../Build/build_pipeline_linux.makefile

##################################################
