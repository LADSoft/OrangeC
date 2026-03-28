#  Software License Agreement
#  
#      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
#  
#      This file is part of the Orange C Compiler package.
#  
#      The Orange C Compiler package is free software: you can redistribute it and/or modify
#      it under the terms of the GNU General Public License as published by
#      the Free Software Foundation, either version 3 of the License, or
#      (at your option) any later version.
#  
#      The Orange C Compiler package is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#      GNU General Public License for more details.
#  
#      You should have received a copy of the GNU General Public License
#      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
#  
#      contact information:
#          email: TouchStone222@runbox.com <David Lindauer>
#  

SHELL=cmd.exe

EXCLUDE = $(EXCLUDE) crtdll.l
H_FILES = $(wildcard *.h)
C_DEPENDENCIES = $(wildcard *.c)

all: crtdll.l c0Objects

include ..\c0\c0.mak

$(eval $(FORMATTER))

crtdll.l: c0Objects
	-del $@
	$(LIB) $(LIBFLAGS) $@ +- $(C0OBJECTS) $(WINOBJECT)\wargset.o $(WINOBJECT)\wenvset.o $(WINOBJECT)\llwgenv.o $(C0OBJECTS)
	$(LIB) $(LIBFLAGS) $@ +- $(CLIBOBJECT)\srproc.o $(CLIBOBJECT)\lxsar.o $(CLIBOBJECT)\lxshr.o $(CLIBOBJECT)\lxshl.o $(CLIBOBJECT)\lxmul.o $(CLIBOBJECT)\lxdiv.o $(CLIBOBJECT)\substk.o $(CLIBOBJECT)\stkchk.o $(CLIBOBJECT)\__fconvto.o $(CLIBOBJECT)\__fconvfrom.o $(CLIBOBJECT)\__fconst.o $(OBJECT)\vsconio.o $(OBJECT)\dllmain.o $(CLIBOBJECT)\fexcpt.o 
	$(IMPLIB) $@ .\crtdll.def
	move $@ $(SYSOBJECT)

DISTRIBUTE: