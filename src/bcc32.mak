#	Software License Agreement (BSD License)
#	
#	Copyright (c) 1997-2009, David Lindauer, (LADSoft).
#	All rights reserved.
#	
#	Redistribution and use of this software in source and binary forms, 
#	with or without modification, are permitted provided that the following 
#	conditions are met:

#	* Redistributions of source code must retain the above
#	  copyright notice, this list of conditions and the
#	  following disclaimer.

#	* Redistributions in binary form must reproduce the above
#	  copyright notice, this list of conditions and the
#	  following disclaimer in the documentation and/or other
#	  materials provided with the distribution.

#	* Neither the name of LADSoft nor the names of its
#	  contributors may be used to endorse or promote products
#	  derived from this software without specific prior
#	  written permission of LADSoft.

#	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
#	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
#	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
#	PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
#	OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
#	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
#	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
#	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
#	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
#	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#	contact information:
#		email: TouchStone222@runbox.com <David Lindauer>

# this file for the embarcadero version. it won't work with BCC55 because of
# use of C++11 constructs in the sources.
ifeq "$(COMPILER)" "BCC32"

RADSTUDIO_BASE := c:\program files (x86)\Embarcadero\Studio
RADSTUDIO_VERSION := $(lastword $(shell dir /b "$(RADSTUDIO_BASE)"))
COMPILER_PATH := $(RADSTUDIO_BASE)\$(RADSTUDIO_VERSION)
OBJ_IND_PATH := bcc32

ifeq "$(RADSTUDIO_VERSION)" ""
$(error "Please install Embarcadero RadStudio")
endif

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
CCFLAGS = /c /w- /RT- /O2 /v  

LINK="$(COMPILER_PATH)\bin\ilink32"
LFLAGS=-v -c -m -Gn -Gi /V5.1 /L"$(COMPILER_PATH)\lib\win32c\release";"$(COMPILER_PATH)\lib\win32c\release\psdk";$(_LIBDIR)

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
LIB_DEPENDENCIES := $(addsuffix .lib,$(LIB_DEPENDENCIES))

CCFLAGS := $(CCFLAGS) $(CINCLUDES) $(DEFINES) /DMICROSOFT /DBORLAND /DWIN32
ifeq "$(TARGET)" "GUI"
STARTUP=C0W32.obj
TYPE=/Tpe/aa
COMPLIB=cw32mt$(LIB_EXT)
else
STARTUP=C0X32.obj
TYPE=/Tpe/ap
COMPLIB=cw32$(LIB_EXT)
endif

COMPLIB:=$(COMPLIB) msimg32 shell32
vpath %.obj $(_OUTPUTDIR)
vpath %$(LIB_EXT) c:\bcc55\lib c:\bcc55\lib\psdk $(_LIBDIR)
vpath %.res $(_OUTPUTDIR)

%.obj: %.cpp
	$(CC) $(CCFLAGS) -o$@ $^

%.obj: %.c
	$(CC) $(CCFLAGS) -o$@ $^

%.obj: %.asm
	$(TASM) /ml /zi /i$(INCLUDE) $(ADEFINES) $^, $@

%.obj: %.nas
	$(ASM) $(ASMFLAGS) -o$@ $^

%.res: %.rc
	$(RC) -i$(RCINCLUDE) $(RCFLAGS) -o$@ $^

$(_LIBDIR)\$(NAME)$(LIB_EXT): $(LLIB_DEPENDENCIES)
#	-del $(_LIBDIR)\$(NAME)$(LIB_EXT) >> $(NULLDEV)
	$(LIB) $(LIBFLAGS) $(_LIBDIR)\$(NAME)$(LIB_EXT) @&&|
 $(addprefix -+$(_OUTPUTDIR)\,$(LLIB_DEPENDENCIES))
|

$(NAME).exe: $(MAIN_DEPENDENCIES) $(addprefix $(_LIBDIR)\,$(LIB_DEPENDENCIES)) $(_LIBDIR)\$(NAME)$(LIB_EXT) $(RES_deps)
	$(LINK) $(TYPE) $(LFLAGS) @&&|
$(STARTUP) $(addprefix $(_OUTPUTDIR)\,$(MAIN_DEPENDENCIES))
$(NAME)
$(NAME)
$(_LIBDIR)\$(NAME)$(LIB_EXT) $(LIB_DEPENDENCIES) $(COMPLIB) import32$(LIB_EXT)
$(DEF_DEPENDENCIES)
$(addprefix $(_OUTPUTDIR)\,$(RES_deps))
|

%.exe: %.c
	$(CC) -o$@ $^

%.exe: %.cpp
	$(CC) -o$@ $^

endif