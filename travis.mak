ifeq "$(TRAVIS_OS_NAME)" "linux"
export COMPILER:=gcc-linux

all:
	make -C src -f ./makefile
else
export PATH:=$(PATH);c:\orangec\bin;c:\program files7-zip;c:\program files (x86)\inno setup 6
export ComSpec=c:\windows\system32\cmd.exe
export APPVEYOR_VERSION=6.0.45.$(TRAVIS_BUILDNUMBER)
all:
        cd ..
        mv orangec /c/    
	$(ComSpec) /C c:\orangec\src\build.bat
endif
