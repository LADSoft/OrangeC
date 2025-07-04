# Software License Agreement
# 
#     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
# 

ifeq "$(COMPILER)" "MINGW64"

OBJ_IND_PATH := mingw64

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


CC=g++
CCFLAGS = -c -O2 -D__MSVCRT__ -D_MINGW -Wno-int-to-pointer-cast
LINK=ld
LFLAGS=-L$(_LIBDIR) -Xlinker -Map=output.map

LIB=ar
LIBFLAGS=-r -s
LIB_EXT:=.a
LIB_PREFIX:=lib

ASM=as
ASMFLAGS=

RC=windres
RCFLAGS=-DWINVER=0x400

ifneq "$(INCLUDES)" ""
CINCLUDES:=$(addprefix -I,$(INCLUDES))
endif

DEFINES:= NDEBUG TARGET_OS_WINDOWS HAVE_C99INCLUDES $(DEFINES)
DEFINES:=$(addprefix -D,$(DEFINES))
DEFINES:=$(subst @, ,$(DEFINES))

CFLAGS := $(CCFLAGS) $(CINCLUDES) $(DEFINES) -x c -DGNUC -D_WIN32_IE=0x600 -D_WIN32_WINNT=0x500 -DWINVER=0x500 -D_WIN32 -Dx64_compiler 
CXXFLAGS = -std=c++17 -fpermissive $(CCFLAGS) -x c++ $(CINCLUDES) $(DEFINES) -DGNUC -D_WIN32_IE=0x600 -D_WIN32_WINNT=0x500 -DWINVER=0x500 -D_WIN32 -Dx64_compiler

ifeq "$(TARGET)" "GUI"
LFLAGS := $(LFLAGS) -s -Wl,--subsystem,windows
endif

COMPLIB=-lcomctl32 -lgdi32 -lcomdlg32 -lole32 -luxtheme -lkernel32 -lmsimg32 -luuid


vpath %.o $(_OUTPUTDIR)
vpath %.a $(_LIBDIR)
vpath %.res $(_OUTPUTDIR)

%.o: %.cpp
	$(CC) $(CXXFLAGS) -o$(_OUTPUTDIR)/$@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o$(_OUTPUTDIR)/$@ $^

%.o: %.s
	$(ASM) $(ASMFLAGS) -o$(_OUTPUTDIR)/$@ $^

%.o: %.rc
	$(RC) $(RCFLAGS) -i $^ -o $(_OUTPUTDIR)/$@

$(_LIBDIR)\$(LIB_PREFIX)$(NAME)$(LIB_EXT): $(LLIB_DEPENDENCIES)
#	-del $(_LIBDIR)\$(LIB_PREFIX)$(NAME)$(LIB_EXT) 2> $(NULLDEV)
	$(LIB) $(LIBFLAGS) $(_LIBDIR)\$(LIB_PREFIX)$(NAME)$(LIB_EXT) $(addprefix $(_OUTPUTDIR)/,$(LLIB_DEPENDENCIES))

LDEPS := $(addprefix -l,$(NAME) $(LIB_DEPENDENCIES))

LDEPS2 := $(addprefix $(LIB_PREFIX),$(NAME) $(LIB_DEPENDENCIES))
LDEPS2 := $(addsuffix .a, $(LDEPS2))

LMAIN := $(addprefix $(_OUTPUTDIR)\,$(MAIN_DEPENDENCIES) $(RES_deps))

$(NAME).exe: $(MAIN_DEPENDENCIES) $(LDEPS2) $(_LIBDIR)\$(LIB_PREFIX)$(NAME)$(LIB_EXT) $(RES_deps)
	$(CC) $(LFLAGS) -o $(NAME).exe $(LMAIN) $(LDEPS) $(COMPLIB) $(DEF_DEPENDENCIES)


%.exe: %.c
	$(CC) $(LFLAGS) -o $@ $^ $(COMPLIB)

%.exe: %.cpp
	$(CC) $(LFLAGS) -o $@ $^ $(COMPLIB)

endif
