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

H_FILES = $(wildcard *.h)

SHELL=cmd.exe

EXCLUDE := $(EXCLUDE) .]lscrtl.dll

RCFILE=lscrtl

all: $(DEPENDENCIES) .\c0dll.o .\lscrtl.dll

$(eval $(FORMATTER))

ifneq ($(shell type makefile),)
B = /b
endif

.\lscrtl.dll: .\c0dll.o $(RCFILE).res
	-del $@
	-del .\lscrtl.l
	$(LINK) $(LINKFLAGS) /mx /T:DLL32 /DIMAGEBASE=0x10800000 /DFIXED=1 -o$@ .\c0dll.o $(WINOBJECT)\* $(CLIBOBJECT)\*.o $(SYSOBJECT)\climp.l $(RCFILE).res
	move $@ $(LSCRTLOBJECT)

.\c0dll.o: c0dll.c
	$(CC) /c /C-E $(CFLAGS) $(STDINCLUDE) -o$@ $^

$(RCFILE).res: $(RCFILE).rc ..\..\..\..\version.h
	echo $(CURDIR)
	orc -i..\inc $(RCFILE).rc

DISTRIBUTE: