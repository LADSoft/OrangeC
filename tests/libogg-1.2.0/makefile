INCLUDE := include
EXE := bitwise.exe framing.exe ogg.l

all: $(EXE)
	bitwise 2> bitwise.out
	fc /b bitwise.out bitwise.cmpx
	framing 2> framing.out
	fc /b framing.out framing.cmpx	

ogg.l: bitwise.o framing.o
	del ogg.l
	olib ogg.l bitwise.o framing.o

bitwise.o:
	occ /c /9 /I$(INCLUDE) /D_WIN32 /DNDEBUG /D_WINDOWS /D_USRDLL /DLIBOGG_EXPORTS bitwise.c
framing.o:
	occ /c /9 /I$(INCLUDE) /D_WIN32 /DNDEBUG /D_WINDOWS /D_USRDLL /DLIBOGG_EXPORTS framing.c

bitwise.exe:
	occ /9 /I$(INCLUDE) /D_V_SELFTEST /D_WIN32 /DNDEBUG /D_WINDOWS /D_USRDLL /DLIBOGG_EXPORTS bitwise.c
	del bitwise.o

framing.exe:
	occ /9 /I$(INCLUDE) /D_V_SELFTEST /D_WIN32 /DNDEBUG /D_WINDOWS /D_USRDLL /DLIBOGG_EXPORTS framing.c
	del framing.o

