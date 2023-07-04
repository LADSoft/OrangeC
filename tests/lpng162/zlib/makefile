
vpath %.c ..\..\zlib-1.2.5

OUT = zlib.l
INCLUDE= ..\..\zlib\1.2.5
OBJ = adler32.o compress.o crc32.o deflate.o gzclose.o gzlib.o gzread.o \
      gzwrite.o infback.o inflate.o inftrees.o inffast.o trees.o uncompr.o zutil.o
INC =

LIB =

CFLAGS = /c +e /! /9 /D_lseeki64=lseek /D_wopen=open /I$(INCLUDE)
AFLAGS = -fobj
LFLAGS = /Wc /!
RFLAGS = /r /!


all: $(OUT)

clean:
	$(CLEAN)

CCDIR = $(ORANGEC)\bin

CC = $(CCDIR)\occ
AS = $(CCDIR)\nasm
LD = $(CCDIR)\valx
RC = $(CCDIR)\xrc
AR = $(CCDIR)\olib


$(OUT): $(OBJ)
	$(AR) /! $(OUT) +- $(OBJ)

$(OBJ): $(INC)

%.o: %.c
	@$(CC) $(CFLAGS) $(LFLAGS) $<

%.o: %.asm
	@$(AS) $(AFLAGS) $<

%.res: %.rc
	$(RC) $(RFLAGS) $<
