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

CC=occ
CFLAGS = /! $(C_FLAGS) $(DEFINES)
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
CFLAGS := $(CFLAGS) /DSTD_NEWHANDLER
endif
endif

%.o: %.cpp
	$(CC) /c $(CFLAGS) $(BUILDING_DLL) $(STDINCLUDE) -o$(OBJECT)\$@ $^
#	$(CC) /S $(CFLAGS) $(BUILDING_DLL) $(STDINCLUDE) $^
#	$(ASM) $(ASMFLAGS) $(BUILDING_DLL) -o$(OBJECT)\$@ $*

%.o: %.c
	$(CC) /1 /c $(CFLAGS) $(BUILDING_DLL) $(STDINCLUDE) -o$(OBJECT)\$@ $^
#	$(CC) /S $(CFLAGS) $(BUILDING_DLL) $(STDINCLUDE) $^
#	$(ASM) $(ASMFLAGS) $(BUILDING_DLL) -o$(OBJECT)\$@ $*
%.o: %.nas
	$(ASM) $(ASMFLAGS) $(BUILDING_DLL) -o$(OBJECT)\$@ $(subst /,\,$^)

%.ilo: %.c
	occil -N$(OCCIL_CLASS) /1 /c /WcMn $(CILCFLAGS) $(STDINCLUDE) -o$(CILOBJECT)\$@ $(subst /,\,$^)

C_deps = $(notdir $(C_DEPENDENCIES:.c=.o))
ASM_deps = $(notdir $(ASM_DEPENDENCIES:.nas=.o))
CPP_deps = $(notdir $(CPP_DEPENDENCIES:.cpp=.o))
ifdef LSMSILCRTL
CIL_DEPS = $(notdir $(CIL_DEPENDENCIES:.c=.ilo))
endif
DEPENDENCIES = $(filter-out $(EXCLUDE), $(C_deps) $(ASM_deps) $(CPP_deps) $(CIL_DEPS))

define SUFFIXDIRS =
        $(foreach dir, $(1), $(dir)$(2))
endef


define ALLDIRS =
C_DIRS = $$(call SUFFIXDIRS,$(DIRS),.dirs)
alldirs: $$(C_DIRS)
$$(C_DIRS) : %.dirs :
	$$(MAKE) -C$$*
endef
export ALLDIRS