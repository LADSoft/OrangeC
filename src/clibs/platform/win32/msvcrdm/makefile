#	Software License Agreement (BSD License)
#	
#	Copyright (c) 1997-2009, David Lindauer, (LADSoft).
#	All rights reserved.
#	
#	Redistribution and use of this software in source and binary forms, 
#	with or without modification, are permitted provided that the following 
#	conditions are met:

#	* Redistributions of source code must retain the above
#	  copyright notice, this list of conditions and the
#	  following disclaimer.

#	* Redistributions in binary form must reproduce the above
#	  copyright notice, this list of conditions and the
#	  following disclaimer in the documentation and/or other
#	  materials provided with the distribution.

#	* Neither the name of LADSoft nor the names of its
#	  contributors may be used to endorse or promote products
#	  derived from this software without specific prior
#	  written permission of LADSoft.

#	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
#	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
#	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
#	PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
#	OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
#	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
#	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
#	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
#	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
#	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#	contact information:
#		email: TouchStone222@runbox.com <David Lindauer>
EXCLUDE = $(EXCLUDE) msvcrt.l

all: $(DEPENDENCIES) msvcrt.l

.\msvcrdm.o: msvcrdm.c
	$(CC) /c $(CFLAGS) -o$@ $^

.\c0.o: ..\pe\c0.c
	$(CC) /c $(CFLAGS) -Wcl -o$@ $^

msvcrt.l: .\msvcrdm.o .\c0.o
	del $(SYSOBJECT)\$@
	$(LIB) $(LIBFLAGS) $(SYSOBJECT)\$@ + .\c0.o .\msvcrdm.o
	$(LIB) $(LIBFLAGS) $(SYSOBJECT)\$@ + @&&|
$(CLIBOBJECT)\srproc.o $(CLIBOBJECT)\lxsar.o $(CLIBOBJECT)\lxshr.o $(CLIBOBJECT)\lxshl.o $(CLIBOBJECT)\lxmul.o $(CLIBOBJECT)\lxdiv.o $(CLIBOBJECT)\substk.o $(CLIBOBJECT)\stkchk.o $(WINOBJECT)\vsconio.o
$(CLIBOBJECT)\nan.o $(CLIBOBJECT)\signbit.o $(CLIBOBJECT)\nancomp.o $(CLIBOBJECT)\acosh.o $(CLIBOBJECT)\asinh.o
$(CLIBOBJECT)\atanh.o $(CLIBOBJECT)\cbrt.o $(CLIBOBJECT)\copysign.o $(CLIBOBJECT)\erf.o $(CLIBOBJECT)\exp2.o
$(CLIBOBJECT)\expm1.o $(CLIBOBJECT)\fdim.o $(CLIBOBJECT)\fma.o $(CLIBOBJECT)\fmax.o $(CLIBOBJECT)\fmin.o
$(CLIBOBJECT)\ilogb.o $(CLIBOBJECT)\lgamma.o $(CLIBOBJECT)\lrint.o $(CLIBOBJECT)\llrint.o $(CLIBOBJECT)\logb.o
$(CLIBOBJECT)\log1p.o $(CLIBOBJECT)\log2.o $(CLIBOBJECT)\lround.o $(CLIBOBJECT)\llround.o $(CLIBOBJECT)\nearbyin.o
$(CLIBOBJECT)\next.o $(CLIBOBJECT)\remain.o $(CLIBOBJECT)\remquo.o $(CLIBOBJECT)\rint.o $(CLIBOBJECT)\round.o
$(CLIBOBJECT)\scalbn.o $(CLIBOBJECT)\signif.o $(CLIBOBJECT)\tgamma.o $(CLIBOBJECT)\trunc.o $(CLIBOBJECT)\abs.o
$(CLIBOBJECT)\lldiv.o $(CLIBOBJECT)\i64toa.o $(CLIBOBJECT)\_matherr.o $(CLIBOBJECT)\genmerr.o
$(CLIBOBJECT)\acos.o $(CLIBOBJECT)\asin.o $(CLIBOBJECT)\atan2.o $(CLIBOBJECT)\atan.o $(CLIBOBJECT)\ceil.o $(CLIBOBJECT)\cos.o $(CLIBOBJECT)\cosh.o
$(CLIBOBJECT)\exp.o $(CLIBOBJECT)\fabs.o $(CLIBOBJECT)\floor.o $(CLIBOBJECT)\fmod.o $(CLIBOBJECT)\frexp.o $(CLIBOBJECT)\ldexp.o $(CLIBOBJECT)\log.o
$(CLIBOBJECT)\log10.o $(CLIBOBJECT)\modf.o $(CLIBOBJECT)\pow.o $(CLIBOBJECT)\sin.o $(CLIBOBJECT)\sinh.o $(CLIBOBJECT)\sqrt.o
$(CLIBOBJECT)\tan.o $(CLIBOBJECT)\tanh.o $(CLIBOBJECT)\fpclass.o
$(CLIBOBJECT)\hypot.o $(CLIBOBJECT)\polyf.o $(CLIBOBJECT)\poly.o $(CLIBOBJECT)\polyl.o $(CLIBOBJECT)\pow10.o
$(CLIBOBJECT)\del1.o $(CLIBOBJECT)\new1.o $(CLIBOBJECT)\del2.o $(CLIBOBJECT)\new2.o $(CLIBOBJECT)\rnew.o
$(CLIBOBJECT)\_ixb.o $(CLIBOBJECT)\cpphandler.o $(CLIBOBJECT)\_term.o $(CLIBOBJECT)\_gunwind.o $(CLIBOBJECT)\arrcall.o
$(WINOBJECT)\secure.o
|
	$(IMPLIB) $(SYSOBJECT)\$@ .\msvcrt.def

DISTRIBUTE:
	copy *.c $(DISTSTARTUPWIN)