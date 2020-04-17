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
#     As a special exception, if other files instantiate templates or
#     use macros or inline functions from this file, or you compile
#     this file and link it with other works to produce a work based
#     on this file, this file does not by itself cause the resulting
#     work to be covered by the GNU General Public License. However
#     the source code for this file must still be made available in
#     accordance with section (3) of the GNU General Public License.
#     
#     This exception does not invalidate any other reasons why a work
#     based on this file might be covered by the GNU General Public
#     License.
# 
#     contact information:
#         email: TouchStone222@runbox.com <David Lindauer>
# 

SHELL=cmd.exe
export SHELL

all: mkapps

BASEDIRS= ALLOC cmath COMPLIB IO locale MATH PROCONT stdlib STRING strings \
	threads TIME uio wc uchar
CLIBDIRS = $(BASEDIRS) cpp
DLLDIRS = $(BASEDIRS) cpp
DIRS= $(CLIBDIRS)

define SUFFIXDIRS =
	$(foreach dir, $(1), $(dir)$(2))
endef

C_CLIBS:= $(call SUFFIXDIRS,$(DIRS),.clibs)
C_DLLS:= $(call SUFFIXDIRS,$(DLLDIRS),.dlls)

MAKEFILES := $(MAKEFILES) $(subst /,\,$(realpath maker.mak))
export MAKEFILES
include maker.mak

ifneq ($(shell type makefile),)
B = /b
endif

THISPATH:=$(subst /,\,$(CURDIR)\)

DOSOBJECT:=$(THISPATH)object\dos
export DOSOBJECT
WINOBJECT:=$(THISPATH)object\windows
export WINOBJECT
ifdef OLDSGL
STDINCLUDE:=-I$(subst /,\,$(realpath cpp\stdinc)) -I$(subst /,\,$(realpath stdinc))
else
ifdef STLPORT
STDINCLUDE:=-I$(subst /,\,$(realpath cpp\stlport\stl)) -I$(subst /,\,$(realpath cpp\stdinc)) -I$(subst /,\,$(realpath stdinc))
else
STDINCLUDE:=-I$(subst /,\,$(realpath cpp\stdinc_libcxx)) -I$(subst /,\,$(realpath stdinc)) -I$(subst /,\,$(realpath cpp\libcxx\include)) -I$(subst /,\,$(realpath platform\win32\wininc)) -I$(subst /,\,$(realpath platform\win32\wininc\win32))
endif
endif
export STDINCLUDE

SYSOBJECT:=$(THISPATH)object\startup
export SYSOBJECT

LSCRTLOBJECT:=$(THISPATH)object\startup
export LSCRTLOBJECT

DLLOBJECT:=$(THISPATH)object\dll
export DLLOBJECT

CLIBOBJECT:=$(THISPATH)object\clib
export CLIBOBJECT

CILOBJECT:=$(THISPATH)object\cil
export CILOBJECT

LIBOBJS = $(subst $(CLIBOBJECT)\,,$(shell dir $(B) $(CLIBOBJECT)\*.o))
DOSOBJS1 = $(subst $(CLIBOBJECT)\,,$(shell dir $(B) $(DOSOBJECT)\*.o))
DOSOBJS = $(DOSOBJS1:.OBJ=.o)
WINOBJS = $(subst $(CLIBOBJECT)\,,$(shell dir $(B) $(WINOBJECT)\*.o))
CILOBJS = $(subst $(CLIBOBJECT)\,,$(shell dir $(B) $(CILOBJECT)\*.ilo))

mkdir:
	-mkdir object 2> NUL
	-mkdir object\dos 2> NUL
	-mkdir object\windows 2> NUL
	-mkdir object\startup 2> NUL
	-mkdir object\dll 2> NUL
	-mkdir object\cil 2> NUL
	-mkdir object\clib 2> NUL

$(C_DLLS) : %.dlls : mkdir
	$(MAKE) -C$* --eval="OBJECT=$(DLLOBJECT)" --eval="$(BUILDING_DLL)"

$(C_CLIBS) : %.clibs : $(C_DLLS)
	$(MAKE) -C$* --eval="OBJECT=$(CLIBOBJECT)"

platform: $(C_CLIBS)
	$(MAKE) -Cplatform --eval="OBJECT=$(CLIBOBJECT)"

