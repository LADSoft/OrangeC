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
CC=\bcc55\bin\bcc32
CFLAGS = /c /w- /RT- /O2 /v /I$(INCLUDE) $(DEFINES) /DMICROSOFT
LINK=\bcc55\bin\ilink32
LINKFLAGS=-v -c -m -Gn -Gi /V5.1 /Lc:\bcc55\lib;c:\bcc55\lib\psdk;$(OBJECT)

LIB=\bcc55\bin\tlib
LIBFLAGS=/P1024

TASM=tasm32

ASM=nasm
ASMFLAGS = -fobj

RC=orc
RCFLAGS = -r

vpath %.obj $(OBJECT)
vpath %.lib c:\bcc55\lib c:\bcc55\lib\psdk $(OBJECT)

%.obj: %.cpp
	$(CC) $(CFLAGS) -o$@ $^

%.obj: %.c
	$(CC) $(CFLAGS) -o$@ $^

%.obj: %.asm
	$(TASM) /ml /zi /i$(INCLUDE) $(ADEFINES) $^, $@

%.obj: %.nas
	$(ASM) $(ASMFLAGS) -o$@ $^

%.res: %.rc
	$(RC) $(RCFLAGS) -o$@ $^

CPP_deps = $(notdir $(CPP_DEPENDENCIES:.cpp=.obj))
C_deps = $(notdir $(C_DEPENDENCIES:.c=.obj))
ASM_deps = $(notdir $(ASM_DEPENDENCIES:.nas=.obj))
TASM_deps = $(notdir $(TASM_DEPENDENCIES:.asm=.obj))
RES_deps = $(notdir $(RC_DEPENDENCIES:.rc=.res))

MAIN_DEPENDENCIES = $(MAIN_FILE:.cpp=.obj)
ifeq "$(MAIN_DEPENDENCIES)" "$(MAIN_FILE)"
MAIN_DEPENDENCIES = $(MAIN_FILE:.c=.obj)
endif

LLIB_DEPENDENCIES = $(filter-out $(EXCLUDE) $(MAIN_DEPENDENCIES), $(CPP_deps) $(C_deps) $(ASM_deps) $(TASM_deps)) 

$(OBJECT)\$(NAME).lib: $(LLIB_DEPENDENCIES)
	-del $(OBJECT)\$(NAME).lib
	$(LIB) $(LIBFLAGS) $(OBJECT)\$(NAME).lib @&&|
+ $(addprefix +$(OBJECT)\,$(LLIB_DEPENDENCIES))
|
ifeq "$(TARGET)" "GUI"
STARTUP=C0W32.obj
TYPE=/Tpe/aa
COMPLIB=cw32mt.lib
else
STARTUP=C0X32.obj
TYPE=/Tpe/ap
COMPLIB=cw32.lib
endif

$(NAME).exe: $(MAIN_DEPENDENCIES) $(OBJ_DEPENDENCIES) $(OBJECT)\$(NAME).lib $(LIB_DEPENDENCIES) $(RES_deps)
	$(LINK) $(TYPE) $(LINKFLAGS) @&&|
$(STARTUP) $(OBJ_DEPENDENCIES) $(MAIN_DEPENDENCIES) $(LLIB_DEPENDENCIES)
$(NAME)
$(NAME)
$(LIB_DEPENDENCIES) $(COMPLIB) import32.lib
$(DEF_DEPENDENCIES)
$(RES_deps)
|

include $(DISTMAKE)