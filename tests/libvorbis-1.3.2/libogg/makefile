INCLUDE := ..\..\libogg-1.2.0\include

vpath %.c ..\..\libogg-1.2.0

all: ogg.l

ogg.l: bitwise.o framing.o
	del ogg.l 2> NUL
	olib ogg.l bitwise.o framing.o

bitwise.o: bitwise.c
	occ /c /! /9 /I$(INCLUDE) /D_WIN32 /DNDEBUG /D_WINDOWS /D_USRDLL /DLIBOGG_EXPORTS $<
framing.o: framing.c
	occ /c /! /9 /I$(INCLUDE) /D_WIN32 /DNDEBUG /D_WINDOWS /D_USRDLL /DLIBOGG_EXPORTS $<

