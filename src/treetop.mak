# Software License Agreement
# 
#     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
# 
#     This file is part of the Orange C Compiler package.
# 
#     The Orange C Compiler package is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version, with the addition of the 
#     Orange C "Target Code" exception.
# 
#     The Orange C Compiler package is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
# 
#     You should have received a copy of the GNU General Public License
#     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
# 
#     contact information:
#         email: TouchStone222@runbox.com <David Lindauer>
# 

SHELL=cmd.exe
export SHELL


PATHSWAP = $(subst /,\,$(1))
export PATHSWAP

TREETOP := $(call PATHSWAP,$(dir $(_TREEROOT)))

DISTROOT := $(TREETOP)..

export DISTROOT

_TARGETDIR:= $(call PATHSWAP,$(CURDIR))

TEST := $(shell dir /b "$(_TARGETDIR)\dirs.mak")
ifeq "$(TEST)" "dirs.mak"
include $(_TARGETDIR)\dirs.mak
endif

define SUFFIXDIRS =
	$(foreach dir, $(1), $(dir)$(2))
endef


LIBS:= $(call SUFFIXDIRS,$(DIRS),.library)
EXES:= $(call SUFFIXDIRS,$(DIRS),.exefile)
CLEANS:= $(call SUFFIXDIRS,$(DIRS),.clean)
DISTS:= $(call SUFFIXDIRS,$(DIRS),.dist)

all: files

NULLDEV := NUL


del:
	-del /Q  $(_OUTPUTDIR)\*.* 2> $(NULLDEV)
	-del /Q *.exe 2> $(NULLDEV)
mkdir:
	-mkdir  $(_OUTPUTDIR) 2> $(NULLDEV)
rmdir:
	-rmdir  $(_OUTPUTDIR) 2> $(NULLDEV)


TEST := $(shell dir /b "$(_TARGETDIR)\makefile")
ifeq "$(TEST)" "makefile"
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

ifeq "$(NAME)" ""
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

cleanDISTRIBUTE: copydir.exe restub.exe renseg.exe pepatch.exe
ifndef NOMAKEDIR
	-mkdir $(DISTROOT) 2> $(NULLDEV)
#	-del /Q $(DISTROOT) 2> $(NULLDEV)
	-mkdir $(DISTROOT)\rule 2> $(NULLDEV)
	-del /Q $(DISTROOT)\rule 2> $(NULLDEV)
	-mkdir $(DISTBIN) 2> $(NULLDEV)
	-del /Q $(DISTBIN) 2> $(NULLDEV)
