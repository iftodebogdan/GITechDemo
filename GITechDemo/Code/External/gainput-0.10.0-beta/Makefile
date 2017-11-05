#=============================================================================
# This file is part of the "GITechDemo" application
# Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
#
#       File:   gainput/Makefile
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
PROJECT_NAME := gainput
BIN_TYPE := a

CPP_FILES := \
    $(wildcard source/*.cpp) \
    $(wildcard source/builtin/*.cpp) \
    $(wildcard source/dev/*.cpp) \
    $(wildcard source/gestures/*.cpp) \
    $(wildcard source/keyboard/*.cpp) \
    $(wildcard source/mouse/*.cpp) \
    $(wildcard source/pad/*.cpp) \
    $(wildcard source/recorder/*.cpp) \
    $(wildcard source/touch*.cpp) \

C_FILES :=

INCLUDE_DIRS := \
    -Iinclude

BIN_DIR := ../../../Bin/$(PLATFORM)/$(CONFIGURATION)/$(APP_NAME)
BINTEMP_DIR := ../../../BinTemp/$(APP_NAME)/$(PROJECT_NAME)/$(PLATFORM)/$(CONFIGURATION)

DEFINES :=
LIBRARIES :=

##################################################



##################
# Build pipeline #
##################

-include ../../../Build/build_pipeline_linux.makefile

##################################################
