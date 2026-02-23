#  Software License Agreement
#  
#      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
#  
#      This file is part of the Orange C Compiler package.
#  
#      The Orange C Compiler package is free software: you can redistribute it and/or modify
#      it under the terms of the GNU General Public License as published by
#      the Free Software Foundation, either version 3 of the License, or
#      (at your option) any later version.
#  
#      The Orange C Compiler package is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#      GNU General Public License for more details.
#  
#      You should have received a copy of the GNU General Public License
#      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
#  
#      contact information:
#          email: TouchStone222@runbox.com <David Lindauer>
#  

include ../pathext2.mak

ifeq "$(COMPILER)" "gcc-linux"
MKDIR := mkdir -p
DEL := rm
COPY := cp
else
MKDIR := mkdir
DEL := del /Q
COPY := copy
endif

ifeq "$(NULLDEV)" ""
ifeq "$(COMPILER)" "gcc-linux"
NULLDEV := /dev/null
else
NULLDEV := NUL
endif
export NULLDEV
endif

export MKDIR
export DEL
export COPY

all: mkapps

BASEDIRS= alloc cmath complib io locale math procont stdbit stdlib string strings \
	threads time uio wc uchar
CLIBDIRS = $(BASEDIRS) cpp
DLLDIRS = $(BASEDIRS) cpp
DIRS= $(CLIBDIRS)

define SUFFIXDIRS =
	$(foreach dir, $(1), $(dir)$(2))
endef

C_CLIBS:= $(call SUFFIXDIRS,$(DIRS),.clibs)
C_DLLS:= $(call SUFFIXDIRS,$(DLLDIRS),.dlls)
C_FORMAT= $(call SUFFIXDIRS,$(DIRS),.formats)

MAKEFILES := $(MAKEFILES) $(subst /,$(PATHEXT2),$(realpath maker.mak))
export MAKEFILES
include maker.mak

ifneq ($(shell type makefile),)
B = /b
endif

THISPATH:=$(subst /,$(PATHEXT2),$(CURDIR)\)

#DOSOBJECT:=$(THISPATH)object\dos
#export DOSOBJECT
WINOBJECT:=$(THISPATH)object$(PATHEXT2)windows
export WINOBJECT

SYSOBJECT:=$(THISPATH)object$(PATHEXT2)startup
export SYSOBJECT

LSCRTLOBJECT:=$(THISPATH)object$(PATHEXT2)startup
export LSCRTLOBJECT

CLIBOBJECT:=$(THISPATH)object$(PATHEXT2)clib
export CLIBOBJECT

CILOBJECT:=$(THISPATH)object$(PATHEXT2)cil
export CILOBJECT

LIBOBJS = $(subst $(CLIBOBJECT)\,,$(shell dir $(B) $(CLIBOBJECT)$(PATHEXT2)*.o))
#DOSOBJS1 = $(subst $(CLIBOBJECT)\,,$(shell dir $(B) $(DOSOBJECT)\*.o))
#DOSOBJS = $(DOSOBJS1:.OBJ=.o)
WINOBJS = $(subst $(CLIBOBJECT)\,,$(shell dir $(B) $(WINOBJECT)$(PATHEXT2)*.o))
CILOBJS = $(subst $(CLIBOBJECT)\,,$(shell dir $(B) $(CILOBJECT)$(PATHEXT2)*.ilo))

$(MKDIR):
	-$(MKDIR) object 2> $(NULLDEV)
#	-$(MKDIR) object$(PATHEXT2)dos 2> $(NULLDEV)
	-$(MKDIR) object$(PATHEXT2)windows 2> $(NULLDEV)
	-$(MKDIR) object$(PATHEXT2)startup 2> $(NULLDEV)
	-$(MKDIR) object$(PATHEXT2)cil 2> $(NULLDEV)
	-$(MKDIR) object$(PATHEXT2)clib 2> $(NULLDEV)

