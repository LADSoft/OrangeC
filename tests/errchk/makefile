FILES := $(wildcard *.c) $(wildcard *.cpp)
EXCLUDE :=
DEPENDENCIES := $(filter-out $(EXCLUDE), $(FILES))
TESTS := $(DEPENDENCIES:.cpp=.tst)
TESTS := $(TESTS:.c=.tst)
TESTS := $(TESTS:.asm=.tst)

all: $(TESTS)

%.tst: %.c
	occ /1 /c /! $< > $*.tst
	del $*.o
	fc /b $*.cmpx $*.tst

%.tst: %.cpp
	occ /1 /c /! $< > $*.tst
	del $*.o
	fc /b $*.cmpx $*.tst


clean:
	del *.tst
	del *.o
