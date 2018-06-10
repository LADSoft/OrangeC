all: test 

test: errchk.o
	fc /b tst.txt cmpx.tst

errchk.o: errchk.c
	occ -c errchk.c > tst.txt
