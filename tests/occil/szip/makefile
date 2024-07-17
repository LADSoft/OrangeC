ifneq "$(NETCORE)" ""
NETCOREDLL := --netcore 0
endif

all: lsmsilcrtl.dll comp.tst
lsmsilcrtl.dll:
	-copy $(ORANGEC)\bin\lsmsilcrtl*.dll *.*

comp.tst: comp.exe
	szip szip.ref encode.tst
	szip -d encode.tst decode.tst
	fc /b encode.tst encode.cmpx
	fc /b decode.tst szip.ref

comp.exe:
	occil comp.c $(NETCOREDLL) /DMY_UNSIGNED_TYPES /oszip.exe /C+f	

clean:
	del *.exe
	del *.dll