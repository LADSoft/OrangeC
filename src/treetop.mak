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
#		email: TouchStone222runbox.com <David Lindauer>

TREETOP := $(subst /,\,$(dir $(_TREEROOT)))
DISTROOT := $(TREETOP)..

export DISTROOT

all: files

NULLDEV := NUL


del:
	-del /Q  $(_OUTPUTDIR)\*.* >> $(NULLDEV)
mkdir:
	-mkdir  $(_OUTPUTDIR) >> $(NULLDEV)
rmdir:
	-rmdir  $(_OUTPUTDIR) >> $(NULLDEV)

_TARGETDIR:= $(_TREETARGET)

define DOLIB
	$(MAKE) -C$(dir) -f$(_TREEROOT) -D_TREETARGET=$(_TARGETDIR)\$(dir) -DLIBRARY
endef
define DOEXE
	$(MAKE) -C$(dir) -f$(_TREEROOT) -D_TREETARGET=$(_TARGETDIR)\$(dir) -DEXEFILE
endef

define DODISTRIBUTE
	$(MAKE) distribute -C$(dir) -f$(_TREEROOT) -D_TREETARGET=$(_TARGETDIR)\$(dir)
endef
define DOCLEAN
	$(MAKE) clean -C$(dir) -f$(_TREEROOT) -D_TREETARGET=$(_TARGETDIR)\$(dir)
endef

ifeq "$(exists $(_TARGETDIR)\dirs.mak)" "1"
include $(_TARGETDIR)\dirs.mak
endif

ifeq "$(exists $(_TARGETDIR)\makefile)" "1"
include $(_TARGETDIR)\makefile
include $(DISTROOT)\src\dist.mak
else
DISTRIBUTE:

endif


ifeq "$(MAIN_DEPENDENCIES)" "$(MAIN_FILE)"
link:
else
link: $(NAME).exe
endif

_OUTPUTDIR=$(_TARGETDIR)\obj\$(OBJ_IND_PATH)
export _OUTPUTDIR
_LIBDIR=$(DISTROOT)\src\lib\$(OBJ_IND_PATH)
export _LIBDIR

include $(TREETOP)config.mak

export LIB_EXT
export LIB_PREFIX


ifndef NAME
compile:
else
compile: $(LLIB_DEPENDENCIES) $(_LIBDIR)\$(LIB_PREFIX)$(NAME)$(LIB_EXT)
endif

ifndef _STARTED
_STARTED = 1
export _STARTED
export _TREEROOT
DISTBIN=$(DISTROOT)\bin
export DISTBIN
#DISTBIN_7=$(DISTROOT)\bin_7
#export DISTBIN_7
#DISTBIN_8=$(DISTROOT)\bin_8
#export DISTBIN_8
DISTHELP=$(DISTROOT)\help
export DISTHELP
DISTINC=$(DISTROOT)\include
export DISTINC
DISTINCWIN=$(DISTINC)\win32
export DISTINCWIN
DISTINCSTL=$(DISTINC)\stlport
export DISTINCSTL
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

COPYDIR := $(realpath $(DISTROOT)\src\copydir.exe)
export COPYDIR

PEPATCH:=$(realpath $(DISTROOT)\src\pepatch.exe)
export PEPATCH

RESTUB:=$(realpath $(DISTROOT)\src\restub.exe)
export RESTUB

RENSEG:=$(realpath $(DISTROOT)\src\renseg.exe)
export RENSEG

STUB:=$(realpath $(DISTROOT)\src\clibs\platform\dos32\extender\hx\dpmist32.bin)
export STUB

DISTMAKE := $(realpath $(DISTROOT)\src\dist.mak)
export DISTMAKE

define filesdef

cleanDISTRIBUTE: copydir.exe restub.exe renseg.exe pepatch.exe
ifndef NOMAKEDIR
	-mkdir $(DISTROOT) >> $(NULLDEV)
#	-del /Q $(DISTROOT) >> $(NULLDEV)
	-mkdir $(DISTROOT)\rule >> $(NULLDEV)
	-del /Q $(DISTROOT)\rule >> $(NULLDEV)
	-mkdir $(DISTBIN) >> $(NULLDEV)
	-del /Q $(DISTBIN) >> $(NULLDEV)
