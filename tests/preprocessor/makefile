TEST_FILES = n_1.exe \
    n_10.exe \
    n_11.exe \
    n_12.exe \
    n_13.exe \
    n_13_13.exe \
    n_13_5.exe \
    n_13_7.exe \
    n_13_8.exe \
    n_15.exe \
    n_18.exe \
    n_19.exe \
    n_2.exe \
    n_20.exe \
    n_21.exe \
    n_22.exe \
    n_23.exe \
    n_24.exe \
    n_25.exe \
    n_26.exe \
    n_27.exe \
    n_28.exe \
    n_29.exe \
    n_3.exe \
    n_30.exe \
    n_32.exe \
    n_37.exe \
    n_4.exe \
    n_5.exe \
    n_6.exe \
    n_7.exe \
    n_9.exe \
    n_std.exe \
    bignum.exe \
    stringize1.exe


all: af.o namedvarargs.exe
	$(MAKE) tests

tests: $(TEST_FILES)

clean:
	$(CLEAN)

af.o : af.c
	occ /c /! $^

namedvarargs.exe: namedvarargs.c
	occ /! /T /9 $^
	-$* > $*.out
	fc /b $*.cmpx $*.out

%.exe: %.c af.o
	occ /! /T /9 $^ af.o
	$*.exe
