FILES := $(wildcard *.c) $(wildcard *.cpp)
EXCLUDE :=
DEPENDENCIES := $(filter-out $(EXCLUDE), $(FILES))
TESTS := $(DEPENDENCIES:.cpp=.tst)
TESTS := $(TESTS:.c=.tst)
TESTS := $(TESTS:.asm=.tst)

.PHONY: all testlinkage clean

all: $(TESTS)

clean:
	$(CLEAN)

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
	fc /b testlinkage.cmpx $*

