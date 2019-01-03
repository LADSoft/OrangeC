%.o: %.cpp
	occ /! $^


%.o: %.c
	occ /9 /! $^

%.exe: %.o
	olink /c /! /T:CON32 /o$@ c0xpe.o $^ clwin.l climp.l

test: bzip2.exe parsecsv.exe label.exe delegate.exe
	bzip2 bzip2.test
	fc /b bzip2.test.bz2 bzip2.test.bz2.cmpx
	bzip2 -d bzip2.test.bz2
	fc /b bzip2.test bzip2.test.cmpx
	del out.pcl
	parsecsv
	fc /b test.cmpx out.pcl
	del label1.exe
	label
	label1 > label1.out
	fc /b label1.cmpx label1.out
	delegate > delegate.out
	fc /b delegate.cmpx delegate.out
	