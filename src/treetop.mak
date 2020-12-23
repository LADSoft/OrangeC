# Software License Agreement
# 
#     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
# 
#     This file is part of the Orange C Compiler package.
# 
#     The Orange C Compiler package is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
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

ifeq "$(PATHEXT2)" ""
include pathext2.mak
endif


ifneq "$(COMPILER)" "gcc-linux"
SHELL=cmd.exe
export SHELL
endif

ifeq "$(COMPILER)" "gcc-linux"
MKDIR := mkdir -p
else
MKDIR := mkdir
endif
all:

TREETOP := $(dir $(_TREEROOT))

DISTROOT := $(TREETOP)..

export DISTROOT

ifeq "$(COMPILER)" "gcc-linux"
CURRENT := $(CURDIR)
else
CURRENT := $(subst /,\,$(CURDIR))

endif

_TARGETDIR:= $(CURRENT)

ifneq "$(wildcard $(_TARGETDIR)$(PATHEXT2)dirs.mak)" ""
include $(_TARGETDIR)$(PATHEXT2)dirs.mak
endif
	
COMPARER:=fc /b


LIBS:= $(addsuffix .library,$(DIRS))
EXES:= $(addsuffix .exefile,$(DIRS))
CLEANS:= $(addsuffix .clean,$(DIRS))
DISTS:= $(addsuffix .dist,$(DIRS))
DISTS1:= $(addsuffix .dist1,$(DIRS))
CDIRS:= $(addsuffix .dirs,$(DIRS))
FORMATS:= $(addsuffix .format,$(DIRS))
COMPARES:= $(addsuffix .comp,$(DIRS))

ifeq "$(COMPILER)" "gcc-linux"
NULLDEV := /dev/null
else
NULLDEV := NUL
endif

del:
	-del /Q  $(_OUTPUTDIR)$(PATHEXT2)*.* 2> $(NULLDEV)
	-del /Q *.exe 2> $(NULLDEV)
	-del /Q *.o 2> $(NULLDEV)
	-del /Q *.odx 2> $(NULLDEV)
mkdir:
	-$(MKDIR)  $(_OUTPUTDIR) 2> $(NULLDEV)
rmdir:
	-rmdir  $(_OUTPUTDIR) 2> $(NULLDEV)


ifneq "$(wildcard $(_TARGETDIR)$(PATHEXT2)makefile)" ""
include $(_TARGETDIR)$(PATHEXT2)makefile
include $(DISTROOT)$(PATHEXT2)src$(PATHEXT2)dist.mak
else
DISTRIBUTE:

endif


ifeq "$(MAIN_DEPENDENCIES)" "$(MAIN_FILE)"
link:
else
link: $(NAME).exe
endif

_OUTPUTDIR = $(_TARGETDIR)$(PATHEXT2)obj$(PATHEXT2)$(OBJ_IND_PATH)
export _OUTPUTDIR
_LIBDIR = $(DISTROOT)$(PATHEXT2)src$(PATHEXT2)lib$(PATHEXT2)$(OBJ_IND_PATH)
export _LIBDIR

include $(TREETOP)config.mak

export LIB_EXT
export LIB_PREFIX

ifeq "$(NAME)" ""
compile:
else
compile: $(_LIBDIR)$(PATHEXT2)$(LIB_PREFIX)$(NAME)$(LIB_EXT)
endif


ifndef _STARTED
_STARTED = 1
export _STARTED
export _TREEROOT
DISTBIN=$(DISTROOT)$(PATHEXT2)bin
export DISTBIN
#DISTBIN_7=$(DISTROOT)$(PATHEXT2)bin_7
#export DISTBIN_7
#DISTBIN_8=$(DISTROOT)$(PATHEXT2)bin_8
#export DISTBIN_8
DISTHELP=$(DISTROOT)$(PATHEXT2)help
export DISTHELP
DISTINC=$(DISTROOT)$(PATHEXT2)include
export DISTINC
DISTINCWIN=$(DISTINC)$(PATHEXT2)c$(PATHEXT2)win32
export DISTINCWIN
DISTINCSTL=$(DISTINC)$(PATHEXT2)stlport
export DISTINCSTL
DISTLIB=$(DISTROOT)$(PATHEXT2)lib
export DISTLIB
DISTSTARTUP=$(DISTROOT)$(PATHEXT2)lib$(PATHEXT2)startup
export DISTSTARTUP
DISTSTARTUPDOS=$(DISTROOT)$(PATHEXT2)lib$(PATHEXT2)startup$(PATHEXT2)msdos
export DISTSTARTUPDOS
DISTSTARTUPWIN=$(DISTROOT)$(PATHEXT2)lib$(PATHEXT2)startup$(PATHEXT2)win32
export DISTSTARTUPWIN
DISTADDON=$(DISTROOT)$(PATHEXT2)addon
export DISTADDON
DISTEXAM=$(DISTROOT)$(PATHEXT2)examples
export DISTEXAM
DISTDIST=$(DISTROOT)$(PATHEXT2)dist
export DISTDIST

