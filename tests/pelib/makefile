FILES = \
inc.o \
pelib.o 

%.o: %.cpp
	occ /c /DWIN32_LEAN_AND_MEAN /! -o$@ $^

test: inc.exe
	-inc
	-inc q.q
	inc main.in basic.in windows.in
	output.exe < test.cmd > test.out
	fc /b test.cmpx test.out  

clean:
	$(CLEAN)

inc.exe: $(FILES)
	olink /c /! /T:CON32 /mx /o$@ @&&|
c0xpe.o $(FILES) clwin.l climp.l
|