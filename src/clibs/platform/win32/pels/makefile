#  Software License Agreement
#  
#      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

H_FILES = $(wildcard *.h)
C_DEPENDENCIES = $(wildcard *.c)
ASM_DEPENDENCIES = $(wildcard c0*.nas)

all: $(DEPENDENCIES) mklib
$(eval $(FORMATTER))

mklib: c0lsstartup.o
	$(LIB) $(LIBFLAGS) $(LSCRTLOBJECT)\lscrtl.l +- .\c0lsstartup.o $(CLIBOBJECT)\srproc.o $(WINOBJECT)\dllmain.o

c0lsstartup.o: c0lsstartup.c
	$(CC) /c /C-E $(CFLAGS) $(STDINCLUDE) /s16 /o$* $^

c0ls.o: c0ls.nas
	$(ASM) $(ASMFLAGS) -o$(LSCRTLOBJECT)\$@ $^
c0xls.o: c0xls.nas
	$(ASM) $(ASMFLAGS) -o$(LSCRTLOBJECT)\$@ $^
c0dls.o: c0dls.nas
	$(ASM) $(ASMFLAGS) -o$(LSCRTLOBJECT)\$@ $^

DISTRIBUTE:
	copy *.nas $(LSCRTLOBJECT)
	copy *.c $(LSCRTLOBJECT)