ifdef MS
    RELEASEPATH = $(DISTROOT)$(PATHEXT2)src$(PATHEXT2)release
else
    RELEASEPATH= .
endif

export RELEASEPATH

COPYDIR := $(realpath $(DISTROOT)$(PATHEXT2)src$(PATHEXT2)copydir.exe)
export COPYDIR

PEPATCH:=$(realpath $(DISTROOT)$(PATHEXT2)src$(PATHEXT2)pepatch.exe)
export PEPATCH

RESTUB:=$(realpath $(DISTROOT)$(PATHEXT2)src$(PATHEXT2)restub.exe)
export RESTUB

RENSEG:=$(realpath $(DISTROOT)$(PATHEXT2)src$(PATHEXT2)renseg.exe)
export RENSEG

STUB:=$(realpath $(DISTROOT)$(PATHEXT2)src$(PATHEXT2)clibs$(PATHEXT2)platform$(PATHEXT2)dos32$(PATHEXT2)extender$(PATHEXT2)hx$(PATHEXT2)dpmist32.bin)
export STUB

DISTMAKE := $(realpath $(DISTROOT)$(PATHEXT2)src$(PATHEXT2)dist.mak)
export DISTMAKE

cleanDISTRIBUTE: copydir.exe restub.exe renseg.exe pepatch.exe
	-del /Q $(DISTBIN)\*.pdb
ifndef NOMAKEDIR
	-$(MKDIR) $(DISTROOT) 2> $(NULLDEV)
#	-del /Q $(DISTROOT) 2> $(NULLDEV)
#	-$(MKDIR) $(DISTROOT)$(PATHEXT2)rule 2> $(NULLDEV)
#	-del /Q $(DISTROOT)$(PATHEXT2)rule 2> $(NULLDEV)
	-$(MKDIR) $(DISTBIN) 2> $(NULLDEV)
	-del /Q $(DISTBIN) 2> $(NULLDEV)
