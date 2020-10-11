ifeq "$(TRAVIS_OS_NAME)" "linux"
export COMPILER:=gcc-linux

all:
	make -C src -f ./makefile
else
export PATH:=$(PATH);c:\orangec\bin;c:\program files7-zip;c:\program files (x86)\inno setup 6
all:
	mkdir c:\orangec
	move src c:\orangec
	cd c:\orangec\src
	$(ComSpec) -C build.bat
endif
