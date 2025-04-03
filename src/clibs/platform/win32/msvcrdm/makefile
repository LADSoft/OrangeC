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

EXCLUDE := $(EXCLUDE) msvcrt.l

H_FILES = $(wildcard *.h)

all: $(DEPENDENCIES) msvcrt.l

format: msvcrdm.xcf

.\msvcrdm.o: msvcrdm.c
	$(CC) /c $(CFLAGS) $(STDINCLUDE) -o$@ $^

.PHONY: .\c0.o
.\c0.o: ..\pe\c0.c
	$(CC) /c $(CFLAGS) $(STDINCLUDE) -Wcm -o$@ $^

msvcrt.l: .\msvcrdm.o .\c0.o
	-del $@
	$(LIB) $(LIBFLAGS) $@ +- .\c0.o .\msvcrdm.o
	$(LIB) $(LIBFLAGS) $@ +- $(CLIBOBJECT)\srproc.o $(CLIBOBJECT)\lxsar.o $(CLIBOBJECT)\lxshr.o $(CLIBOBJECT)\lxshl.o $(CLIBOBJECT)\lxmul.o $(CLIBOBJECT)\lxdiv.o $(CLIBOBJECT)\substk.o $(CLIBOBJECT)\stkchk.o $(WINOBJECT)\vsconio.o $(CLIBOBJECT)\nan.o $(CLIBOBJECT)\nancomp.o $(CLIBOBJECT)\acosh.o $(CLIBOBJECT)\asinh.o $(CLIBOBJECT)\atanh.o $(CLIBOBJECT)\cbrt.o $(CLIBOBJECT)\copysign.o $(CLIBOBJECT)\erf.o $(CLIBOBJECT)\exp2.o $(CLIBOBJECT)\fdim.o $(CLIBOBJECT)\fma.o $(CLIBOBJECT)\fmax.o $(CLIBOBJECT)\fmin.o $(CLIBOBJECT)\ilogb.o $(CLIBOBJECT)\lrint.o $(CLIBOBJECT)\llrint.o $(CLIBOBJECT)\logb.o $(CLIBOBJECT)\log2.o $(CLIBOBJECT)\nearbyin.o $(CLIBOBJECT)\next.o $(CLIBOBJECT)\remain.o $(CLIBOBJECT)\remquo.o $(CLIBOBJECT)\rint.o $(CLIBOBJECT)\round.o $(CLIBOBJECT)\scalbn.o $(CLIBOBJECT)\signif.o $(CLIBOBJECT)\tgamma.o $(CLIBOBJECT)\trunc.o $(CLIBOBJECT)\abs.o $(CLIBOBJECT)\lldiv.o $(CLIBOBJECT)\i64toa.o $(CLIBOBJECT)\_matherr.o $(CLIBOBJECT)\genmerr.o $(CLIBOBJECT)\asin.o $(CLIBOBJECT)\atan.o $(CLIBOBJECT)\cosh.o $(CLIBOBJECT)\exp.o $(CLIBOBJECT)\fexcpt.o $(CLIBOBJECT)\fabs.o $(CLIBOBJECT)\floor.o $(CLIBOBJECT)\fmod.o $(CLIBOBJECT)\log.o $(CLIBOBJECT)\log10.o $(CLIBOBJECT)\modf.o $(CLIBOBJECT)\pow.o $(CLIBOBJECT)\sin.o $(CLIBOBJECT)\sinh.o $(CLIBOBJECT)\sqrt.o $(CLIBOBJECT)\tan.o $(CLIBOBJECT)\tanh.o $(CLIBOBJECT)\fpclass.o $(CLIBOBJECT)\polevl.o $(CLIBOBJECT)\exp10.o $(CLIBOBJECT)\_ixb.o $(CLIBOBJECT)\cpphandler.o $(CLIBOBJECT)\_term.o $(CLIBOBJECT)\_gunwind.o $(CLIBOBJECT)\arrcall.o $(CLIBOBJECT)\bad_alloc.o $(CLIBOBJECT)\__fconvto.o $(CLIBOBJECT)\__fconvfrom.o $(CLIBOBJECT)\__fconst.o $(CLIBOBJECT)\unity.o $(CLIBOBJECT)\round.o $(WINOBJECT)\secure.o $(WINOBJECT)\dllmain.o 
	$(IMPLIB) $@ .\msvcrt.def
	move msvcrt.l $(LSCRTLOBJECT)

DISTRIBUTE:
	copy *.c $(DISTSTARTUPWIN)