copyfiles: $(MKDIR)
	-$(MKDIR) ..$(PATHEXT2)..$(PATHEXT2)include 2> $(NULLDEV)
	-$(MKDIR) ..$(PATHEXT2)..$(PATHEXT2)include$(PATHEXT2)c 2> $(NULLDEV)
	-$(MKDIR) ..$(PATHEXT2)..$(PATHEXT2)include$(PATHEXT2)c$(PATHEXT2)sys 2> $(NULLDEV)
	-$(MKDIR) ..$(PATHEXT2)..$(PATHEXT2)include$(PATHEXT2)c$(PATHEXT2)win32 2> $(NULLDEV)
	-$(MKDIR) ..$(PATHEXT2)..$(PATHEXT2)include$(PATHEXT2)c$(PATHEXT2)win32$(PATHEXT2)gl 2> $(NULLDEV)
	-$(MKDIR) ..$(PATHEXT2)..$(PATHEXT2)include$(PATHEXT2)support 2> $(NULLDEV)
	-$(MKDIR) ..$(PATHEXT2)..$(PATHEXT2)include$(PATHEXT2)support$(PATHEXT2)win32 2> $(NULLDEV)
	-$(MKDIR) ..$(PATHEXT2)..$(PATHEXT2)include$(PATHEXT2)support$(PATHEXT2)orangec 2> $(NULLDEV)
	-$(COPY)  cpp$(PATHEXT2)libcxx$(PATHEXT2)include ..$(PATHEXT2)..$(PATHEXT2)include 2>$(NULLDEV)
	-$(COPY)  cpp$(PATHEXT2)libcxx$(PATHEXT2)include$(PATHEXT2)support$(PATHEXT2)win32 ..$(PATHEXT2)..$(PATHEXT2)include$(PATHEXT2)support$(PATHEXT2)win32 2>$(NULLDEV)
	-$(COPY)  cpp$(PATHEXT2)libcxx$(PATHEXT2)include$(PATHEXT2)support$(PATHEXT2)orangec ..$(PATHEXT2)..$(PATHEXT2)include$(PATHEXT2)support$(PATHEXT2)orangec 2>$(NULLDEV)
	-$(COPY)  cpp$(PATHEXT2)stdinc_libcxx ..$(PATHEXT2)..$(PATHEXT2)include 2>$(NULLDEV)
	-$(COPY)  stdinc ..$(PATHEXT2)..$(PATHEXT2)include$(PATHEXT2)c 2>$(NULLDEV)
	-$(COPY)  stdinc$(PATHEXT2)sys ..$(PATHEXT2)..$(PATHEXT2)include$(PATHEXT2)c$(PATHEXT2)sys 2>$(NULLDEV)
	-$(COPY)  platform$(PATHEXT2)win32$(PATHEXT2)inc ..$(PATHEXT2)..$(PATHEXT2)include$(PATHEXT2)c$(PATHEXT2)win32 2>$(NULLDEV)
	-$(COPY)  platform$(PATHEXT2)win32$(PATHEXT2)inc$(PATHEXT2)gdiplus ..$(PATHEXT2)..$(PATHEXT2)include$(PATHEXT2)c$(PATHEXT2)win32 2>$(NULLDEV)
	-$(COPY)  platform$(PATHEXT2)win32$(PATHEXT2)inc$(PATHEXT2)gl ..$(PATHEXT2)..$(PATHEXT2)include$(PATHEXT2)c$(PATHEXT2)win32$(PATHEXT2)gl 2>$(NULLDEV)

$(C_CLIBS) : %.clibs :
	$(MAKE) -C$* --eval="OBJECT=$(CLIBOBJECT)" --eval="$(BUILDING_DLL)"

$(C_FORMAT) : %.formats :
	$(MAKE) -C$* format

platform: $(C_CLIBS)
	$(MAKE) -Cplatform --eval="OBJECT=$(CLIBOBJECT)"

mklibs: platform
ifneq "$(ORANGEC_ONLY)" "YES"
#	-$(DEL) $(SYSOBJECT)\cldos.l
#	$(LIB) $(LIBFLAGS) $(SYSOBJECT)\cldos.l $(foreach dir, $(LIBOBJS), $(CLIBOBJECT)\$(dir)) $(foreach dir, $(DOSOBJS), $(DOSOBJECT)\$(dir))
endif
	-$(DEL) $(SYSOBJECT)$(PATHEXT2)clwin.l
	$(LIB) $(LIBFLAGS) $(SYSOBJECT)$(PATHEXT2)clwin.l $(foreach dir, $(LIBOBJS), $(CLIBOBJECT)$(PATHEXT2)$(dir))
	$(LIB) $(LIBFLAGS) $(SYSOBJECT)$(PATHEXT2)clwin.l $(foreach dir, $(WINOBJS), $(WINOBJECT)$(PATHEXT2)$(dir))

mkapps:
	$(MAKE) copyfiles
	$(MAKE) mklibs
	$(MAKE) -Cplatform --eval="OBJECT=$(CLIBOBJECT)" final

test:
	$(MAKE) -Ccpp$(PATHEXT2)libcxx$(PATHEXT2)test current
BUILDING_DLL = BUILDING_DLL=-D__BUILDING_LSCRTL_DLL

FORMAT: $(C_FORMAT)
	$(MAKE) -Cstdinc format
	$(MAKE) -Cplatform format

ifdef DISTMAKE
	 	include $(DISTMAKE)

