CURFILES = $(wildcard *.c)

all: cc386 sqlite3 local

cc386:
	$(MAKE) -Ccc386

sqlite3:
	$(MAKE) -Csqlite3

msil:
	$(MAKE) -Cmsil

local: $(CURFILES:.c=.exe)

vvtest.exe: vvtest.c dll.dll
	occil vvtest /Ldll.dll
	vvtest > vvtest.txt
	fc /b vvtest.txt vvtest.cmpx

dll.exe:
	echo hi

dll.dll: dll.c
	occil /Wd dll /Na.b

r1.exe: r1.c r2.c
	occil r1 r2
	r1 > r1.txt
	fc /b r1.txt r1.cmpx
r2.exe:
	echo hi

base_conv.exe: base_conv.c
	occil base_conv.c
	base_conv < base_conv.in > base_conv.txt
	fc /b base_conv.txt base_conv.cmpx

float_to_ieee754.exe: float_to_ieee754.c
	occil float_to_ieee754.c
	float_to_ieee754 < float_to_ieee754.in > float_to_ieee754.txt
	fc /b float_to_ieee754.txt float_to_ieee754.cmpx


determinante.exe: determinante.c
	occil determinante.c
	determinante < determinante.in > determinante.txt
	fc /b determinante.txt determinante.cmpx

%.exe: %.c
	copy \orangec\bin\*.dll *.*
	occil /9 $< /LKernel32 /LUser32 /LGDI32
	$* > $*.txt
	fc /b $*.txt $*.cmpx