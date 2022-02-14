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

ifeq "$(COMPILER)" "BCC32"

COMPILER_PATH := \bcc102
OBJ_IND_PATH := bcc32

CPP_deps = $(notdir $(CPP_DEPENDENCIES:.cpp=.obj))
C_deps = $(notdir $(C_DEPENDENCIES:.c=.obj))
ASM_deps = $(notdir $(ASM_DEPENDENCIES:.nas=.obj))
TASM_deps = $(notdir $(TASM_DEPENDENCIES:.asm=.obj))
RES_deps = $(notdir $(RC_DEPENDENCIES:.rc=.res))

MAIN_DEPENDENCIES = $(MAIN_FILE:.cpp=.obj)
ifeq "$(MAIN_DEPENDENCIES)" "$(MAIN_FILE)"
MAIN_DEPENDENCIES = $(MAIN_FILE:.c=.obj)
endif

LLIB_DEPENDENCIES = $(notdir $(filter-out $(addsuffix .obj,$(EXCLUDE)) $(MAIN_DEPENDENCIES), $(CPP_deps) $(C_deps) $(ASM_deps) $(TASM_deps)))


CC="$(COMPILER_PATH)\bin\bcc32c"
CCFLAGS = /c /w- /x /O2 /DBCC32c

LINK="$(COMPILER_PATH)\bin\ilink32"
LFLAGS=-c -m -Gn -Gi /V5.1 /L"$(COMPILER_PATH)\lib\win32c\release" /L"$(COMPILER_PATH)\lib\win32c\release\psdk" /L$(_LIBDIR)

LIB="$(COMPILER_PATH)\bin\tlib"
LIBFLAGS=/P1024
LIB_EXT:=.lib
LIB_PREFIX:=
TASM="$(COMPILER_PATH)\bin\\tasm32"

ASM=nasm
ASMFLAGS = -fobj

RC=$(DISTROOT)\src\orc
RCINCLUDE=$(DISTROOT)\include
RCFLAGS = -r

ifneq "$(INCLUDES)" ""
CINCLUDES:=$(addprefix /I,$(INCLUDES))
endif
DEFINES := $(addprefix /D,$(DEFINES))
DEFINES := $(subst @, ,$(DEFINES))
LIB_DEPENDENCIES := $(foreach file, $(addsuffix .lib,$(LIB_DEPENDENCIES)), $(_LIBDIR)\$(file))

CFLAGS := $(CCFLAGS) $(CINCLUDES) $(DEFINES) /DMICROSOFT /DBORLAND
CXXFLAGS := $(CCFLAGS) $(CINCLUDES) $(DEFINES) /DMICROSOFT /DBORLAND
ifeq "$(TARGET)" "GUI"
STARTUP=C0W32.obj
TYPE=/Tpe/aa
COMPLIB=cw32mt$(LIB_EXT)
else
STARTUP=C0X32.obj
TYPE=/Tpe/ap
COMPLIB=cw32mt$(LIB_EXT)
endif

COMPLIB:=$(COMPLIB) msimg32 shell32
vpath %.obj $(_OUTPUTDIR)
vpath %.lib $(_LIBDIR)
vpath %.res $(_OUTPUTDIR)

%.obj: %.cpp
	$(CC) $(CXXFLAGS) -o$(_OUTPUTDIR)/$@ $^

%.obj: %.c
	$(CC) $(CFLAGS) -o$(_OUTPUTDIR)/$@ $^

%.obj: %.asm
	$(TASM) /ml /zi /i$(INCLUDE) $(ADEFINES) $^, $(_OUTPUTDIR)/$@

%.obj: %.nas
	$(ASM) $(ASMFLAGS) -o$(_OUTPUTDIR)/$@ $^

%.res: %.rc
	$(RC) -i$(RCINCLUDE) $(RCFLAGS) -o$(_OUTPUTDIR)/$@ $^

$(_LIBDIR)\$(NAME)$(LIB_EXT): $(LLIB_DEPENDENCIES)
#	-del $(_LIBDIR)\$(NAME)$(LIB_EXT) 2> $(NULLDEV)
	$(LIB) $(LIBFLAGS) $(_LIBDIR)\$(NAME)$(LIB_EXT) @&&|
 $(addprefix -+$(_OUTPUTDIR)\,$(LLIB_DEPENDENCIES))
|

$(NAME).exe: $(MAIN_DEPENDENCIES) $(LIB_DEPENDENCIES) $(_LIBDIR)\$(NAME)$(LIB_EXT) $(NAME)$(LIB_EXT) $(RES_deps)
	$(LINK) $(TYPE) $(LFLAGS) $(STARTUP) $(addprefix $(_OUTPUTDIR)\,$(MAIN_DEPENDENCIES)), $(NAME), $(NAME), $(_LIBDIR)\$(NAME)$(LIB_EXT) $(LIB_DEPENDENCIES) $(COMPLIB) import32$(LIB_EXT), $(DEF_DEPENDENCIES), $(addprefix $(_OUTPUTDIR)\,$(RES_deps))

%.exe: %.c
	$(CC) -o$@ $^

%.exe: %.cpp
	$(CC) -o$@ $^

endif