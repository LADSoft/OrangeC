FILES := $(wildcard *.c) $(wildcard *.cpp) $(wildcard *.asm)
EXCLUDE :=
DEPENDENCIES := $(filter-out $(EXCLUDE), $(FILES))
TESTS := $(DEPENDENCIES:.cpp=.tst)
TESTS := $(TESTS:.c=.tst)
TESTS := $(TESTS:.asm=.tst)

all: $(TESTS)

clean:
	$(CLEAN)


%.o: %.c
	occ -D__SIZE_TYPE__=unsigned /1 /c /! $<

%.o: %.cpp
	occ -D__SIZE_TYPE__=unsigned /c /! $<

%.o: %.asm
	oasm $<
	
%.exe: %.o
	occ $<
	
%.tst: %.exe
	$<
	echo %ERRORLEVEL% > $*.tst
