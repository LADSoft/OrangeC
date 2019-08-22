FILES := $(wildcard *.c) $(wildcard *.cpp)
EXCLUDE :=
DEPENDENCIES := $(filter-out $(EXCLUDE), $(FILES))
TESTS := $(DEPENDENCIES:.cpp=.tst)
TESTS := $(TESTS:.c=.tst)
TESTS := $(TESTS:.asm=.tst)

.PHONY: all testlinkage clean

all: $(TESTS)

%.tst: %.c
	occ /1 /c /! $< > $*.tst
	del $*.o
	fc /b $*.cmpx $*.tst

%.tst: %.cpp
	occ /1 /c /! $< > $*.tst
	del $*.o
	fc /b $*.cmpx $*.tst

testlinkage.tst: testlinkage.cpp
	-occ /1 /c /! $< > $*
	del testlinkage.o
	fc /b testlinkage.cmpx $*

clean:
	del *.tst
	del *.o
