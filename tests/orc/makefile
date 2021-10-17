FILES := $(wildcard *.rc)
EXCLUDE :=
DEPENDENCIES := $(filter-out $(EXCLUDE), $(FILES))
TESTS := $(DEPENDENCIES:.rc=.res)
all: $(TESTS)

%.res: %.rc
	orc /! $<
clean:
	del *.res

