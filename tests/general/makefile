%.o: %.cpp
	occ $^


%.o: %.c
	occ /9 $^

%.exe: %.o
	olink /c /T:CON32 /o$@ c0xpe.o $^ clwin.l climp.l

test: bzip2.exe parsecsv.exe stlcontainers.exe label.exe delegate.exe
	copy bzip2.map.cmp bzip2.map
	bzip2 bzip2.map
	fc /b bzip2.map.bz2 bzip2.map.bz2.cmp
	bzip2 -d bzip2.map.bz2
	fc /b bzip2.map bzip2.map.bz2
	del out.pcl
	parsecsv
	fc /b text.pcl out.pcl
	stlcontainers > stlcontainers.out
	fc /b stlcontainers.cmp stlcontainers.out
	del label1.exe
	label
	label1 > label1.out
	fc /b label1.cmp label1.out
	delegate > delegate.out
	fc /b delegate.cmp delegate.out
	