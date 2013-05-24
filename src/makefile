#	Software License Agreement (BSD License)
#	
#	Copyright (c) 1997-2009, David Lindauer, (LADSoft).
#	All rights reserved.
#	
#	Redistribution and use of this software in source and binary forms, 
#	with or without modification, are permitted provided that the following 
#	conditions are met:

#	* Redistributions of source code must retain the above
#	  copyright notice, this list of conditions and the
#	  following disclaimer.

#	* Redistributions in binary form must reproduce the above
#	  copyright notice, this list of conditions and the
#	  following disclaimer in the documentation and/or other
#	  materials provided with the distribution.

#	* Neither the name of LADSoft nor the names of its
#	  contributors may be used to endorse or promote products
#	  derived from this software without specific prior
#	  written permission of LADSoft.

#	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
#	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
#	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
#	PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
#	OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
#	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
#	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
#	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
#	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
#	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#	contact information:
#		email: TouchStone222@runbox.com <David Lindauer>
all: files

DIRS= util obj obrc ocpp ogrep oasm dlhex dlle dlmz dlpe dlpm olib olink \
    oimplib omake orc ocl ocide occ\x86 occ\preproc occ\parse86 help dos

SYSINCLIDE=c:\bc5\include
export SYSINCLUDE

CPPMAKE:=$(realpath .\cpp.mak)
export CPPMAKE
DISTMAKE:=$(realpath .\dist.mak)
export DISTMAKE

ZIP:=7z -tzip 
export ZIP

OBJECT:=$(realpath object)
export OBJECT

define DOLIB
	$$(MAKE) /C$(dir) LIBRARY
endef

define DOEXE
	$$(MAKE) /C$(dir) EXEFILE
endef

define filesdef
files: copydir.exe restub.exe renseg.exe pepatch.exe
	-mkdir $(OBJECT)
	$(foreach dir, $(DIRS), $(DOLIB))
	$(foreach dir, $(DIRS), $(DOEXE))
endef


$(eval $(filesdef))

DISTROOT=C:\orangec
export DISTROOT
DISTBIN=$(DISTROOT)\bin
export DISTBIN
DISTHELP=$(DISTROOT)\help
export DISTHELP
DISTINC=$(DISTROOT)\include
export DISTINC
DISTINCWIN=$(DISTINC)\win32
export DISTINCWIN
DISTLIB=$(DISTROOT)\lib
export DISTLIB
DISTSTARTUP=$(DISTROOT)\lib\startup
export DISTSTARTUP
DISTSTARTUPDOS=$(DISTROOT)\lib\startup\msdos
export DISTSTARTUPDOS
DISTSTARTUPWIN=$(DISTROOT)\lib\startup\win32
export DISTSTARTUPWIN
DISTADDON=$(DISTROOT)\addon
export DISTADDON
DISTDOC=$(DISTROOT)\doc
export DISTDOC
DISTLIC=$(DISTROOT)\license
export DISTLIC
DISTEXAM=$(DISTROOT)\examples
export DISTEXAM
DISTDIST=$(DISTROOT)\dist
export DISTDIST

ifdef MS
    RELEASEPATH = $(DISTROOT)\src\release
else
    RELEASEPATH= .
endif

export RELEASEPATH

define DODIST
	$$(MAKE) /C$(dir) DISTRIBUTE
endef

copydir.exe: copydir.c
	bcc32 $^

restub.exe: restub.cpp
	bcc32 $^

renseg.exe:renseg.cpp
	bcc32 $^

pepatch.exe:pepatch.c
	bcc32 -Idlpe $^

COPYDIR := $(realpath .\copydir.exe)

define distdef
DISTRIBUTE:
	-mkdir $(DISTROOT)
