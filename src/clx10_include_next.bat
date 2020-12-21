	mkdir \orangec\include\c
        del /Q \orangec\include
	copy "clibs\cpp\libcxx\include\*." \orangec\include
	copy "clibs\cpp\libcxx\include\*.h" \orangec\include
	copy "clibs\cpp\stdinc_libcxx\*." \orangec\include
	copy "clibs\cpp\stdinc_libcxx\*.h" \orangec\include
        copy "clibs\platform\win32\wininc\*.h" \orangec\include
        copy clibs\stdinc\*.h \orangec\include\c
        copy clibs\stdinc\sys\*.h \orangec\include\sys
	copy occ\target.cfg \orangec\bin\occ.cfg
	copy occil\target.cfg \orangec\bin\occil.cfg
	copy ocpp\target.cfg \orangec\bin\ocpp.cfgs
	copy orc\target.cfg \orangec\bin\orc.cfg