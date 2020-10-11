ifeq "$(TRAVIS_OS_NAME)" "linux"
export COMPILER:=gcc-linux

all:
	make -C src -f ./makefile
else
export ComSpec=c:\windows\system32\cmd.exe
export ORANGEC=c:\orangec
export ORIGPATH=$(ORANGEC)\src
export LADSOFT_DEV=c:\cc386
export APPVEYOR_BUILD_VERSION=6.0.45.$TRAVIS_BUILD_NUMBER
export PATH=$(ORANGEC)\bin;%PATH%;;C:\Program Files (x86)\Inno Setup 6
all:
	mkdir /c/orangec
	mkdir /c/orangec/temp
	mv doc /c/orangec
	mv license /c/orangec
	mv src /c/orangec
	mv tests /c/orangec
	mv *.* /c/orangec  
	$(ComSpec) /C c:\orangec\src\build.bat
endif
