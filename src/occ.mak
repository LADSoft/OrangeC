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

ifeq "$(COMPILER)" "OCC"

COMPILER_PATH := $(DISTROOT)
OBJ_IND_PATH := occ

CPP_deps = $(notdir $(CPP_DEPENDENCIES:.cpp=.o))
C_deps = $(notdir $(C_DEPENDENCIES:.c=.o))
ASM_deps = $(notdir $(ASM_DEPENDENCIES:.nas=.o))
TASM_deps = $(notdir $(TASM_DEPENDENCIES:.asm=.o))
RES_deps = $(notdir $(RC_DEPENDENCIES:.rc=.res))

MAIN_DEPENDENCIES = $(MAIN_FILE:.cpp=.o)
ifeq "$(MAIN_DEPENDENCIES)" "$(MAIN_FILE)"
MAIN_DEPENDENCIES = $(MAIN_FILE:.c=.o)
endif

LLIB_DEPENDENCIES = $(notdir $(filter-out $(addsuffix .o,$(EXCLUDE)) $(MAIN_DEPENDENCIES), $(CPP_deps) $(C_deps) $(ASM_deps) $(TASM_deps)))


CC=$(COMPILER_PATH)\bin\occ
CCFLAGS = /c /E- /!

LINK=$(COMPILER_PATH)\bin\olink
LFLAGS=-c -mx /L$(_LIBDIR) /!

LIB=$(COMPILER_PATH)\bin\olib
LIB_EXT:=.l
LIB_PREFIX:=
LIBFLAGS= /!

ASM=$(COMPILER_PATH)\bin\\oasm

ASM=oasm
ASMFLAGS= /!

RC=$(COMPILER_PATH)\bin\orc
RCINCLUDE=$(DISTROOT)\include
	RCFLAGS = -r /!

ifneq "$(INCLUDES)" ""
CINCLUDES:=$(addprefix /I,$(INCLUDES))
endif
DEFINES := $(addprefix /D,$(DEFINES))
DEFINES := $(subst @, ,$(DEFINES))
LIB_DEPENDENCIES := $(foreach file, $(addsuffix .l,$(LIB_DEPENDENCIES)), $(file))

$(info $(LIB_DEPENDENCIES))

CCFLAGS := $(CCFLAGS) $(CINCLUDES) $(DEFINES) /DMICROSOFT /DBORLAND /DWIN32
ifeq "$(TARGET)" "GUI"
STARTUP=C0pe.o
TYPE=/T:GUI32
COMPLIB=clwin$(LIB_EXT) climp$(LIB_EXT)
else
STARTUP=C0Xpe.o
TYPE=/T:CON32
COMPLIB=clwin$(LIB_EXT) climp$(LIB_EXT)
endif

vpath %.o $(_OUTPUTDIR)
vpath %$(LIB_EXT) $(DISTROOT)\lib $(_LIBDIR)
vpath %.res $(_OUTPUTDIR)

%.o: %.cpp
	$(CC) $(CCFLAGS) -o$(_OUTPUTDIR)/$@ $^

%.o: %.c
	$(CC) /9 $(CCFLAGS) -o$(_OUTPUTDIR)/$@ $^

%.o: %.nas
	$(ASM) $(ASMFLAGS) -o$(_OUTPUTDIR)/$@ $^

%.res: %.rc
	$(RC) -i$(RCINCLUDE) $(RCFLAGS) -o$(_OUTPUTDIR)/$@ $^

$(_LIBDIR)\$(NAME)$(LIB_EXT): $(LLIB_DEPENDENCIES)
#	-del $(_LIBDIR)\$(NAME)$(LIB_EXT) >> $(NULLDEV)
	$(LIB) $(LIBFLAGS) $(_LIBDIR)\$(NAME)$(LIB_EXT) $(addprefix +-$(_OUTPUTDIR)\,$(LLIB_DEPENDENCIES))

$(NAME).exe: $(MAIN_DEPENDENCIES) $(addprefix $(_LIBDIR)\,$(LIB_DEPENDENCIES)) $(_LIBDIR)\$(NAME)$(LIB_EXT) $(RES_deps)
	$(LINK) /o$(NAME).exe $(TYPE) $(LFLAGS) $(STARTUP) $(addprefix $(_OUTPUTDIR)\,$(MAIN_DEPENDENCIES)) $(_LIBDIR)\$(NAME)$(LIB_EXT) $(LIB_DEPENDENCIES) $(COMPLIB) $(DEF_DEPENDENCIES) $(addprefix $(_OUTPUTDIR)\,$(RES_deps))

%.exe: %.c
	$(CC) -o$@ $^

%.exe: %.cpp
	$(CC) -o$@ $^

endif