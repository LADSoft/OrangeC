	copy "\libcxx 10\libcxx-10.0.1.src\include\*." \orangec\include
	copy "\libcxx 10\libcxx-10.0.1.src\include\*.h" \orangec\include
	copy "\libcxx 10\libcxx-10.0.1.src\include\support\win32\*.h" \orangec\include\support\win32
	copy "clibs\cpp\stdinc_libcxx\*." \orangec\include
	copy "clibs\cpp\stdinc_libcxx\*.h" \orangec\include
        copy clibs\stdinc\*.h \orangec\include
        copy clibs\stdinc\sys\*.h \orangec\include\sys
