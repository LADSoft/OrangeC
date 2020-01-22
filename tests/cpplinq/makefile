FILES = cpplinqtest.o

%.o: %.cpp
	occ /c /! -o$@ $^

test: cpplinqtest.exe
	cpplinqtest > cpplinqtest.out
	fc /b cpplinqtest.cmpx cpplinqtest.out
	$(MAKE) /Cfwdslash
	$(MAKE) /Cbackslash

clean:
	$(CLEAN)

cpplinqtest.exe: $(FILES)
	olink /c /! /T:CON32 /mx /o$@ c0xpe.o $(FILES) clwin.l climp.l