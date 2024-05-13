ifneq "$(NETCORE)" ""
NETCORE := -DNETCORE
NETCOREDLL := --netcore 0
endif

ifeq "$(COMPILER)" "gcc-linux"
NULLDEV := /dev/null
else
NULLDEV := NUL
endif

CURFILES = $(wildcard *.c)

all: cc386x sqlite3x szipx local
	echo %ERRORLEVEL%

lsmsilcrtl.dll:
	-copy $(ORANGEC)\bin\lsmsilcrtl*.dll *.* > $(NULLDEV)

cc386x:
	$(MAKE) -Ccc386 $(NETCORE)

sqlite3x:
	$(MAKE) -Csqlite3 $(NETCORE)

szipx:
	$(MAKE) -Cszip $(NETCORE)

local: lsmsilcrtl.dll $(CURFILES:.c=.exe)

vvtest.exe: vvtest.c dll.dll
	occil $(NETCOREDLL) vvtest.c /Ldll.dll
	vvtest > vvtest.txt.
	fc /b vvtest.txt vvtest.cmpx
cc.exe: cc.c
	occil $(NETCOREDLL) cc.c /Lkernel32
	cc xx.cc
	xx > xx.tst
	fc /b xx.tst xx.cmpx

dll.exe:
	echo hi

dll.dll: dll.c
	occil $(NETCOREDLL) /Wd dll.c /Na.b

r1.exe: r1.c r2.c
	occil $(NETCOREDLL) r1.c r2.c
	r1 > r1.txt
	fc /b r1.txt r1.cmpx
r2.exe:
	echo hi

base_conv.exe: base_conv.c
	occil $(NETCOREDLL) base_conv.c
	base_conv < base_conv.in > base_conv.txt
	fc /b base_conv.txt base_conv.cmpx

float_to_ieee754.exe: float_to_ieee754.c
	occil $(NETCOREDLL) float_to_ieee754.c
	float_to_ieee754 < float_to_ieee754.in > float_to_ieee754.txt
	fc /b float_to_ieee754.txt float_to_ieee754.cmpx


determinante.exe: determinante.c
	occil $(NETCOREDLL) determinante.c
	determinante < determinante.in > determinante.txt
	fc /b determinante.txt determinante.cmpx

%.exe: %.c
	occil $(NETCOREDLL) /9 $< /LKernel32 /LUser32 /LGDI32
	$* > $*.txt
	fc /b $*.txt $*.cmpx

clean:
	del *.exe
	del *.dll
	$(MAKE) -Ccc386 clean
	$(MAKE) -Csqlite3 clean
	$(MAKE) -Cszip clean