mklibs: platform
	-del $(SYSOBJECT)\cldos.l
	$(LIB) $(LIBFLAGS) $(SYSOBJECT)\cldos.l $(foreach dir, $(LIBOBJS), $(CLIBOBJECT)\$(dir)) $(foreach dir, $(DOSOBJS), $(DOSOBJECT)\$(dir))
	-del $(SYSOBJECT)\clwin.l
	$(LIB) $(LIBFLAGS) $(SYSOBJECT)\clwin.l $(foreach dir, $(LIBOBJS), $(CLIBOBJECT)\$(dir)) $(foreach dir, $(WINOBJS), $(WINOBJECT)\$(dir))

mkapps: mklibs
	$(MAKE) -Cplatform --eval="OBJECT=$(CLIBOBJECT)" --eval="FINAL=1"

BUILDING_DLL = BUILDING_DLL=-D__BUILDING_LSCRTL_DLL



ifdef DISTMAKE
	 	include $(DISTMAKE)

DISTRIBUTE:
ifdef OLDSGL
	$(COPYDIR) cpp\stdinc\%. $(DISTINC)
	$(COPYDIR) cpp\stdinc\%.h $(DISTINC)
	$(COPYDIR) cpp\sgiinc\%. $(DISTINC)
	$(COPYDIR) cpp\sgiinc\%.h $(DISTINC)
	$(COPYDIR) cpp\sgiinc\stl\%. $(DISTINC)\stl
	$(COPYDIR) cpp\sgiinc\stl\%.h $(DISTINC)\stl
else
ifdef STLPORT
	$(COPYDIR) cpp\stlport-5.2.1\stlport\%.% $(DISTROOT)\stlport
	$(COPYDIR) cpp\stlport-5.2.1\stlport\stl\%.% $(DISTROOT)\stlport\stl
	$(COPYDIR) cpp\stlport-5.2.1\stlport\stl\config\%.% $(DISTROOT)\stlport\stl\config
	$(COPYDIR) cpp\stlport-5.2.1\stlport\stl\debug\%.% $(DISTROOT)\stlport\stl\debug
	$(COPYDIR) cpp\stlport-5.2.1\stlport\stl\pointers\%.% $(DISTROOT)\stlport\stl\pointers
	$(COPYDIR) cpp\stlport-5.2.1\stlport\using\%.% $(DISTROOT)\stlport\using
	$(COPYDIR) cpp\stlport-5.2.1\stlport\using\h\%.% $(DISTROOT)\stlport\using\h
else
	$(COPYDIR) cpp\libcxx\include\%. $(DISTINC)
	$(COPYDIR) cpp\libcxx\include\%.h $(DISTINC)
	$(COPYDIR) cpp\stdinc_libcxx\%. $(DISTINC)
	$(COPYDIR) cpp\stdinc_libcxx\%.h $(DISTINC)
endif
endif
	$(COPYDIR) stdinc\%.h $(DISTINC)
	$(COPYDIR) stdinc\sys\%.h $(DISTINC)\sys
	$(COPYDIR) platform\win32\wininc\%.h $(DISTINC)
	$(COPYDIR) platform\win32\wininc\win32\%.h $(DISTINC)\win32
ifndef BUILDENV
	$(COPYDIR) object\startup\%.l $(DISTLIB)
	$(COPYDIR) object\startup\%.o $(DISTLIB)
	$(COPYDIR) object\startup\%.exe $(DISTLIB)
	$(COPYDIR) object\startup\%.dll $(DISTBIN)
	$(MAKE) -Cplatform DISTRIBUTE
endif
endif

clean:
	-del /Q $(DOSOBJECT) 2> NUL
	-del /Q $(WINOBJECT) 2> NUL
	-del /Q $(SYSOBJECT) 2> NUL
	-del /Q $(DLLOBJECT) 2> NUL
	-del /Q $(CILOBJECT) 2> NUL
	-del /Q $(CLIBOBJECT) 2> NUL
	-rmdir $(DOSOBJECT) 2> NUL
	-rmdir $(WINOBJECT) 2> NUL
	-rmdir $(SYSOBJECT) 2> NUL
	-rmdir $(DLLOBJECT) 2> NUL
	-rmdir $(CILOBJECT) 2> NUL
	-rmdir $(CLIBOBJECT) 2> NUL
	-rmdir object 2> NUL
