# Software License Agreement
# 
#     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
# 
#     This file is part of the Orange C Compiler package.
# 
#     The Orange C Compiler package is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version, with the addition of the 
#     Orange C "Target Code" exception.
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


CC=gcc
CCFLAGS = -c -D__MSVCRT__ -U__STRICT_ANSI__ -DGCCLINUX -DSQLITE_OS_UNIX

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

CCFLAGS := $(CCFLAGS) $(CINCLUDES) $(DEFINES)\
    -DGNUC -DWIN32 -D_WIN32_IE=0x600 -D_WIN32_WINNT=0x500 -DWINVER=0x500

ifeq "$(TARGET)" "GUI"
LFLAGS := $(LFLAGS) -s -Wl,--subsystem,windows
endif

COMPLIB=-lstdc++ -lcomctl32 -lgdi32 -lcomdlg32 -lole32 -luxtheme -lkernel32 -lmsimg32 -lshell32


vpath %.o $(_OUTPUTDIR)
vpath %.a $(_LIBDIR)
vpath %.res $(_OUTPUTDIR)

$(_OUTPUTDIR)/%.o: %.cpp
	$(CC) -std=c++11 $(CCFLAGS) -o $@ $^

$(_OUTPUTDIR)/%.o: %.c
	$(CC) $(CCFLAGS) -o $@ $^

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