DIRS = alexcs asm asmgas bzip2-1.0.5 cpplinq ellf general lame libogg-1.2.0 libvorbis-1.3.2 lpng162 sqlite3 x264 zlib-1.2.5 errchk regression atomic attributes preprocessor

CDIRS = $(addsuffix .dir, $(DIRS))

all: $(CDIRS)

$(CDIRS): %.dir :
	$(MAKE) /C$*
