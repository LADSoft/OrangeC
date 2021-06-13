all: comp.tst

comp.tst: comp.exe
	szip szip.ref encode.tst
	szip -d encode.tst decode.tst
	fc /b encode.tst encode.cmpx
	fc /b decode.tst szip.c

comp.exe:
	-copy \orangec\bin\lsmsilcrtl.dll *.*
	occil comp.c /DMY_UNSIGNED_TYPES /oszip.exe /C+f	

clean:
	del *.exe