OBJS= blocksort.o  \
      huffman.o    \
      crctable.o   \
      randtable.o  \
      compress.o   \
      decompress.o \
      bzlib.o

DLL_OBJS= bzip-dllversion.o

.SUFFIXES:
.SUFFIXES: .c .o .exe .rc .dll .res

RESDIR=.\resource
vpath %.rc $(RESDIR)
vpath %.res $(RESDIR)


.c.o:
	occ /D_WIN32 /Wc /c /! -o$@ $<

$(RESDIR)\%.res: %.rc	
	orc /! -i.\;$(RESDIR) $<

all: bzip2 bzip2recover test

bzip2: libbz2.l bzip2.o $(RESDIR)\bzip2-res.res
	occ /Wc /! /obzip2.exe bzip2.o $(RESDIR)\bzip2-res.res libbz2.l

bzip2recover: libbz2.l bzip2recover.o $(RESDIR)\bzip2recover-res.res
	occ /Wc /! /obzip2recover.exe bzip2recover.o $(RESDIR)\bzip2recover-res.res libbz2.l

libbz2.l: $(OBJS)
	olib /c /! libbz2 +- $(OBJS)

bzip2.dll: libbz2.l $(DLL_OBJS)
	-del bzip2.def
	occ /Wd /! /o$@ $(DLL_OBJS) libbz2.l  

check: test
test: bzip2
	@type words1
	.\bzip2 -1  < sample1.ref > sample1.rb2
	.\bzip2 -2  < sample2.ref > sample2.rb2
	.\bzip2 -3  < sample3.ref > sample3.rb2
	.\bzip2 -d  < sample1.bz2 > sample1.tst
	.\bzip2 -d  < sample2.bz2 > sample2.tst
	.\bzip2 -ds < sample3.bz2 > sample3.tst
	fc /b sample1.bz2 sample1.rb2 
	fc /b sample2.bz2 sample2.rb2
	fc /b sample3.bz2 sample3.rb2
	fc /b sample1.tst sample1.ref
	fc /b sample2.tst sample2.ref
	fc /b sample3.tst sample3.ref
#	@type words3
