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

NAME=occpr
MAIN_FILE=ccmain.c
INCLUDES=..\parse ..\preproc ..\middle
C_DEPENDENCIES= \
    $(BACKEND_FILES) \
    ccmain.c \
    ccif.c \
    crc.c \
    db.c \
    piper.c \
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
    init.c \
    inline.c \
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
    unmangle.c \
    wseh.c
LIB_DEPENDENCIES=sqlite3
#determine if we can ignore the banner for a batch compile
WIN32 = WIN32
#use long long type
LL = USE_LONGLONG
DEFINES=$(i386) $(LL) $(MSDOS) $(WIN32) PARSER_ONLY 

SOURCE = ..\parse;..\preproc;..\middle

BACKEND_INCLUDE_PATH=.
SRCPATH:=$(BACKEND_INCLUDE_PATH);$(SOURCE);..
BACKEND_FILES = config.c


vpath %.c $(SRCPATH)

ifndef _TREEROOT
_TREEROOT := $(CURDIR)\..\..\treetop.mak
_TREETARGET := $(CURDIR)
export _TREEROOT
export _TREETARGET

redirect:
	@$(MAKE) /f$(_TREEROOT)
format:
	@$(MAKE) /f$(_TREEROOT) format
endif

DISTRIBUTE: copyexe