#	-mkdir $(DISTBIN_8) 2> $(NULLDEV)
#	-del /Q $(DISTBIN_8) 2> $(NULLDEV)
#	-mkdir $(DISTBIN_8)\branding 2> $(NULLDEV)
#	-del /Q $(DISTBIN_8)\branding 2> $(NULLDEV)
#	-mkdir $(DISTBIN_7) 2> $(NULLDEV)
#	-del /Q $(DISTBIN_7) 2> $(NULLDEV)
#	-mkdir $(DISTBIN_7)\branding 2> $(NULLDEV)
#	-del /Q $(DISTBIN_7)\branding 2> $(NULLDEV)
	-mkdir $(DISTHELP) 2> $(NULLDEV)
	-del /Q $(DISTHELP) 2> $(NULLDEV)
	-mkdir $(DISTINC) 2> $(NULLDEV)
	-del /Q $(DISTINC) 2> $(NULLDEV)
	-mkdir $(DISTINC)\sys 2> $(NULLDEV)
	-del /Q $(DISTINC)\sys 2> $(NULLDEV)
	-mkdir $(DISTINCWIN) 2> $(NULLDEV)
	-del /Q $(DISTINCWIN) 2> $(NULLDEV)
	-mkdir $(DISTINCSTL) 2> $(NULLDEV)
	-del /Q $(DISTINCSTL) 2> $(NULLDEV)
	-mkdir $(DISTINCSTL)\stlport 2> $(NULLDEV)
	-del /Q $(DISTINCSTL)\stlport 2> $(NULLDEV)
	-mkdir $(DISTINCSTL)\stlport\config 2> $(NULLDEV)
	-del /Q $(DISTINCSTL)\stlport\config 2> $(NULLDEV)
	-mkdir $(DISTINCSTL)\stlport\debug 2> $(NULLDEV)
	-del /Q $(DISTINCSTL)\stlport\debug 2> $(NULLDEV)
	-mkdir $(DISTINCSTL)\stlport\pointers 2> $(NULLDEV)
	-del /Q $(DISTINCSTL)\stlport\pointers 2> $(NULLDEV)
	-mkdir $(DISTINCSTL)\using 2> $(NULLDEV)
	-del /Q $(DISTINCSTL)\using 2> $(NULLDEV)
	-mkdir $(DISTINC)\stl 2> $(NULLDEV)
	-del /Q $(DISTINC)\stl 2> $(NULLDEV)
	-del /Q $(DISTLIB) 2> $(NULLDEV)
	-mkdir $(DISTSTARTUP) 2> $(NULLDEV)
	-del /Q $(DISTSTARTUP) 2> $(NULLDEV)
	-mkdir $(DISTSTARTUPDOS) 2> $(NULLDEV)
	-del /Q $(DISTSTARTUPDOS) 2> $(NULLDEV)
	-mkdir $(DISTSTARTUPWIN) 2> $(NULLDEV)
	-del /Q $(DISTSTARTUPWIN) 2> $(NULLDEV)
	-mkdir $(DISTADDON) 2> $(NULLDEV)
	-del /Q $(DISTADDON) 2> $(NULLDEV)
	-mkdir $(DISTDOC) 2> $(NULLDEV)
	-del /Q $(DISTDOC) 2> $(NULLDEV)	
	-mkdir $(DISTDOC)\general 2> $(NULLDEV)
	-del /Q $(DISTDOC)\general 2> $(NULLDEV)
	-mkdir $(DISTDOC)\oasm 2> $(NULLDEV)
	-del /Q $(DISTDOC)\oasm 2> $(NULLDEV)
	-mkdir $(DISTDOC)\occ 2> $(NULLDEV)
	-del /Q $(DISTDOC)\occ 2> $(NULLDEV)
	-mkdir $(DISTDOC)\ogrep 2> $(NULLDEV)
	-del /Q $(DISTDOC)\ogrep 2> $(NULLDEV)
	-mkdir $(DISTDOC)\olink 2> $(NULLDEV)
	-del /Q $(DISTDOC)\olink 2> $(NULLDEV)
	-mkdir $(DISTDOC)\omake 2> $(NULLDEV)
	-del /Q $(DISTDOC)\omake 2> $(NULLDEV)
	-mkdir $(DISTEXAM) 2> $(NULLDEV)
	-del /Q $(DISTEXAM) 2> $(NULLDEV)
	-mkdir $(DISTEXAM)\msdos 2> $(NULLDEV)
	-del /Q $(DISTEXAM)\msdos 2> $(NULLDEV)
	-mkdir $(DISTEXAM)\system 2> $(NULLDEV)
	-del /Q $(DISTEXAM)\system 2> $(NULLDEV)
	-mkdir $(DISTEXAM)\win32 2> $(NULLDEV)
	-del /Q $(DISTEXAM)\win32 2> $(NULLDEV)
	-mkdir $(DISTEXAM)\win32\atc 2> $(NULLDEV)
	-del /Q $(DISTEXAM)\win32\atc 2> $(NULLDEV)
	-mkdir $(DISTEXAM)\win32\listview 2> $(NULLDEV)
	-del /Q $(DISTEXAM)\win32\listview 2> $(NULLDEV)
	-mkdir $(DISTEXAM)\win32\xmlview 2> $(NULLDEV)
	-del /Q $(DISTEXAM)\win32\xmlview 2> $(NULLDEV)
	-mkdir $(DISTEXAM)\win32\RCDemo 2> $(NULLDEV)
	-del /Q $(DISTEXAM)\win32\RCDemo 2> $(NULLDEV)
	-mkdir $(DISTEXAM)\win32\huff 2> $(NULLDEV)
	-del /Q $(DISTEXAM)\win32\huff 2> $(NULLDEV)
	-mkdir $(DISTDIST) 2> $(NULLDEV)
endif

makelibdir:
	-mkdir  $(_LIBDIR) 2> $(NULLDEV)


$(LIBS): %.library :
	$(MAKE) library -f $(_TREEROOT) -C$*
$(EXES): %.exefile :
	$(MAKE) exefile -f $(_TREEROOT) -C$*


files: makelibdir $(LIBS) $(EXES) compile link

library: $(LIBS)

exefile: $(EXES)

cleanstart:
	-del /Q $(_LIBDIR)
	-rmdir $(_LIBDIR)

$(CLEANS): %.clean :
	$(MAKE) clean -f $(_TREEROOT) -C$*

clean: cleanstart $(CLEANS)

else

$(LIBS): %.library : 
	$(MAKE) library -f $(_TREEROOT) -C$*

$(EXES): %.exefile : 
	$(MAKE) exefile -f $(_TREEROOT) -C$*

library: mkdir compile $(LIBS)

exefile: mkdir link $(EXES)


cleanDISTRIBUTE:

$(CLEANS): %.clean :
	$(MAKE) clean -f $(_TREEROOT) -C$*
clean: del rmdir $(CLEANS)

endif

ifndef BUILDENV
exeDISTRIBUTE: $(DISTS)
$(DISTS): %.dist :
	$(MAKE) distribute -f $(_TREEROOT) -C$*
else
exeDISTRIBUTE:
endif

distribute: cleanDISTRIBUTE exeDISTRIBUTE DISTRIBUTE   
