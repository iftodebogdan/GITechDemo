#=============================================================================
# This file is part of the "GITechDemo" application
# Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
#
#       File:   AppMain/Makefile
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
PROJECT_NAME := GITechDemo
BIN_TYPE := elf

CPP_FILES := \
$(wildcard Framework/*.cpp) \
$(wildcard Framework/Linux/*.cpp) \
$(wildcard GITechDemo/*.cpp) \
$(wildcard GITechDemo/RenderScheme/*.cpp) \
$(wildcard GITechDemo/Resources/*.cpp) \
$(wildcard GITechDemo/Utilities/*.cpp)

C_FILES :=

INCLUDE_DIRS := \
-IFramework \
-IFramework/Linux \
-IGITechDemo \
-IGITechDemo/Resources \
-IGITechDemo/RenderScheme \
-IGITechDemo/Utilities \
-I../External/freetype-2.6/include \
-I../External/gainput-0.10.0-beta/include \
-I../External/Synesthesia3D \
-I../External/Synesthesia3D/Base \
-I../External/Synesthesia3D/External/gmtl/include

BIN_DIR := ../../Bin/$(PLATFORM)/$(CONFIGURATION)/$(APP_NAME)
BINTEMP_DIR := ../../BinTemp/$(APP_NAME)/$(PROJECT_NAME)/$(PLATFORM)/$(CONFIGURATION)

DEFINES :=
LIBRARIES := '-Wl,-rpath,$$ORIGIN' -L$(BIN_DIR) -l:synesthesia3d.so -l:gainput.a -lX11 -l:freetype.a -lpthread -l"m" -l"GL" -l"GLU" -l"glut"

##################################################



##################
# Build pipeline #
##################

-include ../../Build/build_pipeline_linux.makefile

##################################################
