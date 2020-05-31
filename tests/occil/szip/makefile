all: comp.tst

comp.tst: comp.exe
	szip szip.c encode.tst
	szip -d encode.tst decode.tst
	fc /b encode.tst encode.cmpx
	fc /b decode.tst szip.c

comp.exe:
	occil comp.c /oszip.exe /C+f	