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

all: crtdll.l
$(eval $(FORMATTER))

.\crtdmain.o: crtdmain.c
	$(CC) /c $(CFLAGS) $(STDINCLUDE) -o$@ $^

.PHONY: .\c0.o
.\c0.o: ..\pe\c0.c
	$(CC) /c $(CFLAGS) $(STDINCLUDE) -Wcc -o$@ $^

crtdll.l: .\crtdmain.o .\c0.o
	-del $@
	$(LIB) $(LIBFLAGS) $@ +- .\c0.o .\crtdmain.o
	$(LIB) $(LIBFLAGS) $@ +- $(CLIBOBJECT)\srproc.o $(CLIBOBJECT)\lxsar.o $(CLIBOBJECT)\lxshr.o $(CLIBOBJECT)\lxshl.o $(CLIBOBJECT)\lxmul.o $(CLIBOBJECT)\lxdiv.o $(CLIBOBJECT)\substk.o $(CLIBOBJECT)\stkchk.o $(CLIBOBJECT)\__fconvto.o $(CLIBOBJECT)\__fconvfrom.o $(CLIBOBJECT)\__fconst.o $(OBJECT)\vsconio.o $(OBJECT)\dllmain.o $(CLIBOBJECT)\fexcpt.o 
	$(IMPLIB) $@ .\crtdll.def
	move crtdll.l $(LSCRTLOBJECT)

DISTRIBUTE:
	copy *.c $(DISTSTARTUPWIN)

