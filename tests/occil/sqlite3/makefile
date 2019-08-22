test: main
	testsqlite < sqlite.in > sqlite.txt
	fc /b sqlite.txt sqlite.cmpx

main: sqlite3.dll zzz.dll testsqlite.exe test.exe

sqlite3.dll:
	occil /9 /Wd /Lkernel32 sqlite3.c /Nsqlite3.sqlite

zzz.dll:
	occil /Wd zzz.c /Nzzz.ar

test.exe: test.c
	occil test.c /LKernel32

testsqlite.exe: testsqlite.cs
	csc /unsafe /r:lsmsilcrtl.dll;zzz.dll;sqlite3.dll /platform:x86 testsqlite.cs 
