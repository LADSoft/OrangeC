FILES := $(wildcard *.rc)
EXCLUDE :=
DEPENDENCIES := $(filter-out $(EXCLUDE), $(FILES))
TESTS := $(DEPENDENCIES:.rc=.tst)
all: $(TESTS)

%.tst: %.res
	fc $*.res $*.cmpx
%.res: %.rc
	orc /! $<
clean:
	del *.res

