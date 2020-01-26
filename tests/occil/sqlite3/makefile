ifeq "$(COMPILER)" "gcc-linux"
NULLDEV := /dev/null
else
NULLDEV := NUL
endif

vpath %.c $(ORANGEC)\src\sqlite3

test: main
	testsqlite < sqlite.in > sqlite.txt
	fc /b sqlite.txt sqlite.cmpx

main: zzz.dll testsqlite.exe test.exe

sqlite3.dll: sqlite3.c
	-copy \orangec\bin\lsmsilcrtl.dll *.* > $(NULLDEV)
	occil /9 /o$^ /Wd /Lkernel32 $< /Nsqlite3.sqlite

zzz.dll:
	-copy \orangec\bin\lsmsilcrtl.dll *.* > $(NULLDEV)
	occil /Wd zzz.c /Nzzz.ar

test.exe: test.c sqlite3.dll
	-copy \orangec\bin\lsmsilcrtl.dll *.* > $(NULLDEV)
	occil test.c /I$(ORANGEC)\src\sqlite3 /LKernel32

testsqlite.exe: testsqlite.cs sqlite3.dll
	-copy \orangec\bin\lsmsilcrtl.dll *.* > $(NULLDEV)
	csc /unsafe /r:lsmsilcrtl.dll;zzz.dll;sqlite3.dll /platform:x86 testsqlite.cs 
