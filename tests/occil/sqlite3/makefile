test: main
	testsqlite < sqlite.in > sqlite.txt
	fc /b sqlite.txt sqlite.cmpx

main: sqlite3.dll zzz.dll testsqlite.exe test.exe

sqlite3.dll:
	copy \orangec\bin\lsmsilcrtl.dll *.*
	occil /9 /Wd /Lkernel32 sqlite3.c /Nsqlite3.sqlite

zzz.dll:
	copy \orangec\bin\lsmsilcrtl.dll *.*
	occil /Wd zzz.c /Nzzz.ar

test.exe: test.c
	copy \orangec\bin\lsmsilcrtl.dll *.*
	occil test.c /LKernel32

testsqlite.exe: testsqlite.cs
	copy \orangec\bin\lsmsilcrtl.dll *.*
	csc /unsafe /r:lsmsilcrtl.dll;zzz.dll;sqlite3.dll /platform:x86 testsqlite.cs 
