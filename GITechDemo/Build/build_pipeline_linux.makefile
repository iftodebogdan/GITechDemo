#=============================================================================
# This file is part of the "GITechDemo" application
# Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
#
#       File:   build_pipeline_linux.makefile
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

# Configuration

.RECIPEPREFIX +=

BIN_FILENAME := $(shell echo $(PROJECT_NAME).$(BIN_TYPE) | tr A-Z a-z)

OBJ_FILES := $(CPP_FILES:%.cpp=$(BINTEMP_DIR)/%.o)
OBJ_FILES += $(C_FILES:%.c=$(BINTEMP_DIR)/%.o)
DEP_FILES := $(OBJ_FILES:%.o=%.d)

CC := g++
AR := ar rcs
WARNINGS := -Wall -Werror -Wfatal-errors
CC_FLAGS := -c -std=c++11 $(WARNINGS) $(DEFINES) $(INCLUDE_DIRS)
LD_FLAGS := $(WARNINGS)

# Select between 32 and 64 bit
ifeq ($(PLATFORM),i686-linux-gnu)
    CC_FLAGS += -m32
    LD_FLAGS += -m32
else ifeq ($(PLATFORM),x86_64-linux-gnu)
    CC_FLAGS += -m64
    LD_FLAGS += -m64
endif

# Select optimization level
ifeq ($(CONFIGURATION),Debug)
    CC_FLAGS += -g -O0
    LD_FLAGS += -g -O0
else ifeq ($(CONFIGURATION),Profile)
    CC_FLAGS += -pg --coverage -O2
    LD_FLAGS += -pg --coverage -O2
else ifeq ($(CONFIGURATION),Release)
    CC_FLAGS += -O2
    LD_FLAGS += -O2
endif

# Select binary type (executable / dynamic library)
ifeq ($(BIN_TYPE),elf)
    CC_FLAGS +=
    LD_FLAGS +=
else ifeq ($(BIN_TYPE),so)
    CC_FLAGS += -fPIC
    LD_FLAGS += -shared
else ifeq ($(BIN_TYPE),a)
    CC_FLAGS +=
    LD_FLAGS +=
endif

# Build rules

build : $(BIN_DIR)/$(BIN_FILENAME)

ifeq ($(BIN_TYPE),a)
$(BIN_DIR)/$(BIN_FILENAME) : $(OBJ_FILES)
    mkdir -p $(@D)
    $(AR) $@ $^
else
$(BIN_DIR)/$(BIN_FILENAME) : $(OBJ_FILES)
    mkdir -p $(@D)
    $(CC) $(LD_FLAGS) $^ $(LIBRARIES) -o $@
endif

-include $(DEP_FILES)

$(BINTEMP_DIR)/%.o : %.cpp
    mkdir -p $(@D)
    $(CC) $(CC_FLAGS) -MMD -c $< -o $@

$(BINTEMP_DIR)/%.o : %.c
    mkdir -p $(@D)
    $(CC) $(CC_FLAGS) -MMD -c $< -o $@

clean :
    -rm -f $(BIN_DIR)/$(BIN_FILENAME) $(OBJ_FILES) $(DEP_FILES)
    find $(BINTEMP_DIR) -mindepth 1 -type d -empty -delete 2>/dev/null

rebuild : clean build
