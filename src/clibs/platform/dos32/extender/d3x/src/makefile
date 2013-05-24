#		   D3X(tm) DOS-Extender v0.90 unleash `g' alpha
#				  core makefile
#					
#			Copyright (c) 1998-2002 Borca Daniel
#					
#				 dborca@yahoo.com
#			 http://www.geocities.com/dborca

AS = nasm
AFLAGS = -fobj -ikernel/
LINK = alink
LDFLAGS =
CC = gcc
CFLAGS = -Wall -W -pedantic -O2
RM = del

.SUFFIXES:
.SUFFIXES: .exe .obj

.obj.exe:
	$(LINK) $(LDFLAGS) $<

stubx.exe: stubx.c d3x1.h d3xd.h d3xw.h
	$(CC) -o $@ -s $(CFLAGS) $<
	upx --best $@
	$@ -s $@

d3x1.h: fix.exe d3x1.exe
	fix.exe d3x1.exe -e > $@

d3xd.h: fix.exe d3xd.exe
	fix.exe d3xd.exe -p -e > $@

d3xw.h: fix.exe d3xw.exe
	fix.exe d3xw.exe -n -e > $@

fix.exe:
	$(CC) -o $@ $(CFLAGS) fix.c

d3x1.obj:
	$(AS) -o $@ $(AFLAGS) -DTARGET=0 kernel/d3x.asm

d3xd.obj:
	$(AS) -o $@ $(AFLAGS) -DTARGET=1 kernel/d3x.asm

d3xw.obj:
	$(AS) -o $@ $(AFLAGS) -DTARGET=2 -DI21API -DI33API kernel/d3x.asm

clean:
	-$(RM) d3x*.*
	-$(RM) fix.exe
