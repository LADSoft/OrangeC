NETCOREDLL := --netcore 0

all: lsmsilcrtl.dll szip.tst
lsmsilcrtl.dll:
	-copy $(ORANGEC)\bin\lsmsilcrtl*.dll *.*

szip.tst: szip.exe
	szip szip.ref encode.tst
	szip -d encode.tst decode.tst
	fc /b encode.tst encode.cmpx
	fc /b decode.tst szip.ref

szip.exe:
	occil comp.c $(NETCOREDLL) /DMY_UNSIGNED_TYPES /oszip.exe /C+f	

clean:
	del *.exe
	del *.dll