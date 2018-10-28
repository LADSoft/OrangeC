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

include ../../pathext2.mak

NAME=occ
MAIN_FILE=ccmain.c
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
    unmangle.c \
    winsystem.c \
    wseh.c

ifeq "$(COMPILER)" "gcc-linux"
H_FILES=$(wildcard *.h) $(wildcard ../parse/*.h) $(wildcard ../middle/*.h) $(wildcard ../preproc/*.h)
else
H_FILES=$(wildcard *.h) $(wildcard ..$(PATHEXT2)parse$(PATHEXT2)*.h) $(wildcard ..$(PATHEXT2)middle$(PATHEXT2)*.h) $(wildcard ..$(PATHEXT2)preproc$(PATHEXT2)*.h)
endif

#LIB_DEPENDENCIES=util.lib objlib.lib
#determine if we can ignore the banner for a batch compile
#use long long type
LL = USE_LONGLONG
DEFINES=$(i386) $(LL) $(MSDOS)

BACKEND_INCLUDE_PATH=.

ifeq "$(COMPILER)" "gcc-linux"
SOURCE = ..$(PATHEXT2)parse:..$(PATHEXT2)preproc:..$(PATHEXT2)middle
INCLUDES=..$(PATHEXT2)parse ..$(PATHEXT2)preproc ..$(PATHEXT2)middle
SRCPATH=$(BACKEND_INCLUDE_PATH):$(SOURCE):..:..$(PATHEXT2)..$(PATHEXT2)util
else
SOURCE = ..$(PATHEXT2)parse;..$(PATHEXT2)preproc;..$(PATHEXT2)middle
INCLUDES=..$(PATHEXT2)parse ..$(PATHEXT2)preproc ..$(PATHEXT2)middle
SRCPATH=$(BACKEND_INCLUDE_PATH);$(SOURCE);..;..$(PATHEXT2)..$(PATHEXT2)util
endif
BACKEND_FILES = config.c gen.c inasm.c invoke.c objieeea.c outasm.c outcode.c peep.c rewrite.c


vpath %.c $(SRCPATH)


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

DISTRIBUTE: copyexe
	copy target.cfg $(DISTBIN)$(PATHEXT2)occ.cfg