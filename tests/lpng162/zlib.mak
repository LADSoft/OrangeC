
.path.c =

OUT = zlib.lib
OBJ = adler32.obj compress.obj crc32.obj deflate.obj gzclose.obj gzlib.obj gzread.obj \
      gzwrite.obj infback.obj inflate.obj inftrees.obj inffast.obj trees.obj uncompr.obj zutil.obj
INC =

LIB =

CFLAGS = /c +e /9 /D_lseeki64=lseek /D_wopen=open
AFLAGS = -fobj
LFLAGS = /Wc
RFLAGS = /r


all: $(OUT)


CCDIR = %userprofile%\ccdl\bin

CC = $(CCDIR)\cc386
AS = $(CCDIR)\nasm
LD = $(CCDIR)\valx
RC = $(CCDIR)\xrc
AR = $(CCDIR)\xlib


$(OUT): $(OBJ)
        $(AR) /p16 $(OUT) -+ $(OBJ)

$(OBJ): $(INC)

.c.obj:
        @$(CC) $(CFLAGS) $(LFLAGS) $<

.asm.obj:
        @$(AS) $(AFLAGS) $<

.rc.res:
        $(RC) $(RFLAGS) $<
