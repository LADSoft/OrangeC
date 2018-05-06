INCLUDE := include

all: ogg.l

ogg.l: bitwise.o framing.o
	del ogg.l
	olib ogg.l bitwise.o framing.o

bitwise.o:
	occ /c /9 /I$(INCLUDE) /D_WIN32 /DNDEBUG /D_WINDOWS /D_USRDLL /DLIBOGG_EXPORTS bitwise.c
framing.o:
	occ /c /9 /I$(INCLUDE) /D_WIN32 /DNDEBUG /D_WINDOWS /D_USRDLL /DLIBOGG_EXPORTS framing.c

