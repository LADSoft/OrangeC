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

ifeq "$(COMPILER)" "OWC"

COMPILER_PATH := c:\watcom
OBJ_IND_PATH := watcom

PATH:=$(COMPILERPATH)\bin;$(PATH)
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

ifneq "$(RES_deps)" ""
RESOURCE_OPTION := , resource=$(addprefix $(_OUTPUTDIR)\,$(RES_deps))
endif

CC=$(COMPILER_PATH)\binnt\cl
CCFLAGS = -c

LINK=$(COMPILER_PATH)\binnt\wlink
LFLAGS=

LIB=$(COMPILER_PATH)\binnt\lib386
LIBFLAGS=
LIB_EXT:=.lib
LIB_PREFIX:=
TASM=$(COMPILER_PATH)\binnt\tasm32

ASM=nasm
ASMFLAGS = -fobj

RC=$(COMPILER_PATH)\binnt\rc.exe
RCINCLUDE=
RCFLAGS = -r

ifneq "$(INCLUDES)" ""
CINCLUDES:=$(addprefix -I,$(INCLUDES))
endif
DEFINES := $(addprefix -D,$(DEFINES))
DEFINES := $(subst -D","-D,$(DEFINES))
DEFINES := $(subst @, ,$(DEFINES))
LIB_DEPENDENCIES := $(foreach file, $(addsuffix .lib,$(LIB_DEPENDENCIES)), $(_LIBDIR)\$(file))

CCFLAGS := $(CCFLAGS) $(CINCLUDES) $(DEFINES) -DOPENWATCOM -DWIN32 -D_WIN32
ifeq "$(TARGET)" "GUI"
SYSTEM=nt_win
else
SYSTEM=nt
endif

COMPLIB=comctl32 gdi32 comdlg32 ole32 uxtheme kernel32 msimg32 shell32 user32 shfolder
vpath %.obj $(_OUTPUTDIR)
vpath %$(LIB_EXT) c:\bcc55\lib c:\bcc55\lib\psdk $(_LIBDIR)
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
#	-del $(_LIBDIR)\$(NAME)$(LIB_EXT) >> $(NULLDEV)
	$(LIB) $(LIBFLAGS) -out:$(_LIBDIR)\$(NAME)$(LIB_EXT) @&&|
 $(addprefix $(_OUTPUTDIR)\,$(LLIB_DEPENDENCIES))
|

FUDGE:=,

$(NAME).exe: $(MAIN_DEPENDENCIES) $(addprefix $(_LIBDIR)\,$(LIB_DEPENDENCIES)) $(_LIBDIR)\$(NAME)$(LIB_EXT) $(RES_deps)
	$(LINK) $(TYPE) $(LFLAGS) @&&|
system $(SYSTEM)
name .\$(NAME).exe
option map $(RESOURCE_OPTION)
import SetLayeredWindowAttributes_ USER32.SetLayeredWindowAttributes
import AllowSetForegroundWindow_ USER32.AllowSetForegroundWindow
import AddFontResourceExA_ gdi32.AddFontResourceExA
option stack=0x100000
file $(addprefix $(_OUTPUTDIR)\,$(MAIN_DEPENDENCIES))
library $(_LIBDIR)\$(NAME)$(LIB_EXT) $(addprefix $(FUDGE)$(_LIBDIR)\,$(LIB_DEPENDENCIES)) $(addprefix $(FUDGE),$(COMPLIB))
|


%.exe: %.cpp
	$(CC) $(CFLAGS) -Fo$*.obj $^
	$(LINK) $(TYPE) $(LFLAGS) @&&|
system nt
name .\$@
option map $(RESOURCE_OPTION)
import SetLayeredWindowAttributes_ USER32.SetLayeredWindowAttributes
import AllowSetForegroundWindow_ USER32.AllowSetForegroundWindow
import AddFontResourceExA_ gdi32.AddFontResourceExA
option stack=0x100000
file .\$*.obj
library comctl32 $(addprefix $(FUDGE),$(COMPLIB)
|

%.exe: %.c
	$(CC) $(CFLAGS) -Fo$*.obj $^
	$(LINK) $(TYPE) $(LFLAGS) @&&|
system nt
name .\$@
option map $(RESOURCE_OPTION)
import SetLayeredWindowAttributes_ USER32.SetLayeredWindowAttributes
import AllowSetForegroundWindow_ USER32.AllowSetForegroundWindow
import AddFontResourceExA_ gdi32.AddFontResourceExA
option stack=0x100000
file .\$*.obj
library comctl32 $(addprefix $(FUDGE),$(COMPLIB)
|

endif