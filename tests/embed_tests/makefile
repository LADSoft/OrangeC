all: test_replicates test_if_empty test_prefix test_postfix test_single fails limits

test_replicates: replicates.exe
	.\replicates.exe
	fc /b Tillman.jpg Tillman_testout.jpg
test_if_empty: if_empty.exe
	.\if_empty.exe
	fc /b if_empty_expected.txt if_empty_out.txt
test_prefix: prefix.exe
	.\prefix.exe
	fc /b prefix_postfix_expected.txt prefix_out.txt
test_postfix: suffix.exe
	.\suffix.exe
	fc /b prefix_postfix_expected.txt postfix_out.txt
test_single: single.exe
	.\single.exe
	fc /b single.txt single_out.txt

offset.exe:
	occ /2 offset.c -o offset.exe
test_offset: offset.exe
	.\offset.exe
	fc /b offset_expected.txt offset_out.txt

fails.exe:
	-occ fails.cpp -o fails.exe > NUL 2>&1
fails: fails.exe
ifeq (fails.exe,$(wildcard fails.exe))
    $(error fails.exe exists!)
endif
limits: limits.exe
	.\limits.exe
clean:
	-del *.o
	-del *.obj
	-del *.exe
	-del Tillman_testout.jpg
	-del *_out.txt
