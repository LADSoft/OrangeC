FILES := $(wildcard *.c) $(wildcard *.cpp) $(wildcard *.asm)
EXCLUDE :=
DEPENDENCIES := $(filter-out $(EXCLUDE), $(FILES))
TESTS := $(DEPENDENCIES:.cpp=.tst)
TESTS := $(TESTS:.c=.tst)
TESTS := $(TESTS:.asm=.tst)

all: $(TESTS)

$(info $(TESTS))

%.o: %.c
	occ /1 /c /! $<

%.o: %.cpp
	occ /c /! $<

%.o: %.asm
	oasm $<
	
%.exe: %.o
	occ $<
	
%.tst: %.exe
	$< > $*.tst
	fc /b $*.cmpx $*.tst

clean:
	del *.tst
	del *.o
	del *.exe