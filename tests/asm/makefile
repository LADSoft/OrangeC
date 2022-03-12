all: alltests

clean:
	$(CLEAN)


asmcmp.exe : asmcmp.cpp
	occ /! $^

%.o: %.nas
	oasm /! /b $^

%.tst: %.o %1.cmpx asmcmp.exe
	asmcmp $* > $*.tst
	fc /b $*.tst $*.cmpx

farbranch.tst: farbranch.o farbranch1.cmpx
	fc /b farbranch.o farbranch1.cmpx

alltests: addr16.tst addr32.tst addr64.tst int.tst float.tst sse.tst farbranch.tst