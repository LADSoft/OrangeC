# Software License Agreement
# 
#     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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
#     As a special exception, if other files instantiate templates or
#     use macros or inline functions from this file, or you compile
#     this file and link it with other works to produce a work based
#     on this file, this file does not by itself cause the resulting
#     work to be covered by the GNU General Public License. However
#     the source code for this file must still be made available in
#     accordance with section (3) of the GNU General Public License.
#     
#     This exception does not invalidate any other reasons why a work
#     based on this file might be covered by the GNU General Public
#     License.
# 
#     contact information:
#         email: TouchStone222@runbox.com <David Lindauer>
# 

SHELL=cmd.exe

H_FILES = $(wildcard *.h)
C_DEPENDENCIES = $(wildcard *.c)
ASM_DEPENDENCIES = $(wildcard c0*.nas)

all: $(DEPENDENCIES) dfstb32.exe
$(eval $(FORMATTER))

c0.o: c0.c
	$(CC) $(STDINCLUDE) /c $(CFLAGS) /s16 -o$(WINOBJECT)\$@ $^

c0pe.o: c0pe.nas
	$(ASM) $(ASMFLAGS) -o$(SYSOBJECT)\$@ $^
c0xpe.o: c0xpe.nas
	$(ASM) $(ASMFLAGS) -o$(SYSOBJECT)\$@ $^
c0dpe.o: c0dpe.nas
	$(ASM) $(ASMFLAGS) -o$(SYSOBJECT)\$@ $^

wildargsw.o: wildargsw.c
	$(CC) /c $(STDINCLUDE) $(CFLAGS) -o$(SYSOBJECT)\$@ $^

.\dfstb32.o: dfstb32.nas
	$(ASM) $(ASMFLAGS) -o$@ $^

dfstb32.exe: .\dfstb32.o
	$(LINK) $(LINKFLAGS) /T:REAL -odfstb32 $^

DISTRIBUTE:
	copy *.nas $(DISTSTARTUPWIN)
	copy *.c $(DISTSTARTUPWIN)
