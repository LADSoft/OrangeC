DIRS = alexcs asm asmgas bzip2-1.0.5 cpplinq ellf general lame libogg-1.2.0 libvorbis-1.3.2 lpng162 pelib sqlite3 x264 zlib-1.2.5 errchk regression atomic attributes preprocessor orc embed_tests

CDIRS = $(addsuffix .dir, $(DIRS))
CLEANDIRS = $(addsuffix .cleandir, $(DIRS))

CLEAN = -del *.o *.exe *.out *.tst *.res *.l *.dll 2>$(NULLDEV)
export CLEAN

all: $(CDIRS)  ctestsuite
	$(MAKE) /Care-we-fast-yet
	echo %ERRORLEVEL%

clean: $(CLEANDIRS) ctestsuite.clean
	$(MAKE) /Care-we-fast-yet clean

ctestsuite:
	$(MAKE) /j:1 /Cc-testsuite

ctestsuite.clean:
	$(MAKE) /Cc-testsuite clean

$(CDIRS): %.dir :
	$(MAKE) /C$*

$(CLEANDIRS): %.cleandir :
	$(MAKE) /C$* clean

