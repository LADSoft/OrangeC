FILES := $(wildcard *.c) $(wildcard *.cpp) $(wildcard *.asm)
EXCLUDE :=
DEPENDENCIES := $(filter-out $(EXCLUDE), $(FILES))
TESTS := $(DEPENDENCIES:.cpp=.tst)
TESTS := $(TESTS:.c=.tst)
TESTS := $(TESTS:.asm=.tst)
TESTS := $(TESTS) fastcall1.tst
all: $(TESTS)

CFLAGS= /! /1 /DWIN32_LEAN_AND_MEAN
CXXFLAGS= /! /DWIN32_LEAN_AND_MEAN
ASFLAGS= /!
LDFLAGS= /!
	
mpfrdll.dll: mpfr.c
	occ /! /Wd /ompfrdll.dll /DDLL mpfr.c
	oimplib /! -C mpfrdll.l mpfrdll.dll

mpfr.exe: mpfrdll.dll mpfr.c
	occ /! /ompfr.exe mpfr.c -l mpfrdll.l

fastcall1.exe: fastcall.c
	occ /! /ofastcall1.exe /O- fastcall.c

canary.exe: canary.c
	occ -fstack-protector canary.c

canary.tst: canary.exe
	-$< 2> canary.out
	fc /b canary.cmpx canary.out

bufferoverflow.exe: bufferoverflow.c
	occ -fruntime-object-overflow bufferoverflow.c

bufferoverflow.tst: bufferoverflow.exe
	-$< 2> bufferoverflow.out
	fc /b bufferoverflow.cmpx bufferoverflow.out

uninitvar.exe: uninitvar.c
	occ -fruntime-uninitialized-variable -g uninitvar.c

uninitvar.tst: uninitvar.exe
	-$< 2> uninitvar.out
	fc /b uninitvar.cmpx uninitvar.out

heap.exe: heap.c
	occ -fruntime-heap-check heap.c

heap.tst: heap.exe
	-$< 2> heap.out
	fc /b heap.cmpx heap.out

cppexterninline.o: cppexterninline.cpp
	occ /c /ocppexterninline.out $<

cppexterninline.tst: cppexterninline.o
	-$ onm cppexterninline.o > cppexterninline.out
	fc cppexterninline.cmpx cppexterninline.out

%.tst: %.exe
	$< > $*.out
	fc /b $*.cmpx $*.out

clean:
	$(CLEAN)

