ifeq "$(TRAVIS_OS_NAME)" "linux"
export COMPILER:=gcc-linux

all:
	make -C src -f ./makefile
else
all:
	mkdir /c/orangec
        move src /c/orangec
        cd /c/orangec/src
        PATH=$PATH:/c/orangec/bin:/c/program files/7-zip:/c/program files (x86)/inno setup 5
	omake fullbuild
endif
