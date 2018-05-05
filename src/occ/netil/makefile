# Software License Agreement
# 
#     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
# 
#     This file is part of the Orange C Compiler package.
# 
#     The Orange C Compiler package is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version, with the addition of the 
#     Orange C "Target Code" exception.
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

NAME=occil
MAIN_FILE=ccmain.c
INCLUDES=..\parse ..\preproc ..\middle netlib\ ..\..\util 
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
    invoke.c \
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
    process.c \
    property.c \
    ppexpr.c \
    rtti.c \
    stmt.c \
    symtab.c \
    template.c \
    types.c \
    unmangle.c\
    winsystem.c \
    wseh.c
CPP_DEPENDENCIES=\
    using.cpp gen.cpp process.cpp import.cpp
LIB_DEPENDENCIES=util netlib

#determine if we can ignore the banner for a batch compile
WIN32 = WIN32
#use long long type
LL = USE_LONGLONG
DEFINES=$(i386) $(LL) $(MSDOS) $(WIN32)

SOURCE = ..\parse;..\preproc;..\middle

SRCPATH=$(BACKEND_INCLUDE_PATH);$(SOURCE);..;..\..\util
BACKEND_FILES = config.c

BACKEND_INCLUDE_PATH=.

vpath %.c $(SRCPATH);dll
vpath %.cs $(SRCPATH);dll

compiler: netlib redirect netlink lsmsilcrtl.dll occmsil testsqlite.exe

redirect:
	@$(MAKE) /T

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