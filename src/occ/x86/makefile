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
NAME=occ
MAIN_FILE=ccmain.c
INCLUDES=..\parse ..\preproc ..\middle
C_DEPENDENCIES= \
    $(BACKEND_FILES) \
    ccmain.c \
    crc.c \
    beinterf.c \
    browse.c \
    ccerr.c \
    constopt.c \
    cppbltin.c \
    cpplookup.c \
    debug.c \
    declare.c \
    declcons.c \
    declcpp.c \
    expr.c \
    exprcpp.c \
    float.c \
    help.c \
    ialias.c \
    iblock.c \
    iconfl.c \
    iconst.c \
    iexpr.c \
    iflow.c \
    iinline.c \
    iinvar.c \
    ilazy.c \
    ilive.c \
    ilocal.c \
    iloop.c \
    init.c \
    inline.c \
    ioptutil.c \
    iout.c \
    ipeep.c \
    irc.c \
    ireshape.c \
    irewrite.c \
    issa.c \
    istmt.c \
    istren.c \
    lambda.c \
    lex.c \
    libcxx.c \
    list.c \
    mangle.c \
    memory.c \
    osutil.c \
    output.c \
    preproc.c \
    property.c \
    ppexpr.c \
    rtti.c \
    stmt.c \
    symtab.c \
    template.c \
    types.c \
    unmangle.c\
    winsystem.c
#LIB_DEPENDENCIES=util.lib objlib.lib
#determine if we can ignore the banner for a batch compile
WIN32 = WIN32
#use long long type
LL = USE_LONGLONG
DEFINES=$(i386) $(LL) $(MSDOS) $(WIN32)

SOURCE = ..\parse;..\preproc;..\middle

SRCPATH=$(BACKEND_INCLUDE_PATH);$(SOURCE);..;..\..\util
BACKEND_FILES = config.c dbgieee.c gen.c inasm.c invoke.c objieeea.c outasm.c outcode.c peep.c rewrite.c

BACKEND_INCLUDE_PATH=.

vpath %.c $(SRCPATH)

redirect:
	@$(MAKE) /T

DISTRIBUTE: copyexe
	copy target.cfg $(DISTBIN)\occ.cfg