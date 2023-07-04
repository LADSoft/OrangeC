ifeq "$(COMPILER)" "gcc-linux"
NULLDEV := /dev/null
else
NULLDEV := NUL
endif

vpath %.c $(ORANGEC)\src\sqlite3

test: main
	testsqlite < sqlite.in > sqlite.txt
	fc /b sqlite.txt sqlite.cmpx
	test2 > test2.txt
	fc /b test2.txt test2.cmpx

main: zzz.dll testsqlite.exe test.exe test2.exe

sqlite3.dll: sqlite3.c
	-copy $(ORANGEC)\bin\lsmsilcrtl.dll *.* > $(NULLDEV)
	occil /9 /o$^ /Wd /Lkernel32 $< /Nsqlite3.sqlite

zzz.dll:
	-copy $(ORANGEC)\bin\lsmsilcrtl.dll *.* > $(NULLDEV)
	occil /Wd zzz.c /Nzzz.ar

test.exe: sqlite3.c shell.c sqlite3.dll
	-copy $(ORANGEC)\bin\lsmsilcrtl.dll *.* > $(NULLDEV)
	occil /otest.exe sqlite3.c shell.c /I$(ORANGEC)\src\sqlite3 /LKernel32

testsqlite.exe: testsqlite.cs sqlite3.dll
	-copy $(ORANGEC)\bin\lsmsilcrtl.dll *.* > $(NULLDEV)
	csc /unsafe /r:lsmsilcrtl.dll;zzz.dll;sqlite3.dll /platform:x86 testsqlite.cs 

test2.exe: test2.cs sqlite3.dll
	-copy $(ORANGEC)\bin\lsmsilcrtl.dll *.* > $(NULLDEV)
	csc /unsafe /r:lsmsilcrtl.dll;zzz.dll;sqlite3.dll /platform:x86 test2.cs 

clean:
	del *.exe
	del sqlite3.dll