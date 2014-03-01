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

BACKEND_FILES = config.obj

BACKEND_INCLUDE_PATH=.

LIBRARY:
EXEFILE: mkobj occpr.exe

mkobj:
	-mkdir $(OBJECT)

#compiler defines we need to take care of eventually
#set up stack/dta and handle the 8.3 names for include files
#MSDOS = -DMSDOS
#determine if we can ignore the banner for a batch compile
WIN32 = -DWIN32
#use long long type
LL = -DUSE_LONGLONG


OBJECT=objectb55
CC = bcc32 -c -v
CCFLAGS = /I$(SRCPATH) /DPARSER_ONLY $(i386) $(LL) $(MSDOS) $(WIN32) /DBORLAND
ASM = nasm -fobj
LIB = tlib32
LINK = ilink32 -v -m -Gn -Gi
SOURCE = ..\parse;..\preproc;..\middle

SRCPATH=$(BACKEND_INCLUDE_PATH);$(SOURCE);..\..\sqlite3;..

vpath %.c $(SRCPATH)
vpath %.obj $(OBJECT)

%.obj: %.c
	$(CC) $(CCFLAGS) -o$@ $^

#.path.c = .\;..\parse;..\preproc;..\middle

#		*List Macros*
EXE_dependencies = \
    ccmain.obj \
    sqlite3.obj \
    ccif.obj \
    db.obj \
    piper.obj \
    beinterf.obj \
    browse.obj \
    ccerr.obj \
    constopt.obj \
    cppbltin.obj \
    cpplookup.obj \
    debug.obj \
    declare.obj \
    declcons.obj \
    declcpp.obj \
    expr.obj \
    exprcpp.obj \
    float.obj \
    help.obj \
    init.obj \
    inline.obj \
    lambda.obj \
    lex.obj \
    list.obj \
    mangle.obj \
    memory.obj \
    osutil.obj \
    output.obj \
    preproc.obj \
    ppexpr.obj \
    rtti.obj \
    stmt.obj \
    symtab.obj \
    template.obj \
    types.obj \
    unmangle.obj

CC_RCFILE=occpr

#		*Explicit Rules*
occpr.exe: $(EXE_dependencies) $(BACKEND_FILES)
	$(LINK) $(LINKFLAGS)  @&&|
c0x32.obj $(addprefix $(OBJECT)\,$(EXE_dependencies) $(BACKEND_FILES)),occpr.exe,occ.map, cw32.lib import32.lib
|
#	$(PEPATCH) occpr.exe


DISTRIBUTE:
	copy $(RELEASEPATH)\occpr.exe $(DISTBIN)
	$(RESTUB) $(DISTBIN)\occpr.exe $(STUB)
#	upx $(DISTBIN)\occpr.exe
#	$(RENSEG) $(DISTBIN)\occpr.exe