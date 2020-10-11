ifeq "$(TRAVIS_OS_NAME)" "linux"
export COMPILER:=gcc-linux

all:
	make -C src -f ./makefile
else
export PATH:=$(PATH);c:\orangec\bin;c:\program files7-zip;c:\program files (x86)\inno setup 6
export ComSpec=c:\windows\system32\cmd.exe
all:
	mkdir c:\orangec
	mv src c:\orangec
	cd c:\orangec
	$(ComSpec) /C build.bat
endif
