FILES := $(wildcard *.c) $(wildcard *.cpp) $(wildcard *.asm)
EXCLUDE :=
DEPENDENCIES := $(filter-out $(EXCLUDE), $(FILES))
TESTS := $(DEPENDENCIES:.cpp=.tst)
TESTS := $(TESTS:.c=.tst)
TESTS := $(TESTS:.asm=.tst)

all: $(TESTS)

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

clean:
	del *.tst
	del *.o
	del *.exe