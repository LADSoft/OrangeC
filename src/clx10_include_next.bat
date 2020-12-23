	mkdir \orangec\include\c
	mkdir \orangec\include\c\sys
	mkdir \orangec\include\c\win32
        del /Q \orangec\include
        del /Q \orangec\include\sys
        del /Q \orangec\include\win32
	copy "clibs\cpp\libcxx\include\*." \orangec\include
	copy "clibs\cpp\libcxx\include\*.h" \orangec\include
	copy "clibs\cpp\stdinc_libcxx\*." \orangec\include
	copy "clibs\cpp\stdinc_libcxx\*.h" \orangec\include
        copy "clibs\platform\win32\wininc\*.h" \orangec\include\c
        copy "clibs\platform\win32\wininc\win32\*.h" \orangec\include\c\win32
        copy clibs\stdinc\*.h \orangec\include\c
        copy clibs\stdinc\sys\*.h \orangec\include\c\sys
	copy occ\target.cfg \orangec\bin\occ.cfg
	copy occil\target.cfg \orangec\bin\occil.cfg
	copy ocpp\target.cfg \orangec\bin\ocpp.cfg
	copy orc\target.cfg \orangec\bin\orc.cfg