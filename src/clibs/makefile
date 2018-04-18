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

all: dll clib 

BASEDIRS= ALLOC cmath COMPLIB IO locale MATH PROCONT stdlib STRING strings \
	threads TIME uio wc uchar
CLIBDIRS = $(BASEDIRS) cpp
DLLDIRS = $(BASEDIRS)
DIRS= $(CLIBDIRS) platform

MAKEFILES := $(MAKEFILES) $(realpath maker.mak)
export MAKEFILES
include maker.mak

DOSOBJECT:=$(realpath object\dos)
export DOSOBJECT
WINOBJECT:=$(realpath object\windows)
export WINOBJECT
ifdef OLDSGL
STDINCLUDE:=$(realpath cpp\stdinc);$(realpath stdinc)
else
ifdef STLPORT
STDINCLUDE:=$(realpath cpp\stlport\stl);$(realpath cpp\stdinc);$(realpath stdinc)
else
STDINCLUDE:=$(realpath cpp\stdinc_libcxx);$(realpath stdinc);$(realpath cpp\libcxx\include);$(realpath platform\win32\wininc);$(realpath platform\win32\wininc\win32)
endif
endif
export STDINCLUDE

define CLIB
	$$(MAKE) -C$(dir) "-DOBJECT=$(CLIBOBJECT)"
endef

define DLL
	$$(MAKE) -C$(dir) "-DOBJECT=$(DLLOBJECT)" "$(BUILDING_DLL)"
endef

SYSOBJECT :=$(realpath object\startup)
export SYSOBJECT

LSCRTLOBJECT :=$(realpath object\startup)
export LSCRTLOBJECT

DLLOBJECT :=$(realpath object\dll)
export DLLOBJECT

CLIBOBJECT :=$(realpath object\clib)
export CLIBOBJECT

CILOBJECT :=$(realpath object\cil)
export CILOBJECT

LIBOBJS = $(shell dir /b $(CLIBOBJECT)\*.o)
DOSOBJS1 = $(shell dir /b $(DOSOBJECT)\*.o)
DOSOBJS = $(DOSOBJS1:.OBJ=.o)
WINOBJS = $(shell dir /b $(WINOBJECT)\*.o)
CILOBJS = $(shell dir /b $(CILOBJECT)\*.ilo)
define clibdef
clib:
	-mkdir object
	-mkdir $(DOSOBJECT)
	-mkdir $(WINOBJECT)
	-mkdir $(SYSOBJECT)
	-mkdir $(DLLOBJECT)
	-mkdir $(LSCRTLOBJECT)
	-mkdir $(CILOBJECT)
	-mkdir object\clib
$(foreach dir, $(DIRS), $(CLIB))
#	$(MAKE) -CMSDOS "-DOBJECT=$(DOSOBJECT)"
#	$(MAKE) -CWIN32 "-DOBJECT=$(WINOBJECT)"
	del $(SYSOBJECT)\cldos.l
	$(LIB) $(LIBFLAGS) $(SYSOBJECT)\cldos.l @&&|
$$(foreach dir, $(LIBOBJS), $(CLIBOBJECT)\$(dir)) $$(foreach dir, $(DOSOBJS), $(DOSOBJECT)\$(dir))
|
	del $(SYSOBJECT)\clwin.l
	$(LIB) $(LIBFLAGS) $(SYSOBJECT)\clwin.l @&&|
$$(foreach dir, $(LIBOBJS), $(CLIBOBJECT)\$(dir)) $$(foreach dir, $(WINOBJS), $(WINOBJECT)\$(dir))
|
endef

BUILDING_DLL = /DBUILDING_DLL=-D__BUILDING_LSCRTL_DLL
define dlldef
dll:
	-mkdir object\dll
	$(foreach dir, $(DLLDIRS), $(DLL))
endef

$(eval $(clibdef))
$(eval $(dlldef))

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