#	-$(MKDIR) $(DISTBIN_8) 2> $(NULLDEV)
#	-del /Q $(DISTBIN_8) 2> $(NULLDEV)
#	-$(MKDIR) $(DISTBIN_8)$(PATHEXT2)branding 2> $(NULLDEV)
#	-del /Q $(DISTBIN_8)$(PATHEXT2)branding 2> $(NULLDEV)
#	-$(MKDIR) $(DISTBIN_7) 2> $(NULLDEV)
#	-del /Q $(DISTBIN_7) 2> $(NULLDEV)
#	-$(MKDIR) $(DISTBIN_7)$(PATHEXT2)branding 2> $(NULLDEV)
#	-del /Q $(DISTBIN_7)$(PATHEXT2)branding 2> $(NULLDEV)
	-$(MKDIR) $(DISTHELP) 2> $(NULLDEV)
	-del /Q $(DISTHELP) 2> $(NULLDEV)
	-$(MKDIR) $(DISTINC) 2> $(NULLDEV)
	-del /Q $(DISTINC) 2> $(NULLDEV)
	-$(MKDIR) $(DISTINC)$(PATHEXT2)c 2> $(NULLDEV)
	-del /Q $(DISTINC)$(PATHEXT2)c 2> $(NULLDEV)
	-$(MKDIR) $(DISTINC)$(PATHEXT2)support 2> $(NULLDEV)
	-del /Q $(DISTINC)$(PATHEXT2)support 2> $(NULLDEV)
	-$(MKDIR) $(DISTINC)$(PATHEXT2)support$(PATHEXT2)win32 2> $(NULLDEV)
	-del /Q $(DISTINC)$(PATHEXT2)support$(PATHEXT2)win32 2> $(NULLDEV)
	-$(MKDIR) $(DISTINC)$(PATHEXT2)c$(PATHEXT2)sys 2> $(NULLDEV)
	-del /Q $(DISTINC)$(PATHEXT2)c$(PATHEXT2)sys 2> $(NULLDEV)
	-del /Q $(DISTINC)$(PATHEXT2)sys 2> $(NULLDEV)
	-$(MKDIR) $(DISTINCWIN) 2> $(NULLDEV)
	-del /Q $(DISTINCWIN) 2> $(NULLDEV)
	-$(MKDIR) $(DISTINCSTL) 2> $(NULLDEV)
	-del /Q $(DISTINCSTL) 2> $(NULLDEV)
	-$(MKDIR) $(DISTINCSTL)$(PATHEXT2)stlport 2> $(NULLDEV)
	-del /Q $(DISTINCSTL)$(PATHEXT2)stlport 2> $(NULLDEV)
	-$(MKDIR) $(DISTINCSTL)$(PATHEXT2)stlport$(PATHEXT2)config 2> $(NULLDEV)
	-del /Q $(DISTINCSTL)$(PATHEXT2)stlport$(PATHEXT2)config 2> $(NULLDEV)
	-$(MKDIR) $(DISTINCSTL)$(PATHEXT2)stlport$(PATHEXT2)debug 2> $(NULLDEV)
	-del /Q $(DISTINCSTL)$(PATHEXT2)stlport$(PATHEXT2)debug 2> $(NULLDEV)
	-$(MKDIR) $(DISTINCSTL)$(PATHEXT2)stlport$(PATHEXT2)pointers 2> $(NULLDEV)
	-del /Q $(DISTINCSTL)$(PATHEXT2)stlport$(PATHEXT2)pointers 2> $(NULLDEV)
	-$(MKDIR) $(DISTINCSTL)$(PATHEXT2)using 2> $(NULLDEV)
	-del /Q $(DISTINCSTL)$(PATHEXT2)using 2> $(NULLDEV)
	-$(MKDIR) $(DISTINC)$(PATHEXT2)stl 2> $(NULLDEV)
	-del /Q $(DISTINC)$(PATHEXT2)stl 2> $(NULLDEV)
	-del /Q $(DISTLIB) 2> $(NULLDEV)
	-$(MKDIR) $(DISTSTARTUP) 2> $(NULLDEV)
	-del /Q $(DISTSTARTUP) 2> $(NULLDEV)
	-$(MKDIR) $(DISTSTARTUPDOS) 2> $(NULLDEV)
	-del /Q $(DISTSTARTUPDOS) 2> $(NULLDEV)
	-$(MKDIR) $(DISTSTARTUPWIN) 2> $(NULLDEV)
	-del /Q $(DISTSTARTUPWIN) 2> $(NULLDEV)
	-$(MKDIR) $(DISTADDON) 2> $(NULLDEV)
	-del /Q $(DISTADDON) 2> $(NULLDEV)
	-$(MKDIR) $(DISTEXAM) 2> $(NULLDEV)
	-del /Q $(DISTEXAM) 2> $(NULLDEV)
	-$(MKDIR) $(DISTEXAM)$(PATHEXT2)msdos 2> $(NULLDEV)
	-del /Q $(DISTEXAM)$(PATHEXT2)msdos 2> $(NULLDEV)
	-$(MKDIR) $(DISTEXAM)$(PATHEXT2)system 2> $(NULLDEV)
	-del /Q $(DISTEXAM)$(PATHEXT2)system 2> $(NULLDEV)
	-$(MKDIR) $(DISTEXAM)$(PATHEXT2)c$(PATHEXT2)win32 2> $(NULLDEV)
	-del /Q $(DISTEXAM)$(PATHEXT2)c$(PATHEXT2)win32 2> $(NULLDEV)
	-del /Q $(DISTEXAM)$(PATHEXT2)win32 2> $(NULLDEV)
	-$(MKDIR) $(DISTEXAM)$(PATHEXT2)win32$(PATHEXT2)atc 2> $(NULLDEV)
	-del /Q $(DISTEXAM)$(PATHEXT2)win32$(PATHEXT2)atc 2> $(NULLDEV)
	-$(MKDIR) $(DISTEXAM)$(PATHEXT2)win32$(PATHEXT2)listview 2> $(NULLDEV)
	-del /Q $(DISTEXAM)$(PATHEXT2)win32$(PATHEXT2)listview 2> $(NULLDEV)
	-$(MKDIR) $(DISTEXAM)$(PATHEXT2)win32$(PATHEXT2)xmlview 2> $(NULLDEV)
	-del /Q $(DISTEXAM)$(PATHEXT2)win32$(PATHEXT2)xmlview 2> $(NULLDEV)
	-$(MKDIR) $(DISTEXAM)$(PATHEXT2)win32$(PATHEXT2)RCDemo 2> $(NULLDEV)
	-del /Q $(DISTEXAM)$(PATHEXT2)win32$(PATHEXT2)RCDemo 2> $(NULLDEV)
	-$(MKDIR) $(DISTEXAM)$(PATHEXT2)win32$(PATHEXT2)huff 2> $(NULLDEV)
	-del /Q $(DISTEXAM)$(PATHEXT2)win32$(PATHEXT2)huff 2> $(NULLDEV)
	-$(MKDIR) $(DISTDIST) 2> $(NULLDEV)
