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

BUILDING_DLL := BUILDING_DLL=-D__BUILDING_LSCRTL_DLL

SHELL=cmd.exe
export SHELL

all: alldirs


lscrtl_objs:
	$(MAKE) -Crtl --eval="OBJECT=$(DLLOBJECT)" --eval="$(BUILDING_DLL)"
rtl_objs: lscrtl_objs
	$(MAKE) -Crtl

alldirs: rtl_objs
ifdef FINAL
	$(MAKE) -Clsdbghelper
ifneq "$(ORANGEC_ONLY)" "YES"
	$(MAKE) -Coccil
endif
else
	$(MAKE) -Cpe
	$(MAKE) -Cdef
	$(MAKE) -Cmsvcrdm
	$(MAKE) -Ccrtdmain
	$(MAKE) -Clscrtl
	$(MAKE) -Cpels
endif

DISTRIBUTE:
	$(MAKE) -Cpe DISTRIBUTE
	$(MAKE) -Cpels DISTRIBUTE
	$(MAKE) -Cmsvcrdm DISTRIBUTE
	$(MAKE) -Ccrtdmain DISTRIBUTE
	$(COPYDIR) wininc\%.h $(DISTINC)
	$(COPYDIR) wininc\win32\%.h $(DISTINCWIN)
#	$(MAKE) -Clscrtl DISTRIBUTE
