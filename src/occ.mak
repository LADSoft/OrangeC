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

ifeq "$(COMPILER)" "OCC"

COMPILER_PATH := $(DISTROOT)
OBJ_IND_PATH := occ

CPP_deps = $(notdir $(CPP_DEPENDENCIES:.cpp=.o))
C_deps = $(notdir $(C_DEPENDENCIES:.c=.o))
ASM_deps = $(notdir $(ASM_DEPENDENCIES:.s=.o))
TASM_deps = $(notdir $(TASM_DEPENDENCIES:.asm=.o))
RES_deps = $(notdir $(RC_DEPENDENCIES:.rc=.res))

MAIN_DEPENDENCIES = $(MAIN_FILE:.cpp=.o)
ifeq "$(MAIN_DEPENDENCIES)" "$(MAIN_FILE)"
MAIN_DEPENDENCIES = $(MAIN_FILE:.c=.o)
endif

LLIB_DEPENDENCIES = $(notdir $(filter-out $(addsuffix .o,$(EXCLUDE)) $(MAIN_DEPENDENCIES), $(CPP_deps) $(C_deps) $(ASM_deps) $(TASM_deps)))


ifneq "$(OCCPR)" ""
CC=$(COMPILER_PATH)\bin\occpr /z$(ORANGEC)\include
else
CC=$(COMPILER_PATH)\bin\occ
endif

ifneq "$(WITHDEBUG)" ""
DEBUGFLAG := /g
endif

ifeq "$(VIAASSEMBLY)" ""
CCFLAGS = /c /E- /! $(DEBUGFLAG)
else
CCFLAGS = /S /E- /!
endif

ifeq "$(STD)" ""
STD := --std:c++17
endif

LINK=$(COMPILER_PATH)\bin\olink
LFLAGS=-c -mx /L$(_LIBDIR) /! $(DEBUGFLAG) $(MEMFLAGS)

LIB=$(COMPILER_PATH)\bin\olib
LIB_EXT:=.l
LIB_PREFIX:=
LIBFLAGS= /!

ASM=$(COMPILER_PATH)\bin\oasm

ASM=oasm
ASMFLAGS= /!

RC=$(COMPILER_PATH)\bin\orc
RCINCLUDE=$(DISTROOT)\include
	RCFLAGS = -r /!

ifneq "$(INCLUDES)" ""
CINCLUDES:=$(addprefix /I,$(INCLUDES))
endif
DEFINES := NDEBUG WIN32_LEAN_AND_MEAN TARGET_OS_WINDOWS $(DEFINES)
DEFINES := $(addprefix /D,$(DEFINES))
DEFINES := $(subst @, ,$(DEFINES))
LIB_DEPENDENCIES := $(foreach file, $(addsuffix .l,$(LIB_DEPENDENCIES)), $(file))

ifeq "$(TARGET)" "GUI"
ifeq "$(LSCRTL)" ""
STARTUP=C0pe.o
TYPE=/T:GUI32
COMPLIB=clwin$(LIB_EXT) climp$(LIB_EXT)
else
STARTUP=C0ls.o
TYPE=/T:GUI32
COMPLIB=lscrtl$(LIB_EXT) climp$(LIB_EXT)
CCFLAGS:=$(CCFLAGS) /Wgl
endif
else
ifeq "$(LSCRTL)" ""
STARTUP=C0Xpe.o
TYPE=/T:CON32
COMPLIB=clwin$(LIB_EXT) climp$(LIB_EXT)
else
STARTUP=C0xls.o
TYPE=/T:CON32
COMPLIB=lscrtl$(LIB_EXT) climp$(LIB_EXT)
CCFLAGS:=$(CCFLAGS) /Wcl
endif
endif
CFLAGS := $(CCFLAGS) $(CINCLUDES) $(DEFINES)
CXXFLAGS := $(CCFLAGS) $(CINCLUDES) $(DEFINES) $(STD)

vpath %.o $(_OUTPUTDIR)
vpath %.l $(_LIBDIR)
vpath %.res $(_OUTPUTDIR)

ifneq "$(OCCPR)" ""

%.c: %.l #override the builtin rule for mingw64

%.o: %.cpp
	$(CC) $(CXXFLAGS) -o$(_OUTPUTDIR)/temp.ods $^
	echo "" > $(_OUTPUTDIR)/$@

%.o: %.c
	$(CC) $(CFLAGS) -o$(_OUTPUTDIR)/temp.ods $^
	echo "" > $(_OUTPUTDIR)/$@

else
ifeq "$(VIAASSEMBLY)" ""
%.o: %.cpp
	$(CC) $(CXXFLAGS) -o$(_OUTPUTDIR)/$@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o$(_OUTPUTDIR)/$@ $^

else
%.o: %.cpp
	$(CC) $(CXXFLAGS) -o$(_OUTPUTDIR)/$*.s $^
	$(ASM) $(ASMFLAGS) -o$(_OUTPUTDIR)/$@ $(_OUTPUTDIR)/$*.s
%.o: %.c
	$(CC) $(CFLAGS) -o$(_OUTPUTDIR)/$*.s $^
	$(ASM) $(ASMFLAGS) -o$(_OUTPUTDIR)/$@ $(_OUTPUTDIR)/$*.s
endif
endif

%.o: %.s
	$(ASM) $(ASMFLAGS) -o$(_OUTPUTDIR)/$@ $^

%.res: %.rc
	$(RC) -i$(RCINCLUDE) $(RCFLAGS) -o$(_OUTPUTDIR)/$@ $^

$(_LIBDIR)\$(NAME)$(LIB_EXT): $(LLIB_DEPENDENCIES)
#	-del $(_LIBDIR)\$(NAME)$(LIB_EXT) >> $(NULLDEV)
ifeq "$(OCCPR)" ""
	$(LIB) $(LIBFLAGS) $(_LIBDIR)\$(NAME)$(LIB_EXT) $(addprefix +-$(_OUTPUTDIR)\,$(LLIB_DEPENDENCIES))
else
	echo "" > $(_LIBDIR)\$(NAME)$(LIB_EXT)
endif

$(NAME).exe: $(MAIN_DEPENDENCIES) $(LIB_DEPENDENCIES) $(_LIBDIR)\$(NAME)$(LIB_EXT) $(RES_deps)
ifeq "$(OCCPR)" ""
	$(LINK) /o$(NAME).exe $(TYPE) $(LFLAGS) $(STARTUP) $(addprefix $(_OUTPUTDIR)\,$(MAIN_DEPENDENCIES)) $(_LIBDIR)\$(NAME)$(LIB_EXT) $(LIB_DEPENDENCIES) $(COMPLIB) $(DEF_DEPENDENCIES) $(addprefix $(_OUTPUTDIR)\,$(RES_deps))
endif

ifeq "$(OCCPR)" ""
%.exe: %.c
	$(CC) -! -o$@ $^

%.exe: %.cpp
	$(CC) -! -o$@ $^
endif

endif