endif


cleanstart:
	-del /Q $(_LIBDIR)
	-rmdir $(_LIBDIR)

$(CLEANS): %.clean :
	$(MAKE) clean -f $(_TREEROOT) -C$*

cleanlib:
	$(MAKE) -C clibs clean

clean: cleanstart cleanlib $(CLEANS)

distribute: $(DISTS1)
	$(MAKE) DISTRIBUTE
else

cleanDISTRIBUTE:

$(CLEANS): %.clean :
	$(MAKE) clean -f $(_TREEROOT) -C$*

clean: del rmdir $(CLEANS)
	del *.xcf *.xcppf *.xhf *.pdb

distribute: $(DISTS1)

endif

$(DISTS): %.dist : cleanDISTRIBUTE
	$(MAKE) DISTRIBUTE -f $(_TREEROOT) -C$*

$(DISTS1): %.dist1 : $(DISTS)
	$(MAKE) distribute -f $(_TREEROOT) -C$*

compare_exe:
ifneq "$(MAIN_FILE)" ""
ifeq "$(NO_COMPARE)" ""
	$(COMPARER) $(DISTBIN)\..\temp2\$(DISTNAME).exe $(DISTBIN)\$(DISTNAME).exe
endif
endif

compare: $(COMPARES)

$(COMPARES): %.comp :
	$(MAKE) compare_exe -f $(_TREEROOT) -C$*

zip:
ifdef WITHMSDOS
# this requires CC386 be installed since it relies on far pointer support
# so I don't make it a part of the default install
	@$(MAKE) -C$(DISTROOT)$(PATHEXT2)src$(PATHEXT2)..$(PATHEXT2) -f $(realpath .$(PATHEXT2)doszip.mak)
	@$(MAKE) -C$(DISTROOT)$(PATHEXT2)src$(PATHEXT2)dos$(PATHEXT2)install -fmakefile.le
endif
ifeq "$(COMPILER)" "gcc-linux"
	@$(MAKE) -C/ -f $(realpath .$(PATHEXT2)zip.mak)
else
	@$(MAKE) -C\ -f $(realpath .$(PATHEXT2)zip.mak)
endif
$(CDIRS): %.dirs :
	-$(MKDIR) $*$(PATHEXT2)obj$(PATHEXT2)$(OBJ_IND_PATH) 2> $(NULLDEV)

$(LIBS): %.library : $(CDIRS)
	$(MAKE) library compile -f $(_TREEROOT) -C$*
$(EXES): %.exefile :
	$(MAKE) exefile link -f $(_TREEROOT) -C$*

distribute_self:  cleanDISTRIBUTE
	$(MAKE) DISTRIBUTE
distribute_exe: $(DISTS1)

distribute_clibs_no_binary:
	@$(MAKE) -Cclibs DISTRIBUTE --eval="BUILDENV=1"
distribute_clibs:
	@$(MAKE) -Cclibs DISTRIBUTE


makelibdir:
	-$(MKDIR)  $(_LIBDIR) 2> $(NULLDEV)

library: makelibdir $(LIBS)

exefile: makelibdir $(EXES)

localfiles: makelibdir mkdir compile library exefile link


%.xcppf: %.cpp
	clang-format -style=file $< > $@
	copy $@ $<

%.xcf: %.c
	clang-format -style=file $< > $@
	copy $@ $<

%.xhf: %.h
	clang-format -style=file $< > $@
	copy $@ $<


$(FORMATS): %.format :
	$(MAKE) format -f $(_TREEROOT) -C$*

format: $(H_FILES:.h=.xhf) $(CPP_DEPENDENCIES:.cpp=.xcppf) $(C_DEPENDENCIES:.c=.xcf) $(FORMATS)


