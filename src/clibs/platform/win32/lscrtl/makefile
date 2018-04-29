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
#     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
# 
#     contact information:
#         email: TouchStone222@runbox.com <David Lindauer>

RCFILE=lscrtl

all: .\c0dll.o .\lscrtl.dll .\lscrtlil.dll

ifeq ($(findstring sh.exe,$(SHELL)),)
B = /b
endif

$(info "DLLOBJECT=$(DLLOBJECT)")
WINOBJS = $(shell dir $(B) $(WINOBJECT)\*.o)
DLLOBJS = $(shell dir $(B) $(DLLOBJECT)\*.o)
$(info "DLLOBJS= $(DLLOBJS)")
.\lscrtl.dll: .\lscrtl.o .\c0dll.o $(RCFILE).res
	-del $@
	$(LINK) $(LINKFLAGS) /T:DLL32 -o.\lscrtl .\lscrtl.o .\c0dll.o $(CLIBOBJECT)\srproc.o $(foreach dir, $(filter-out c0.o c0lsstartup.o, $(DLLOBJS)), $(DLLOBJECT)\$(dir)) $(SYSOBJECT)\climp.l $(RCFILE).res
	move lscrtl.dll $(LSCRTLOBJECT)
	move lscrtl.l $(LSCRTLOBJECT)

.\lscrtlil.dll: .\lscrtl.o .\c0dll.o $(RCFILE).res
	-del $@
	$(LINK) $(LINKFLAGS) /l /T:DLL32 -o.\lscrtlil.rel .\lscrtl.o .\c0dll.o $(CLIBOBJECT)\srproc.o $(foreach dir, $(filter-out c0.o c0lsstartup.o, $(DLLOBJS)), $(DLLOBJECT)\$(dir)) $(SYSOBJECT)\climp.l $(RCFILE).res
	dlpe /! /f /mDLL /olscrtlil.dll lscrtl.
#	move lscrtlil.dll \occil\bin

.\lscrtl.o: lscrtl.nas
	$(ASM) $(ASMFLAGS) -o$@ $^

.\c0dll.o: c0dll.c
	$(CC) /c $(CFLAGS) -o$@ $^

$(RCFILE).res: $(RCFILE).rc ..\..\..\..\version.h
	orc -i$(ORANGEC)\include $(RCFILE).rc

#DISTRIBUTE:
#	$(ZIP) -porangec a $(DISTADDON)\lscrtl $(LSCRTLOBJECT)\*.*
