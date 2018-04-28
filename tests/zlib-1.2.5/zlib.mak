
.path.c =

OUT = zlib.l
OBJ = adler32.o compress.o crc32.o deflate.o gzclose.o gzlib.o gzread.o \
      gzwrite.o infback.o inflate.o inftrees.o inffast.o trees.o uncompr.o zutil.o
INC =

LIB =

CFLAGS = /c +e /9 /D_lseeki64=lseek /D_wopen=open
AFLAGS = -fobj
LFLAGS = /Wc
RFLAGS = /r


all: $(OUT)


CCDIR = \orangec\bin

CC = $(CCDIR)\occ
AS = $(CCDIR)\nasm
LD = $(CCDIR)\valx
RC = $(CCDIR)\xrc
AR = $(CCDIR)\olib


$(OUT): $(OBJ)
    @del $(OUT)
        $(AR) $(OUT) $(OBJ)

$(OBJ): $(INC)

.c.o:
        @$(CC) $(CFLAGS) $(LFLAGS) $<

.asm.o:
        @$(AS) $(AFLAGS) $<

.rc.res:
        $(RC) $(RFLAGS) $<
