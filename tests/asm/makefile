all: alltests

asmcmp.exe : asmcmp.cpp
	occ $^

%.o: %.asm
	oasm /b $^

%.tst: %.o %1.cmpx
	asmcmp $* > $*.tst
	fc /b $*.tst $*.cmpx

alltests: asmcmp.exe addr16.tst addr32.tst addr64.tst int.tst float.tst sse.tst farbranch.tst