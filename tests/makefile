DIRS = alexcs asm asmgas bzip2-1.0.5 cpplinq ellf general lame libogg-1.2.0 libvorbis-1.3.2 lpng162 pelib sqlite3 x264 zlib-1.2.5 errchk regression atomic attributes preprocessor c-testsuite

CDIRS = $(addsuffix .dir, $(DIRS))
CLEANDIRS = $(addsuffix .cleandir, $(DIRS))

CLEAN = -del *.o *.exe *.out *.tst *.res 2>NUL
export CLEAN

all: $(CDIRS)
clean: $(CLEANDIRS)

$(CDIRS): %.dir :
	$(MAKE) /C$*

$(CLEANDIRS): %.cleandir :
	$(MAKE) /C$* clean

