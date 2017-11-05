#=============================================================================
# This file is part of the "GITechDemo" application
# Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
#
#       File:   Build/Makefile
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

PROJECT_ROOTS := \
    ../Code/External/freetype-2.6 \
    ../Code/External/gainput-0.10.0-beta \
    ../Code/External/Synesthesia3D \
    ../Code/AppMain

##################################################



##################
# Build pipeline #
##################

.RECIPEPREFIX +=

TARGETS := build clean rebuild

$(TARGETS) : $(PROJECT_ROOTS)

$(PROJECT_ROOTS) :
    $(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TARGETS) $(PROJECT_ROOTS)

##################################################
