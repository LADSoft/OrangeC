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

EXCLUDE = $(EXCLUDE) crtdll.l

all: crtdll.l

.\crtdmain.o: crtdmain.c
	$(CC) /c $(CFLAGS) -o$@ $^

.PHONY: .\c0.o
.\c0.o: ..\pe\c0.c
	$(CC) /c $(CFLAGS) -Wcl -o$@ $^

crtdll.l: .\crtdmain.o .\c0.o
	-del $@
	$(LIB) $(LIBFLAGS) $@ + .\c0.o .\crtdmain.o
	$(LIB) $(LIBFLAGS) $@ + $(CLIBOBJECT)\srproc.o $(CLIBOBJECT)\lxsar.o $(CLIBOBJECT)\lxshr.o $(CLIBOBJECT)\lxshl.o $(CLIBOBJECT)\lxmul.o $(CLIBOBJECT)\lxdiv.o $(CLIBOBJECT)\substk.o $(CLIBOBJECT)\stkchk.o $(OBJECT)\vsconio.o
	$(IMPLIB) $@ .\crtdll.def

DISTRIBUTE:
	copy *.c $(DISTSTARTUPWIN)
