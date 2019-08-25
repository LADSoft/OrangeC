
main: sqlite3.dll zzz.dll testsqlite.exe

sqlite3.dll:
	occil /9 /Wd /Lkernel32 sqlite3.c /Nsqlite3.sqlite

zzz.dll:
	occil /Wd zzz.c /Nzzz.ar

testsqlite.exe:
	csc /unsafe /r:lsmsilcrtl.dll;zzz.dll;sqlite3.dll /platform:x86 testsqlite.cs 