DISTRIBUTE:
	$(COPYDIR) cpp$(PATHEXT2)libcxx$(PATHEXT2)include$(PATHEXT2)% $(DISTINC)
	$(COPYDIR) cpp$(PATHEXT2)libcxx$(PATHEXT2)include$(PATHEXT2)%.h $(DISTINC)
	$(COPYDIR) cpp$(PATHEXT2)libcxx$(PATHEXT2)include$(PATHEXT2)support$(PATHEXT2)win32$(PATHEXT2)% $(DISTINC)$(PATHEXT2)support$(PATHEXT2)win32
	$(COPYDIR) cpp$(PATHEXT2)stdinc_libcxx$(PATHEXT2)% $(DISTINC)
	$(COPYDIR) stdinc$(PATHEXT2)%.h $(DISTINC)$(PATHEXT2)c
	$(COPYDIR) stdinc$(PATHEXT2)sys$(PATHEXT2)%.h $(DISTINC)$(PATHEXT2)c$(PATHEXT2)sys
	$(COPYDIR) platform$(PATHEXT2)win32$(PATHEXT2)inc$(PATHEXT2)%.h $(DISTINC)$(PATHEXT2)c$(PATHEXT2)win32
ifndef BUILDENV
	$(COPYDIR) object$(PATHEXT2)startup$(PATHEXT2)%.l $(DISTLIB)
	$(COPYDIR) object$(PATHEXT2)startup$(PATHEXT2)%.o $(DISTLIB)
ifneq "$(ORANGEC_ONLY)" "YES"
#	$(COPYDIR) object$(PATHEXT2)startup$(PATHEXT2)%.exe $(DISTLIB)
endif
	$(COPYDIR) object$(PATHEXT2)startup$(PATHEXT2)%.dll $(DISTBIN)
	$(MAKE) -Cplatform DISTRIBUTE
endif
endif

clean:
#	-$(DEL) $(DOSOBJECT) 2> $(NULLDEV)
	-$(DEL) $(WINOBJECT) 2> $(NULLDEV)
	-$(DEL) $(SYSOBJECT) 2> $(NULLDEV)
	-$(DEL) $(CILOBJECT) 2> $(NULLDEV)
	-$(DEL) $(CLIBOBJECT) 2> $(NULLDEV)
	-$(DEL) platform$(PATHEXT2)win32$(PATHEXT2)crtdmain$(PATHEXT2)*.o 2>$(NULLDEV)
	-$(DEL) platform$(PATHEXT2)win32$(PATHEXT2)lscrtl$(PATHEXT2)*.o 2>$(NULLDEV)
	-$(DEL) platform$(PATHEXT2)win32$(PATHEXT2)lscrtl$(PATHEXT2)*.dll 2>$(NULLDEV)
	-$(DEL) platform$(PATHEXT2)win32$(PATHEXT2)lsdbghelper$(PATHEXT2)*.o 2>$(NULLDEV)
	-$(DEL) platform$(PATHEXT2)win32$(PATHEXT2)msvcrdm$(PATHEXT2)*.o 2>$(NULLDEV)
	-$(DEL) platform$(PATHEXT2)win32$(PATHEXT2)occil$(PATHEXT2)*.o 2>$(NULLDEV)
	-$(DEL) platform$(PATHEXT2)win32$(PATHEXT2)occil$(PATHEXT2)occmsil.dll 2>$(NULLDEV)
	-$(DEL) platform$(PATHEXT2)win32$(PATHEXT2)pe$(PATHEXT2)*.o 2>$(NULLDEV)
	-$(DEL) platform$(PATHEXT2)win32$(PATHEXT2)pels$(PATHEXT2)*.o 2>$(NULLDEV)
	-$(DEL) platform$(PATHEXT2)win32$(PATHEXT2)rtl$(PATHEXT2)*.o 2>$(NULLDEV)
#	-$(DEL) platform$(PATHEXT2)dos32$(PATHEXT2)debug$(PATHEXT2)*.o 2>$(NULLDEV)
#	-$(DEL) platform$(PATHEXT2)dos32$(PATHEXT2)le$(PATHEXT2)*.o 2>$(NULLDEV)
#	-$(DEL) platform$(PATHEXT2)dos32$(PATHEXT2)pm$(PATHEXT2)*.o 2>$(NULLDEV)
#	-$(DEL) platform$(PATHEXT2)dos32$(PATHEXT2)rtl$(PATHEXT2)*.o 2>$(NULLDEV)
#	-rmdir $(DOSOBJECT) 2> $(NULLDEV)
	-rmdir $(WINOBJECT) 2> $(NULLDEV)
	-rmdir $(SYSOBJECT) 2> $(NULLDEV)
	-rmdir $(CILOBJECT) 2> $(NULLDEV)
	-rmdir $(CLIBOBJECT) 2> $(NULLDEV)
	-rmdir object 2> $(NULLDEV)
