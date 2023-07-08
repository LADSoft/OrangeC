ifeq "$(COMPILER)" "gcc-linux"
NULLDEV := /dev/null
else
NULLDEV := NUL
endif

CURFILES = $(wildcard *.c)

all: cc386x sqlite3x szipx local 

cc386x:
	$(MAKE) -Ccc386

sqlite3x:
	$(MAKE) -Csqlite3

szipx:
	$(MAKE) -Cszip

msil:
	$(MAKE) -Cmsil

local: $(CURFILES:.c=.exe)

vvtest.exe: vvtest.c dll.dll
	occil vvtest /Ldll.dll
	vvtest > vvtest.txt
	fc /b vvtest.txt vvtest.cmpx
cc.exe: cc.c
	occil cc /Lkernel32
	cc xx.cc
	xx > xx.tst
	fc /b xx.tst xx.cmpx

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
	-copy $(ORANGEC)\bin\lsmsilcrtl.dll *.* > $(NULLDEV)
	occil /9 $< /LKernel32 /LUser32 /LGDI32
	$* > $*.txt
	fc /b $*.txt $*.cmpx

clean:
	del *.exe
	$(MAKE) -Ccc386 clean
	$(MAKE) -Csqlite3 clean
	$(MAKE) -Cszip clean
