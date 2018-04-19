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

ifeq "$(COMPILER)" "MS"

# you have to run vcvars32 before you can use this
#
# this path is subject to change, if you don't want to worry with it
# you can probably download and initialize the Windows SDK 10
# and the correct path will be picked up by the code below
UCRTPATH=C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10240.0\ucrt\x86

OBJ_IND_PATH := ms

CPP_deps = $(notdir $(CPP_DEPENDENCIES:.cpp=.obj))
C_deps = $(notdir $(C_DEPENDENCIES:.c=.obj))
ASM_deps = $(notdir $(ASM_DEPENDENCIES:.nas=.obj))
TASM_deps = $(notdir $(TASM_DEPENDENCIES:.asm=.obj))
RES_deps = $(notdir $(RC_DEPENDENCIES:.rc=.res))

MAIN_DEPENDENCIES = $(MAIN_FILE:.cpp=.obj)
ifeq "$(MAIN_DEPENDENCIES)" "$(MAIN_FILE)"
MAIN_DEPENDENCIES = $(MAIN_FILE:.c=.obj)
endif

LLIB_DEPENDENCIES := $(notdir $(filter-out $(addsuffix .obj,$(EXCLUDE)) $(MAIN_DEPENDENCIES), $(CPP_deps) $(C_deps) $(ASM_deps) $(TASM_deps)))


CC=cl.exe
CCFLAGS = /O2 /EHs /c /nologo

LINK=link.exe
LFLAGS=/LTCG:incremental /nologo /NXCOMPAT /DYNAMICBASE /MACHINE:x86 /OPT:REF /SAFESEH  /OPT:ICF /TLBID:1

LIBEXE=lib.exe
LIBFLAGS=/MACHINE:x86 /LTCG /nologo
LIB_EXT:=.lib
LIB_PREFIX:=
TASM=$(COMPILER_PATH)\bin\\tasm32

ASM=nasm
ASMFLAGS = -fwin32

RC=rc.exe
RCFLAGS = ./r

ifneq "$(INCLUDES)" ""
CINCLUDES:=$(addprefix /I,$(INCLUDES))
endif
DEFINES := $(addprefix /D,$(DEFINES))
DEFINES := $(subst @, ,$(DEFINES))
LIB_DEPENDENCIES := $(addsuffix $(LIB_EXT),$(LIB_DEPENDENCIES))

CCFLAGS := $(CCFLAGS) $(CINCLUDES) $(DEFINES) /DMICROSOFT /DWIN32

ifeq "$(TARGET)" "GUI"
LFLAGS:= $(LFLAGS) /SUBSYSTEM:WINDOWS
else
LFLAGS:= $(LFLAGS) /MANIFEST /SUBSYSTEM:CONSOLE /MANIFESTUAC:"level='asInvoker' uiAccess = 'false'"
endif

COMPLIB:=$(COMPLIB) "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "odbc32.lib" "odbccp32.lib" "uxtheme.lib" "comctl32.lib" "msimg32.lib"

vpath %.obj $(_OUTPUTDIR)
vpath %.lib $(_LIBDIR)
vpath %.res $(_OUTPUTDIR)

%.obj: %.cpp
	$(CC) $(CCFLAGS) -Fo$(_OUTPUTDIR)/$@ $^

%.obj: %.c
	$(CC) $(CCFLAGS) -Fo$(_OUTPUTDIR)/$@ $^

%.obj: %.asm
	$(TASM) /ml /zi /i$(INCLUDE) $(ADEFINES) $^, $(_OUTPUTDIR)/$@

%.obj: %.nas
	$(ASM) $(ASMFLAGS) -o$(_OUTPUTDIR)/$@ $^

%.res: %.rc
	$(RC) -i$(RCINCLUDE) $(RCFLAGS) -fo$(_OUTPUTDIR)/$@ $^

$(_LIBDIR)\$(NAME)$(LIB_EXT): $(LLIB_DEPENDENCIES)
#	-del $()_LIBDIR)\$(NAME)$(LIB_EXT) >> $(NULLDEV)
	$(LIBEXE) $(LIBFLAGS) /OUT:$(_LIBDIR)\$(NAME)$(LIB_EXT) $(addprefix $(_OUTPUTDIR)\,$(LLIB_DEPENDENCIES)) 


$(NAME).exe: $(MAIN_DEPENDENCIES) $(LIB_DEPENDENCIES) $(_LIBDIR)\$(NAME)$(LIB_EXT) $(RES_deps)
	$(LINK) $(TYPE) $(LFLAGS) $(COMPLIB) /LIBPATH:"$(UCRTPATH)" /LIBPATH:"$(VCINSTALLDIR)\lib" /OUT:$@ $(_LIBDIR)\$(NAME)$(LIB_EXT) $(LIB_DEPENDENCIES) $(addprefix /DEF:,$(DEF_DEPENDENCIES)) $(addprefix $(_OUTPUTDIR)\,$(RES_deps)) $(addprefix $(_OUTPUTDIR)\,$(MAIN_DEPENDENCIES))


%.exe: %.c
	$(CC) $^

%.exe: %.cpp
	$(CC) $^

endif