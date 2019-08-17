	occil /c /9 /Wd /Lkernel32 sqlite3.c /Nsqlite3.sqlite
	occil /c /Wd zzz.c /Nzzz.ar
	csc /unsafe /r:"%OCCIL_ROOT%\bin\lsmsilcrtl.dll";zzz.dll;sqlite3.dll /platform:x86 testsqlite.cs 
