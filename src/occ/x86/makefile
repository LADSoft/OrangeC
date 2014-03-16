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

BACKEND_FILES = config$(OBJ) dbgieee$(OBJ) gen$(OBJ) inasm$(OBJ) invoke$(OBJ) objieeea$(OBJ) outasm$(OBJ) outcode$(OBJ) peep$(OBJ) rewrite$(OBJ)

BACKEND_INCLUDE_PATH=.

LIBRARY:
EXEFILE: mkobj occ.exe

mkobj:
	-mkdir $(OBJECT)

#compiler defines we need to take care of eventually
#set up stack/dta and handle the 8.3 names for include files
#MSDOS = -DMSDOS
#determine if we can ignore the banner for a batch compile
WIN32 = -DWIN32
#use long long type
LL = -DUSE_LONGLONG

#VIAOCC=1
ifeq "$(VIAOCC)" "1"
OBJECT=objectocc
OBJ=.o
MYCC = \orangec\bin\occ /c
MYCCFLAGS = /9 /I$(SRCPATH) $(i386) $(LL) $(MSDOS) $(WIN32) /DMICROSOFT
MYASM = \orangec\bin\oasm
MYLINK= \orangec\bin\olink /v /c /T:CON32 /m 
MYLIB = \orangec\bin\olib
else
OBJECT=objectb55
OBJ=.obj
MYCCFLAGS = /I$(SRCPATH) $(i386) $(LL) $(MSDOS) $(WIN32) /DBORLAND /DMICROSOFT
MYCC = c:\bcc55\bin\bcc32 -c -v
MYASM = nasm -fobj
MYLIB = c:\bcc55\bin\tlib32
MYLINK = c:\bcc55\bin\ilink32 -Gn -Gi -v -m
endif
SOURCE = ..\parse;..\preproc;..\middle

SRCPATH=$(BACKEND_INCLUDE_PATH);$(SOURCE);..

vpath %.c $(SRCPATH)
vpath %.o $(OBJECT)
vpath %.obj $(OBJECT)

%$(OBJ): %.c
	$(MYCC) $(MYCCFLAGS) -o$@ $^

#.path.c = .\;..\parse;..\preproc;..\middle

#		*List Macros*
EXE_dependencies = \
    ccmain$(OBJ) \
    beinterf$(OBJ) \
    browse$(OBJ) \
    ccerr$(OBJ) \
    constopt$(OBJ) \
    cppbltin$(OBJ) \
    cpplookup$(OBJ) \
    debug$(OBJ) \
    declare$(OBJ) \
    declcons$(OBJ) \
    declcpp$(OBJ) \
    expr$(OBJ) \
    exprcpp$(OBJ) \
    float$(OBJ) \
    help$(OBJ) \
    ialias$(OBJ) \
    iblock$(OBJ) \
    iconfl$(OBJ) \
    iconst$(OBJ) \
    iexpr$(OBJ) \
    iflow$(OBJ) \
    iinvar$(OBJ) \
    ilazy$(OBJ) \
    ilive$(OBJ) \
    ilocal$(OBJ) \
    iloop$(OBJ) \
    init$(OBJ) \
    inline$(OBJ) \
    ioptutil$(OBJ) \
    iout$(OBJ) \
    ipeep$(OBJ) \
    irc$(OBJ) \
    ireshape$(OBJ) \
    irewrite$(OBJ) \
    issa$(OBJ) \
    istmt$(OBJ) \
    istren$(OBJ) \
    lambda$(OBJ) \
    lex$(OBJ) \
    list$(OBJ) \
    mangle$(OBJ) \
    memory$(OBJ) \
    osutil$(OBJ) \
    output$(OBJ) \
    preproc$(OBJ) \
    ppexpr$(OBJ) \
    rtti$(OBJ) \
    stmt$(OBJ) \
    symtab$(OBJ) \
    template$(OBJ) \
    types$(OBJ) \
    unmangle$(OBJ)\
    winsystem$(OBJ)

CC_RCFILE=cc386

CPP_dependencies = \
	ppmain$(OBJ) \
	osutil$(OBJ) \
	memory$(OBJ) \
	symtab$(OBJ) \
	ccerr$(OBJ) \
	preproc$(OBJ) \
	ppexpr$(OBJ)	

CPP_RCFILE=cpp386

winsystem$(OBJ): ..\..\util\winsystem.c
	$(MYCC) $(MYCCFLAGS) -o$@ $^

#		*Explicit Rules*
ifeq "$(VIAOCC)" "1"
occ.exe: $(EXE_dependencies) $(BACKEND_FILES)
	$(MYLINK) /oocc.exe @&&|
c0xpe.o $(addprefix $(OBJECT)\,$(EXE_dependencies) $(BACKEND_FILES)) clwin.l climp.l
|
else
occ.exe: $(EXE_dependencies) $(BACKEND_FILES)
	$(MYLINK) $(LINKFLAGS)  @&&|
c0x32.obj $(addprefix $(OBJECT)\,$(EXE_dependencies) $(BACKEND_FILES)),occ.exe,occ.map, cw32.lib import32.lib
|
endif


include $(CPPMAKE)

DISTRIBUTE:
	copy $(RELEASEPATH)\occ.exe $(DISTBIN)
	$(RESTUB) $(DISTBIN)\occ.exe $(STUB)
#	upx $(DISTBIN)\occ.exe
#	$(RENSEG) $(DISTBIN)\occ.exe
	copy target.cfg $(DISTBIN)\occ.cfg
