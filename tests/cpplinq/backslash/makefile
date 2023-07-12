FILES = cpplinqtest.o

test: cpplinqtest.exe
	cpplinqtest > cpplinqtest.out
	fc /b ..\cpplinqtest.cmpx cpplinqtest.out

cpplinqtest.exe: ..\cpplinqtest.cpp
	occ /! /I ..\. $^