#	-mkdir $(DISTBIN_8) >> $(NULLDEV)
#	-del /Q $(DISTBIN_8) >> $(NULLDEV)
#	-mkdir $(DISTBIN_8)\branding >> $(NULLDEV)
#	-del /Q $(DISTBIN_8)\branding >> $(NULLDEV)
#	-mkdir $(DISTBIN_7) >> $(NULLDEV)
#	-del /Q $(DISTBIN_7) >> $(NULLDEV)
#	-mkdir $(DISTBIN_7)\branding >> $(NULLDEV)
#	-del /Q $(DISTBIN_7)\branding >> $(NULLDEV)
	-mkdir $(DISTHELP) >> $(NULLDEV)
	-del /Q $(DISTHELP) >> $(NULLDEV)
	-mkdir $(DISTINC) >> $(NULLDEV)
	-del /Q $(DISTINC) >> $(NULLDEV)
	-mkdir $(DISTINC)\sys >> $(NULLDEV)
	-del /Q $(DISTINC)\sys >> $(NULLDEV)
	-mkdir $(DISTINCWIN) >> $(NULLDEV)
	-del /Q $(DISTINCWIN) >> $(NULLDEV)
	-mkdir $(DISTINCSTL) >> $(NULLDEV)
	-del /Q $(DISTINCSTL) >> $(NULLDEV)
	-mkdir $(DISTINCSTL)\stlport >> $(NULLDEV)
	-del /Q $(DISTINCSTL)\stlport >> $(NULLDEV)
	-mkdir $(DISTINCSTL)\stlport\config >> $(NULLDEV)
	-del /Q $(DISTINCSTL)\stlport\config >> $(NULLDEV)
	-mkdir $(DISTINCSTL)\stlport\debug >> $(NULLDEV)
	-del /Q $(DISTINCSTL)\stlport\debug >> $(NULLDEV)
	-mkdir $(DISTINCSTL)\stlport\pointers >> $(NULLDEV)
	-del /Q $(DISTINCSTL)\stlport\pointers >> $(NULLDEV)
	-mkdir $(DISTINCSTL)\using >> $(NULLDEV)
	-del /Q $(DISTINCSTL)\using >> $(NULLDEV)
	-mkdir $(DISTINC)\stl >> $(NULLDEV)
	-del /Q $(DISTINC)\stl >> $(NULLDEV)
	-del /Q $(DISTLIB) >> $(NULLDEV)
	-mkdir $(DISTSTARTUP) >> $(NULLDEV)
	-del /Q $(DISTSTARTUP) >> $(NULLDEV)
	-mkdir $(DISTSTARTUPDOS) >> $(NULLDEV)
	-del /Q $(DISTSTARTUPDOS) >> $(NULLDEV)
	-mkdir $(DISTSTARTUPWIN) >> $(NULLDEV)
	-del /Q $(DISTSTARTUPWIN) >> $(NULLDEV)
	-mkdir $(DISTADDON) >> $(NULLDEV)
	-del /Q $(DISTADDON) >> $(NULLDEV)
	-mkdir $(DISTDOC) >> $(NULLDEV)
	-del /Q $(DISTDOC) >> $(NULLDEV)	
	-mkdir $(DISTDOC)\general >> $(NULLDEV)
	-del /Q $(DISTDOC)\general >> $(NULLDEV)
	-mkdir $(DISTDOC)\oasm >> $(NULLDEV)
	-del /Q $(DISTDOC)\oasm >> $(NULLDEV)
	-mkdir $(DISTDOC)\occ >> $(NULLDEV)
	-del /Q $(DISTDOC)\occ >> $(NULLDEV)
	-mkdir $(DISTDOC)\ogrep >> $(NULLDEV)
	-del /Q $(DISTDOC)\ogrep >> $(NULLDEV)
	-mkdir $(DISTDOC)\olink >> $(NULLDEV)
	-del /Q $(DISTDOC)\olink >> $(NULLDEV)
	-mkdir $(DISTDOC)\omake >> $(NULLDEV)
	-del /Q $(DISTDOC)\omake >> $(NULLDEV)
	-mkdir $(DISTLIC) >> $(NULLDEV)
	-del /Q $(DISTLIC) >> $(NULLDEV)
	-mkdir $(DISTEXAM) >> $(NULLDEV)
	-del /Q $(DISTEXAM) >> $(NULLDEV)
	-mkdir $(DISTEXAM)\msdos >> $(NULLDEV)
	-del /Q $(DISTEXAM)\msdos >> $(NULLDEV)
	-mkdir $(DISTEXAM)\system >> $(NULLDEV)
	-del /Q $(DISTEXAM)\system >> $(NULLDEV)
	-mkdir $(DISTEXAM)\win32 >> $(NULLDEV)
	-del /Q $(DISTEXAM)\win32 >> $(NULLDEV)
	-mkdir $(DISTEXAM)\win32\atc >> $(NULLDEV)
	-del /Q $(DISTEXAM)\win32\atc >> $(NULLDEV)
	-mkdir $(DISTEXAM)\win32\listview >> $(NULLDEV)
	-del /Q $(DISTEXAM)\win32\listview >> $(NULLDEV)
	-mkdir $(DISTEXAM)\win32\xmlview >> $(NULLDEV)
	-del /Q $(DISTEXAM)\win32\xmlview >> $(NULLDEV)
	-mkdir $(DISTEXAM)\win32\RCDemo >> $(NULLDEV)
	-del /Q $(DISTEXAM)\win32\RCDemo >> $(NULLDEV)
	-mkdir $(DISTEXAM)\win32\huff >> $(NULLDEV)
	-del /Q $(DISTEXAM)\win32\huff >> $(NULLDEV)
	-mkdir $(DISTDIST) >> $(NULLDEV)
endif

innerFiles:
	-mkdir  $(_LIBDIR) >> $(NULLDEV)
	$(foreach dir, $(DIRS), $(DOLIB))
	$(foreach dir, $(DIRS), $(DOEXE))

files: innerFiles compile link
endef

define cleandef
clean:
	-del /Q $(_LIBDIR)
	-rmdir $(_LIBDIR)
	$(foreach dir, $(DIRS), $(DOCLEAN))
endef

else

define filesdef
ifdef LIBRARY
files: mkdir compile
	$(foreach dir, $(DIRS), $(DOLIB))
endif
ifdef EXEFILE
files: mkdir link
	$(foreach dir, $(DIRS), $(DOEXE))
endif

cleanDISTRIBUTE:

endef
define cleandef
clean: del rmdir
	$(foreach dir, $(DIRS), $(DOCLEAN))
endef
endif

define distdef

exeDISTRIBUTE:
ifndef BUILDENV
	$(foreach dir, $(DIRS), $(DODISTRIBUTE))
endif

distribute: cleanDISTRIBUTE exeDISTRIBUTE DISTRIBUTE   
endef


$(eval $(filesdef))
$(eval $(distdef))
$(eval $(cleandef))
