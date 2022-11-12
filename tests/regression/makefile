FILES := $(wildcard *.c) $(wildcard *.cpp) $(wildcard *.asm)
EXCLUDE :=
DEPENDENCIES := $(filter-out $(EXCLUDE), $(FILES))
TESTS := $(DEPENDENCIES:.cpp=.tst)
TESTS := $(TESTS:.c=.tst)
TESTS := $(TESTS:.asm=.tst)
TESTS := $(TESTS) fastcall1.tst
all: $(TESTS)

%.o: %.c
	occ /1 /DWIN32_LEAN_AND_MEAN /c /! $<

%.o: %.cpp
	occ /c /DWIN32_LEAN_AND_MEAN /! $<

%.o: %.asm
	oasm /! $<
	
%.exe: %.o
	occ /! $<
	
mpfrdll.dll: mpfr.c
	occ /! /Wd /ompfrdll.dll /DDLL mpfr.c
	oimplib /! -C mpfrdll.l mpfrdll.dll

mpfr.exe: mpfrdll.dll mpfr.c
	occ /! /ompfr.exe mpfr.c -l mpfrdll.l

fastcall1.exe: fastcall.c
	occ /! /ofastcall1.exe /O- fastcall.c

%.tst: %.exe
	$< > $*.out
	fc /b $*.cmpx $*.out

clean:
	$(CLEAN)

