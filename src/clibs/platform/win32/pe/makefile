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

EXCLUDE := $(EXCLUDE) clwin.l

H_FILES = $(wildcard *.h)


all: $(DEPENDENCIES) .\clwin.l ./dfstb32.exe

$(eval $(FORMATTER))

include ..\c0\c0.mak

./wildargsw.o: wildargsw.c
	$(CC) /c $(STDINCLUDE) $(CFLAGS) -o$(SYSOBJECT)\$@ $^
.\dfstb32.o: dfstb32.nas
	$(ASM) $(ASMFLAGS) -o$@ $^

.\clwin.l: c0Objects
	-$(DEL) $@
	$(LIB) $(LIBFLAGS) $@ +- $(C0OBJECTS) $(WINOBJECT)\*.o $(CLIBOBJECT)\*.o
	move $@ $(SYSOBJECT)

dfstb32.exe: .\dfstb32.o
	$(LINK) $(LINKFLAGS) /T:REAL -odfstb32 $^
	
DISTRIBUTE:
