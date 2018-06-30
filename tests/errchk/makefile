all: test 

test: ofiles
	fc /b errchk.txt errchk.cmpx
	fc /b allwarn.txt allwarn.cmpx
	fc /b nowarn.txt nowarn.cmpx
	fc /b no105.txt no105.cmpx
	fc /b no1000.txt no1000.cmpx
	fc /b no11.txt no11.cmpx
	fc /b once105.txt once105.cmpx
	fc /b aserr.txt aserr.cmpx
	fc /b aserr2.txt aserr2.cmpx
	fc /b only141.txt only141.cmpx
	fc /b pragma.txt pragma.cmpx
	fc /b goto.txt goto.cmpx
ofiles:
	occ -c --nologo errchk.c > errchk.txt
	occ -c --nologo /w+ errparam.c > allwarn.txt
	occ -c --nologo /w errparam.c > nowarn.txt
	occ -c --nologo /wd105 errparam.c > no105.txt
	occ -c --nologo /wd1000 errparam.c > no1000.txt
	occ -c --nologo /wd11 errparam.c > no11.txt
	occ -c --nologo /wo105 errparam.c > once105.txt
	-occ -c --nologo /wx errparam.c >aserr.txt
	-occ -c --nologo /werror errparam.c > aserr2.txt
	occ -c --nologo /w /w141 errparam.c > only141.txt
	occ -c --nologo errpragma.c > pragma.txt 
	occ -c --nologo goto.c > goto.txt
