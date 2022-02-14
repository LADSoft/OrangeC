# Software License Agreement
# 
#     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
# 
#     This file is part of the Orange C Compiler package.
# 
#     The Orange C Compiler package is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
# 
#     The Orange C Compiler package is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
# 
#     You should have received a copy of the GNU General Public License
#     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
# 
#     contact information:
#         email: TouchStone222@runbox.com <David Lindauer>
# 

ifeq "$(COMPILER)" "gcc-linux"

COMPILER_PATH := 
OBJ_IND_PATH := gcc-linux

CPP_deps = $(notdir $(CPP_DEPENDENCIES:.cpp=.o))
C_deps = $(notdir $(C_DEPENDENCIES:.c=.o))
ASM_deps = $(notdir $(ASM_DEPENDENCIES:.nas=.o))
TASM_deps = $(notdir $(TASM_DEPENDENCIES:.asm=.o))
RES_deps = $(notdir $(RC_DEPENDENCIES:.rc=.o))

MAIN_DEPENDENCIES = $(MAIN_FILE:.cpp=.o)
ifeq "$(MAIN_DEPENDENCIES)" "$(MAIN_FILE)"
MAIN_DEPENDENCIES = $(MAIN_FILE:.c=.o)
endif

LLIB_DEPENDENCIES = $(notdir $(filter-out $(EXCLUDE) $(MAIN_DEPENDENCIES), $(CPP_deps) $(C_deps) $(ASM_deps) $(TASM_deps)))


#CC=gcc
#CXX=g++
CCFLAGS = -c -D__MSVCRT__ -U__STRICT_ANSI__ -DHAVE_UNISTD_H=1 -DSQLITE_OS_UNIX -D_unlink=unlink -Dstricmp=strcasecmp -Dstrnicmp=strncasecmp -Wno-int-to-pointer-cast -D_strnicmp=strncasecmp

CPPFLAGS = -std=c++14
LINK=ld
LFLAGS=-L$(_LIBDIR)

LIB=ar
LIBFLAGS= -r -s
LIB_EXT:=.a
LIB_PREFIX:=lib

ASM=as
ASMFLAGS=

RC=windres
RCFLAGS=-DWINVER=0x400

ifneq "$(INCLUDES)" ""
CINCLUDES:=$(addprefix -I,$(INCLUDES))
endif

DEFINES:=$(addprefix -D,$(DEFINES))
DEFINES:=$(subst @, ,$(DEFINES))

CFLAGS := $(CCFLAGS) $(CINCLUDES) $(DEFINES) -DGNUC -std=c99
CXXFLAGS := $(CCFLAGS) $(CINCLUDES) -fpermissive $(DEFINES) -DGNUC -std=c++14

ifeq "$(TARGET)" "GUI"
LFLAGS := $(LFLAGS) -s -Wl,--subsystem,windows
endif

COMPLIB=-lstdc++ -lpthread -ldl


vpath %.o $(_OUTPUTDIR)
vpath %.a $(_LIBDIR)
vpath %.res $(_OUTPUTDIR)

$(_OUTPUTDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

$(_OUTPUTDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^

$(_OUTPUTDIR)/%.o: %.asm
	$(ASM) $(ASMFLAGS) -o $@ $^

%.o: %.rc
	$(RC) $(RCFLAGS) -i $^ -o $(_OUTPUTDIR)/$@

$(_LIBDIR)/$(LIB_PREFIX)$(NAME)$(LIB_EXT): $(addprefix $(subst \,/,$(_OUTPUTDIR)/),$(LLIB_DEPENDENCIES))
	$(LIB) $(LIBFLAGS) $(_LIBDIR)/$(LIB_PREFIX)$(NAME)$(LIB_EXT) $(addprefix $(subst \,/,$(_OUTPUTDIR)/),$(LLIB_DEPENDENCIES))
LDEPS := $(addprefix -l,$(NAME) $(LIB_DEPENDENCIES))
LDEPS := $(subst \,/,$(LDEPS))

LDEPS2 := $(addprefix $(LIB_PREFIX),$(NAME) $(LIB_DEPENDENCIES))
LDEPS2 := $(addsuffix .a, $(LDEPS2))

LMAIN := $(addprefix $(_OUTPUTDIR)/,$(MAIN_DEPENDENCIES) $(RES_deps))
LMAIN := $(subst \,/,$(LMAIN))

$(NAME).exe: $(addprefix $(_OUTPUTDIR)/,$(MAIN_DEPENDENCIES)) $(LDEPS2) $(RES_deps)
	$(CC) $(LFLAGS) -o $(NAME).exe $(LMAIN) $(LDEPS) $(COMPLIB) $(DEF_DEPENDENCIES)

%.exe: %.c
	$(CC) $(LFLAGS) -o $@ $^ $(COMPLIB)

%.exe: %.cpp
	$(CC) $(LFLAGS) -o $@ $^ $(COMPLIB)

endif