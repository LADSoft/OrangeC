INCLUDE := include
EXE := bitwise.exe framing.exe ogg.l

all: $(EXE)
	bitwise 2> bitwise.out
	fc /b bitwise.out bitwise.cmpx
	framing 2> framing.out
	fc /b framing.out framing.cmpx	

clean:
	$(CLEAN)

ogg.l: bitwise.o framing.o
	olib /! ogg.l +- bitwise.o framing.o

bitwise.o: bitwise.c
	occ /c /! /9 /I$(INCLUDE) /D_WIN32 /DNDEBUG /D_WINDOWS /D_USRDLL /DLIBOGG_EXPORTS bitwise.c
framing.o: framing.c
	occ /c /! /9 /I$(INCLUDE) /D_WIN32 /DNDEBUG /D_WINDOWS /D_USRDLL /DLIBOGG_EXPORTS framing.c

bitwise1.o: bitwise.c
	occ /c /9 /! /I$(INCLUDE) /D_V_SELFTEST /D_WIN32 /DNDEBUG /D_WINDOWS /D_USRDLL /DLIBOGG_EXPORTS /obitwise1.o bitwise.c

framing1.o: framing.c
	occ /c /9 /! /I$(INCLUDE) /D_V_SELFTEST /D_WIN32 /DNDEBUG /D_WINDOWS /D_USRDLL /DLIBOGG_EXPORTS /oframing1.o framing.c

bitwise.exe: bitwise1.o
	occ /! /obitwise bitwise1.o
framing.exe: framing1.o
	occ /! /oframing framing1.o

