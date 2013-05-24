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

#compiler defines we need to take care of eventually
#set up stack/dta and handle the 8.3 names for include files
#MSDOS = -DMSDOS
#determine if we can ignore the banner for a batch compile
WIN32 = -DWIN32
#use long long type
LL = -DUSE_LONGLONG


OBJECT=object
CC = cc386 
CCFLAGS = /C+d /c /O- /9 /I$(SRCPATH) $(i386) $(LL) $(MSDOS) $(WIN32)
ASM = nasm -fobj
LIB = xlib
LINK = valx -use32 -nocaseignore -Map
SOURCE = ..\parse;..\preproc;..\middle

SRCPATH=$(BACKEND_INCLUDE_PATH);$(SOURCE);..

vpath %.c $(SRCPATH)
vpath %.obj $(OBJECT)

%.obj: %.c
	$(CC) $(CCFLAGS) -o$@ $^

#.path.c = .\;..\parse;..\preproc;..\middle

#		*List Macros*
EXE_dependencies = \
    ccmain.obj \
    beinterf.obj \
    browse.obj \
    ccerr.obj \
    constopt.obj \
    debug.obj \
    declare.obj \
    expr.obj \
    float.obj \
    help.obj \
    ialias.obj \
    iblock.obj \
    iconfl.obj \
    iconst.obj \
    iexpr.obj \
    iflow.obj \
    iinvar.obj \
    ilazy.obj \
    ilive.obj \
    ilocal.obj \
    iloop.obj \
    init.obj \
    inline.obj \
    ioptutil.obj \
    iout.obj \
    ipeep.obj \
    irc.obj \
    ireshape.obj \
    irewrite.obj \
    issa.obj \
    istmt.obj \
    istren.obj \
    lex.obj \
    list.obj \
    mangle.obj \
    memory.obj \
    osutil.obj \
    output.obj \
    preproc.obj \
    ppexpr.obj \
    stmt.obj \
    symtab.obj \
    types.obj \
    unmangle.obj

CC_RCFILE=cc386

CPP_dependencies = \
	ppmain.obj \
	osutil.obj \
	memory.obj \
	symtab.obj \
	ccerr.obj \
	preproc.obj \
	ppexpr.obj	

CPP_RCFILE=cpp386

LIBRARY:
EXEFILE: occ.exe

#		*Explicit Rules*
occ.exe: $(EXE_dependencies) $(BACKEND_FILES)
	$(LINK) -use32 -nocaseignore -Map -PE -CON @&&|
c0xwin.obj $(addprefix $(OBJECT)\,$(EXE_dependencies) $(BACKEND_FILES)),occ.exe,occ.map,clwin.lib climp.lib,,,
|

DISTRIBUTE:
	copy occ.exe $(DISTBIN)
	$(RESTUB) $(DISTBIN)\occ.exe $(STUB)
	upx $(DISTBIN)\occ.exe
	copy target.cfg $(DISTBIN)\occ.cfg
	$(RENSEG) $(DISTBIN)\occ.exe