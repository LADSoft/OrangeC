INCLUDE := include
EXE := bitwise.exe framing.exe ogg.l

all: $(EXE)
	bitwise 2> bitwise.out
	fc /b bitwise.out bitwise.cmpx
	framing 2> framing.out
	fc /b framing.out framing.cmpx	

ogg.l: bitwise.o framing.o
	olib /! ogg.l +- bitwise.o framing.o

bitwise.o: bitwise.c
	occ /c /! /9 /I$(INCLUDE) /D_WIN32 /DNDEBUG /D_WINDOWS /D_USRDLL /DLIBOGG_EXPORTS bitwise.c
framing.o: framing.c
	occ /c /! /9 /I$(INCLUDE) /D_WIN32 /DNDEBUG /D_WINDOWS /D_USRDLL /DLIBOGG_EXPORTS framing.c

bitwise.exe: bitwise.c
	occ /9 /! /I$(INCLUDE) /D_V_SELFTEST /D_WIN32 /DNDEBUG /D_WINDOWS /D_USRDLL /DLIBOGG_EXPORTS bitwise.c

framing.exe: framing.c
	occ /9 /! /I$(INCLUDE) /D_V_SELFTEST /D_WIN32 /DNDEBUG /D_WINDOWS /D_USRDLL /DLIBOGG_EXPORTS framing.c

