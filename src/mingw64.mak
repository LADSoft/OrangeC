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

ifeq "$(COMPILER)" "MINGW64"

COMPILER_PATH := c:\mingw64
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


CC=$(COMPILER_PATH)\bin\x86_64-w64-mingw32-gcc
CCFLAGS = -c -std=c++11 -D__MSVCRT__ -U__STRICT_ANSI__

LINK=$(COMPILER_PATH)\bin\ld
LFLAGS=-L$(_LIBDIR)

LIB=$(COMPILER_PATH)\bin\ar
LIBFLAGS=-r -s
LIB_EXT:=.a
LIB_PREFIX:=lib

ASM=$(COMPILER_PATH)\bin\as
ASMFLAGS=

RC=$(COMPILER_PATH)\bin\windres
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

COMPLIB=-lstdc++ -lcomctl32 -lgdi32 -lcomdlg32 -lole32 -luxtheme -lkernel32 -lmsimg32 -luuid


vpath %.o $(_OUTPUTDIR)
vpath %$(LIB_EXT) c:\bcc55\lib c:\bcc55\lib\psdk $(_LIBDIR)
vpath %.res $(_OUTPUTDIR)

%.o: %.cpp
	$(CC) $(CCFLAGS) -o$(_OUTPUTDIR)/$@ $^

%.o: %.c
	$(CC) $(CCFLAGS) -o$(_OUTPUTDIR)/$@ $^

%.o: %.s
	$(ASM) $(ASMFLAGS) -o$(_OUTPUTDIR)/$@ $^

%.o: %.rc
	$(RC) $(RCFLAGS) -i $^ -o $(_OUTPUTDIR)/$@

$(_LIBDIR)\$(LIB_PREFIX)$(NAME)$(LIB_EXT): $(LLIB_DEPENDENCIES)
#	-del $(_LIBDIR)\$(LIB_PREFIX)$(NAME)$(LIB_EXT) >> $(NULLDEV)
	$(LIB) $(LIBFLAGS) $(_LIBDIR)\$(LIB_PREFIX)$(NAME)$(LIB_EXT) $(addprefix $(subst \,/,$(_OUTPUTDIR)\),$(LLIB_DEPENDENCIES))

LDEPS := $(addprefix -l,$(NAME) $(LIB_DEPENDENCIES))
LDEPS := $(subst \,/,$(LDEPS))

LDEPS2 := $(addprefix $(_LIBDIR)\$(LIB_PREFIX),$(NAME) $(LIB_DEPENDENCIES))
LDEPS2 := $(addsuffix .a, $(LDEPS2))

LMAIN := $(addprefix $(_OUTPUTDIR)\,$(MAIN_DEPENDENCIES) $(RES_deps))
LMAIN := $(subst \,/,$(LMAIN))

$(NAME).exe: $(MAIN_DEPENDENCIES) $(LDEPS2) $(RES_deps)
	$(CC) $(LFLAGS) -o $(NAME).exe $(LMAIN) $(LDEPS) $(COMPLIB) $(DEF_DEPENDENCIES)


%.exe: %.c
	$(CC) $(LFLAGS) -o $@ $^ $(COMPLIB)

%.exe: %.cpp
	$(CC) $(LFLAGS) -o $@ $^ $(COMPLIB)

endif