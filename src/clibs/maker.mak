# Software License Agreement
# 
#     Copyright(C) 1994-2021 David Lindauer, (LADSoft)
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

CC=occ
CCFLAGS = /! $(C_FLAGS) $(DEFINES)
CILCFLAGS = $(CIL_C_FLAGS) $(DEFINES)

OCCIL_CLASS=lsmsilcrtl.rtl

LINK=olink
LINKFLAGS= -c+ -!

vpath %.c ./cil/ #
vpath %.ilo $(CILOBJECT)
vpath %.l $(SYSOBJECT)
vpath %.nas ./386/ #
vpath %.o $(OBJECT) $(SYSOBJECT)

ASM=oasm
ASMFLAGS= -!

LIB=olib
LIBFLAGS= -!

IMPLIB=oimplib
IMPLIBFLAGS= -!

ifdef OLDSGL
else
ifdef STLPORT
else
CCFLAGS := $(CCFLAGS) /DSTD_NEWHANDLER
endif
endif

CFLAGS := $(CCFLAGS)
CXXFLAGS := $(CCFLAGS)

%.o: %.cpp
	$(CC) /c $(CXXFLAGS) $(BUILDING_DLL) /D_LIBCPP_BUILDING_SYSTEM_ERROR /D_LIBCPP_BUILDING_LIBRARY -I$(ORANGEC)\src\sqlite3 -o$(OBJECT)\$@ $^
#	$(CC) /S $(CXXFLAGS) $(BUILDING_DLL) $^
#	$(ASM) $(ASMFLAGS) $(BUILDING_DLL) -o$(OBJECT)\$@ $*

%.o: %.c
	$(CC) /1 /c $(CFLAGS) $(BUILDING_DLL) -I$(ORANGEC)\src\sqlite3 -o$(OBJECT)\$@ $^
#	$(CC) /S $(CFLAGS) $(BUILDING_DLL) $^
#	$(ASM) $(ASMFLAGS) $(BUILDING_DLL) -o$(OBJECT)\$@ $*
%.o: %.nas
	$(ASM) $(ASMFLAGS) $(BUILDING_DLL) -o$(OBJECT)\$@ $(subst /,\,$^)

%.ilo: %.c
	occil -N$(OCCIL_CLASS) /1 /c /WcMn $(CILCFLAGS) -o$(CILOBJECT)\$@ $(subst /,\,$^)

%.xcppf: %.cpp
	clang-format -style=file $< > $@
	copy $@ $<

%.xcf: %.c
	clang-format -style=file $< > $@
	copy $@ $<

%.xhf: %.h
	clang-format -style=file $< > $@
	copy $@ $<


C_deps = $(notdir $(C_DEPENDENCIES:.c=.o))
ASM_deps = $(notdir $(ASM_DEPENDENCIES:.nas=.o))
CPP_deps = $(notdir $(CPP_DEPENDENCIES:.cpp=.o))
ifdef LSMSILCRTL
CIL_DEPS = $(notdir $(CIL_DEPENDENCIES:.c=.ilo))
endif
DEPENDENCIES = $(filter-out $(EXCLUDE), $(C_deps) $(ASM_deps) $(CPP_deps) $(CIL_DEPS))
FORMAT_DEPENDENCIES= $(filter-out $(FORMAT_EXCLUDE), $(H_FILES:.h=.xhf) $(CPP_DEPENDENCIES:.cpp=.xcppf) $(C_DEPENDENCIES:.c=.xcf))

define SUFFIXDIRS =
        $(foreach dir, $(1), $(dir)$(2))
endef

define FORMATTER =
format: $$(FORMAT_DEPENDENCIES)
endef

define ALLDIRS =
C_DIRS = $$(call SUFFIXDIRS,$(DIRS),.dirs)
alldirs: $$(C_DIRS)
$$(C_DIRS) : %.dirs :
	$$(MAKE) -C$$*
endef
export ALLDIRS