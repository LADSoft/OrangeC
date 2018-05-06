
.path.c =

OUT = libpng.l
O=.o
OBJS1 = png$(O) pngerror$(O) pngget$(O) pngmem$(O) pngpread$(O)
OBJS2 = pngread$(O) pngrio$(O) pngrtran$(O) pngrutil$(O) pngset$(O)
OBJS3 = pngtrans$(O) pngwio$(O) pngwrite$(O) pngwtran$(O) pngwutil$(O)
OBJ = $(OBJS1) $(OBJS2) $(OBJS3)
INC =

LIB =

all: pnglibconf.h $(OUT)

pnglibconf.h: scripts\pnglibconf.h.prebuilt
       copy scripts\pnglibconf.h.prebuilt $@


CFLAGS = /c +e /9 -I..\zlib-1.2.5
AFLAGS = -fobj
LFLAGS = /Wc
RFLAGS = /r




CCDIR = c:\orangec\bin

CC = $(CCDIR)\occ
AS = $(CCDIR)\nasm
LD = $(CCDIR)\valx
RC = $(CCDIR)\xrc
AR = $(CCDIR)\olib


$(OUT): $(OBJ)
        del $(OUT)
        $(AR) $(OUT) + $(OBJ)

$(OBJ): $(INC)

.c.o:
        @$(CC) $(CFLAGS) $(LFLAGS) $<

.asm.o:
        @$(AS) $(AFLAGS) $<

.rc.res:
        $(RC) $(RFLAGS) $<


pngtest.exe: pnglibconf.h pngtest$(O) libpng.l
        $(CC) $(LFLAGS) /o$@ pngtest$(O) libpng.l ..\zlib-1.2.5\zlib.l

test: pngtest.exe
	pngtest
