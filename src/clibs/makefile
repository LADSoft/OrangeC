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
#     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
# 
#     contact information:
#         email: TouchStone222@runbox.com <David Lindauer>


all: dll clib 

BASEDIRS= ALLOC cmath COMPLIB IO locale MATH PROCONT stdlib STRING strings \
	threads TIME uio wc uchar
CLIBDIRS = $(BASEDIRS) cpp
DLLDIRS = $(BASEDIRS)
DIRS= $(CLIBDIRS) platform

define SUFFIXDIRS =
	$(foreach dir, $(1), $(dir)$(2))
endef

C_CLIBS:= $(call SUFFIXDIRS,$(DIRS),.clibs)
C_DLLS:= $(call SUFFIXDIRS,$(DLLDIRS),.dlls)

MAKEFILES := $(MAKEFILES) $(subst /,\,$(realpath maker.mak))
export MAKEFILES
include maker.mak

DOSOBJECT:=$(subst /,\,$(realpath object\dos))
export DOSOBJECT
WINOBJECT:=$(subst /,\,$(realpath object\windows))
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

SYSOBJECT =$(realpath object\startup)
export SYSOBJECT

LSCRTLOBJECT =$(realpath object\startup)
export LSCRTLOBJECT

DLLOBJECT =$(realpath object\dll)
export DLLOBJECT

CLIBOBJECT =$(realpath object\clib)
export CLIBOBJECT

CILOBJECT =$(realpath object\cil)
export CILOBJECT

LIBOBJS = $(subst $(CLIBOBJECT)\,,$(shell dir $(B) $(CLIBOBJECT)\*.o))
DOSOBJS1 = $(subst $(DOSOBJECT)\,,$(shell dir $(B) $(DOSOBJECT)\*.o))
DOSOBJS = $(DOSOBJS1:.OBJ=.o)
WINOBJS = $(subst $(WINOBJECT)\,,$(shell dir $(B) $(WINOBJECT)\*.o))
CILOBJS = $(subst $(CILOBJECT)\,,$(shell dir $(B) $(CILOBJECT)\*.ilo))

mkdir:
	-mkdir object
	-mkdir $(DOSOBJECT)
	-mkdir $(WINOBJECT)
	-mkdir $(SYSOBJECT)
	-mkdir $(DLLOBJECT)
	-mkdir $(LSCRTLOBJECT)
	-mkdir $(CILOBJECT)
	-mkdir object\clib

mklibs:
	del $(SYSOBJECT)\cldos.l

	$(LIB) $(LIBFLAGS) $(SYSOBJECT)\cldos.l $(foreach dir, $(LIBOBJS), $(CLIBOBJECT)\$(dir)) $(foreach dir, $(DOSOBJS), $(DOSOBJECT)\$(dir))
	del $(SYSOBJECT)\clwin.l
	$(LIB) $(LIBFLAGS) $(SYSOBJECT)\clwin.l $(foreach dir, $(LIBOBJS), $(CLIBOBJECT)\$(dir)) $(foreach dir, $(WINOBJS), $(WINOBJECT)\$(dir))

$(C_CLIBS) : %.clibs :
	$(MAKE) -C$* --eval="OBJECT=$(CLIBOBJECT)"

clib: mkdir $(C_CLIBS) mklibs

BUILDING_DLL = BUILDING_DLL=-D__BUILDING_LSCRTL_DLL

mkdirdll:
	-mkdir object\dll
$(C_DLLS) : %.dlls : 
	$(MAKE) -C$* --eval="OBJECT=$(DLLOBJECT)" --eval="$(BUILDING_DLL)"

dll: mkdirdll $(C_DLLS)


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