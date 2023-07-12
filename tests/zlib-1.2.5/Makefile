O = .o

EXE = minigzip.exe
OBJ = minigzip.o \
	adler32.o compress.o crc32.o deflate.o gzclose.o gzlib.o gzread.o \
	gzwrite.o infback.o inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o
INC =

LIB =

CFLAGS = /c +e /! /C-E /9 /D_WIN32
AFLAGS = -fobj
LFLAGS = /Wc /! 
RFLAGS = /r /!


test: $(EXE)
	minigzip minigzip.test
	fc /b minigzip.test.gz.cmpx minigzip.test.gz
	minigzip -d minigzip.test.gz
	fc /b minigzip.test.cmpx minigzip.test
    
clean:
	$(CLEAN)

CC = $(ORANGEC)\bin\occ
AS = $(ORANGEC)\bin\nasm
LD = $(ORANGEC)\bin\valx
RC = $(ORANGEC)\bin\xrc

.SUFFIXES:
.SUFFIXES: .c .o

$(EXE): $(OBJ)
	$(CC) $(LFLAGS) /o$(EXE) $(OBJ) $(LIB)

$(OBJ): $(INC)

.c.o:
	@$(CC) $(CFLAGS) $(LFLAGS) $<

.asm.obj:
	@$(AS) $(AFLAGS) $<

.nas.obj:
	@$(AS) $(AFLAGS) $<

.rc.res:
	$(RC) $(RFLAGS) $<
