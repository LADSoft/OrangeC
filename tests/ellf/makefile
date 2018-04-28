C_DEPENDENCIES = $(wildcard *.c)
EXCLUDE=
DEPENDENCIES = $(filter-out $(EXCLUDE), $(C_DEPENDENCIES))
O_DEPENDENCIES = $(DEPENDENCIES:.c=.o)

%.o: %.c
	occ /9 /c $^
#	oasm $*.asm

test:   ellf.exe
	ellf < ellf.que > q.out
	fc /b test.cmpx q.out

ellf.exe: $(O_DEPENDENCIES)
	olink -oellf.exe /T:CON32 /m /c c0xpe.o $(O_DEPENDENCIES) clwin.l climp.l	
