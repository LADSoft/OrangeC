# Software License Agreement
# 
#     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
# 
#     This file is part of the Orange C Compiler package.
# 
#     The Orange C Compiler package is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
# 
#     The Orange C Compiler package is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
# 
#     You should have received a copy of the GNU General Public License
#     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
# 
#     contact information:
#         email: TouchStone222@runbox.com <David Lindauer>
# 

include ../../pathext2.mak

NAME=occil
MAIN_FILE=ccmain.cpp
INCLUDES=..\parse ..\preproc ..\middle netlib\ ..\..\util 

C_DEPENDENCIES =\
    winsystem.c

CPP_DEPENDENCIES= \
    $(BACKEND_FILES) \
    ccmain.cpp \
    crc.cpp \
    beinterf.cpp \
    browse.cpp \
    ccerr.cpp \
    constopt.cpp \
    cppbltin.cpp \
    cpplookup.cpp \
    debug.cpp \
    declare.cpp \
    declcons.cpp \
    declcpp.cpp \
    expr.cpp \
    exprcpp.cpp \
    help.cpp \
    ialias.cpp \
    iblock.cpp \
    iconfl.cpp \
    iconst.cpp \
    iexpr.cpp \
    iflow.cpp \
    iinline.cpp \
    iinvar.cpp \
    ilazy.cpp \
    ilive.cpp \
    ilocal.cpp \
    iloop.cpp \
    init.cpp \
    inline.cpp \
    invoke.cpp \
    ioptutil.cpp \
    iout.cpp \
    ipeep.cpp \
    irc.cpp \
    ireshape.cpp \
    irewrite.cpp \
    issa.cpp \
    istmt.cpp \
    istren.cpp \
    lambda.cpp \
    lex.cpp \
    libcxx.cpp \
    list.cpp \
    mangle.cpp \
    memory.cpp \
    osutil.cpp \
    output.cpp \
    preproc.cpp \
    process.cpp \
    property.cpp \
    ppexpr.cpp \
    rtti.cpp \
    stmt.cpp \
    symtab.cpp \
    template.cpp \
    types.cpp \
    unmangle.cpp \
    wseh.cpp \
    using.cpp \
    gen.cpp \
    process.cpp \
    import.cpp
LIB_DEPENDENCIES=util netlib

#determine if we can ignore the banner for a batch compile
WIN32 = WIN32
#use long long type
LL = USE_LONGLONG
DEFINES=$(i386) $(LL) $(MSDOS) $(WIN32)

SOURCE = ..\parse;..\preproc;..\middle

SRCPATH=$(BACKEND_INCLUDE_PATH);$(SOURCE);..;..\..\util
BACKEND_FILES = config.cpp

BACKEND_INCLUDE_PATH=.

vpath %.c $(SRCPATH);dll
vpath %.cs $(SRCPATH);dll
vpath %.cpp $(SRCPATH);dll
compiler: netlib redirect netlink lsmsilcrtl.dll occmsil testsqlite.exe

ifndef _TREEROOT
_TREEROOT := $(CURDIR)$(PATHEXT2)..$(PATHEXT2)..$(PATHEXT2)treetop.mak
_TREETARGET := $(CURDIR)
export _TREEROOT
export _TREETARGET

redirect:
	@$(MAKE) -f$(_TREEROOT)
format:
	@$(MAKE) -f $(_TREEROOT) format
endif

occmsil: occmsil.c
	-del occmsil.dll
	occ /Wd occmsil.c

lsmsilcrtl.dll: lsmsilcrtl.cs
	csc /unsafe /keyfile:..\occil\lsmsilcrtl\occil.snk /t:library ..\occil\lsmsilcrtl\lsmsilcrtl.cs ..\occil\lsmsilcrtl\properties\assemblyinfo.cs

netlib:
	@$(MAKE) /Cnetlib

netlink:
	@$(MAKE) /Cnetlib\netlink

testsqlite.exe: zzz.dll sqlite3.dll sqlite3\testsqlite.cs
	csc /unsafe /r:lsmsilcrtl.dll;zzz.dll;sqlite3.dll /platform:x86 sqlite3\testsqlite.cs 

zzz.dll: sqlite3\zzz.c
	occil /Wd sqlite3\zzz.c /Nzzz.ar

sqlite3.dll: sqlite3\sqlite3.c
	occil /9 /Wd /Lkernel32 sqlite3\sqlite3.c /Nsqlite3.sqlite

DISTRIBUTE: copyexe
	copy target.cfg $(DISTBIN)\occ.cfg