#	-del /Q $(DISTROOT)
	-mkdir $(DISTROOT)\rule
	-del /Q $(DISTROOT)\rule
	-mkdir $(DISTBIN)
	-del /Q $(DISTBIN)
	-mkdir $(DISTHELP)
	-del /Q $(DISTHELP)
	-mkdir $(DISTINC)
	-del /Q $(DISTINC)
	-mkdir $(DISTINC)\sys
	-del /Q $(DISTINC)\sys
	-mkdir $(DISTINCWIN)
	-del /Q $(DISTINCWIN)
	-mkdir $(DISTLIB)
	-del /Q $(DISTLIB)
	-mkdir $(DISTSTARTUP)
	-del /Q $(DISTSTARTUP)
	-mkdir $(DISTSTARTUPDOS)
	-del /Q $(DISTSTARTUPDOS)
	-mkdir $(DISTSTARTUPWIN)
	-del /Q $(DISTSTARTUPWIN)
	-mkdir $(DISTADDON)
	-del /Q $(DISTADDON)
	-mkdir $(DISTDOC)
	-del /Q $(DISTDOC)
	-mkdir $(DISTDOC)\general
	-del /Q $(DISTDOC)\general
	-mkdir $(DISTDOC)\oasm
	-del /Q $(DISTDOC)\oasm
	-mkdir $(DISTDOC)\occ
	-del /Q $(DISTDOC)\occ
	-mkdir $(DISTDOC)\ogrep
	-del /Q $(DISTDOC)\ogrep
	-mkdir $(DISTDOC)\omake
	-del /Q $(DISTDOC)\omake
	-mkdir $(DISTLIC)
	-del /Q $(DISTLIC)
	-mkdir $(DISTEXAM)
	-del /Q $(DISTEXAM)
	-mkdir $(DISTEXAM)\msdos
	-del /Q $(DISTEXAM)\msdos
	-mkdir $(DISTEXAM)\system
	-del /Q $(DISTEXAM)\system
	-mkdir $(DISTEXAM)\win32
	-del /Q $(DISTEXAM)\win32
	-mkdir $(DISTEXAM)\win32\listview
	-del /Q $(DISTEXAM)\win32\listview
	-mkdir $(DISTEXAM)\win32\xmlview
	-del /Q $(DISTEXAM)\win32\xmlview
	-mkdir $(DISTEXAM)\win32\RCDemo
	-del /Q $(DISTEXAM)\win32\RCDemo
	-mkdir $(DISTDIST)
	$(foreach dir, $(DIRS), $(DODIST))
	$(COPYDIR) examples\*.* $(DISTEXAM)
	$(COPYDIR) examples\msdos\*.* $(DISTEXAM)\msdos
	$(COPYDIR) examples\system\*.* $(DISTEXAM)\system
	$(COPYDIR) examples\win32\*.* $(DISTEXAM)\win32
	$(COPYDIR) examples\win32\listview\*.* $(DISTEXAM)\win32\listview
	$(COPYDIR) examples\win32\xmlview\*.* $(DISTEXAM)\win32\xmlview
	$(COPYDIR) examples\win32\RCDemo\*.* $(DISTEXAM)\win32\RCDemo
	$(COPYDIR) doc\*.* $(DISTDOC)
	$(COPYDIR) doc\general\*.* $(DISTDOC)\general
	$(COPYDIR) doc\oasm\*.* $(DISTDOC)\oasm
	$(COPYDIR) doc\occ\*.* $(DISTDOC)\occ
	$(COPYDIR) doc\ogrep\*.* $(DISTDOC)\ogrep
	$(COPYDIR) doc\omake\*.* $(DISTDOC)\omake
	copy license.txt $(DISTROOT)
	copy readme.txt $(DISTROOT)
	copy relnotes.txt $(DISTROOT)
	copy berkely.lic $(DISTLIC)
	copy copying $(DISTLIC)
	copy ocl.lic $(DISTLIC)
	copy addon.txt $(DISTADDON)
ifdef BUILDENV
	$(MAKE) /Cclibs DISTRIBUTE /DBUILDENV
else
	$(MAKE) /Cclibs DISTRIBUTE
# commented out because this is incomplete - you need to build DFLAT with CC386
#	$(MAKE) /C..\ -f$(realpath .\doszip.mak)
#	$(MAKE) /Cdos\install -fmakefile.le
	$(MAKE) /Cc:\ -f$(realpath .\zip.mak)
endif

endef

$(eval $(distdef))

export COPYDIR

PEPATCH:=$(realpath .\pepatch.exe)
export PEPATCH

RESTUB:=$(realpath .\restub.exe)
export RESTUB

RENSEG:=$(realpath .\renseg.exe)
export RENSEG

STUB:=$(realpath clibs\platform\dos32\extender\hx\dpmist32.bin)
export STUB