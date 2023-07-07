# Software License Agreement
# 
#     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

ifneq "$(COMPILER)" "gcc-linux"
SHELL=cmd.exe
endif

ZIP:="c:/program files/7-zip/7z" -tzip -bd

ifeq "$(GITHUB_WORKSPACE)" ""
OCC_VERSION := $(word 3, $(shell cmd /C type \orangec\src\version.h))
OCC_VERSION := $(subst ",,$(OCC_VERSION))
export OCC_VERSION
VERNAME := $(subst .,,$(OCC_VERSION))
endif

DISTEXE=$(ORANGEC)/dist/ZippedBinaries$(VERNAME).zip
DISTSRC=$(ORANGEC)/dist/ZippedSources$(VERNAME).zip

DISTRIBUTE:
ifneq "$(ORANGEC_ONLY)" "YES"
	-del $(subst /,\,$(DISTEXE))
	-del $(subst /,\,$(DISTSRC))
	-mkdir $(DISTROOT)\appdata
	$(ZIP) -r0 a $(DISTEXE) $(ORANGEC)/bin/*.exe $(ORANGEC)/bin/*.pdb $(ORANGEC)/bin/*.app $(ORANGEC)/bin/*.spc $(ORANGEC)/bin/*.dll $(ORANGEC)/bin/*.cfg  $(ORANGEC)/bin/general.props
	$(ZIP) a $(DISTEXE) $(ORANGEC)/lib/*.* $(ORANGEC)/include/*.* $(ORANGEC)/examples/*.* $(ORANGEC)/doc/*.* $(ORANGEC)/license/*.* 
	$(ZIP) a $(DISTEXE) $(ORANGEC)/lib/* $(ORANGEC)/include/* $(ORANGEC)/examples/* $(ORANGEC)/doc/* $(ORANGEC)/license/*
	$(ZIP) d $(DISTEXE) $(ORANGEC)/doc/tools.htm
	$(ZIP) a $(DISTEXE) $(ORANGEC)/rule/*.rul $(ORANGEC)/help/*.* $(ORANGEC)/addon/*.* $(ORANGEC)/appdata
	$(ZIP) a $(DISTEXE) $(ORANGEC)/license.txt $(ORANGEC)/readme.txt $(ORANGEC)/relnotes.txt
ifeq "$(GITHUB_WORKSPACE)" ""
	$(ZIP) -r0 -xr@$(ORANGEC)/src/xclude.lst a $(DISTSRC) $(ORANGEC)/src/*.adl $(ORANGEC)/src/*.cpp $(ORANGEC)/src/*.c $(ORANGEC)/src/*.h $(ORANGEC)/src/*.nas $(ORANGEC)/src/*.ase $(ORANGEC)/src/*.asi $(ORANGEC)/src/*.inc $(ORANGEC)/src/*.p $(ORANGEC)/src/*.rc $(ORANGEC)/src/*.spc $(ORANGEC)/src/*.app $(ORANGEC)/src/*.cfg $(ORANGEC)/src/makefile* $(ORANGEC)/src/*.mak $(ORANGEC)/src/*.txt $(ORANGEC)/src/copying $(ORANGEC)/src/*.def $(ORANGEC)/src/*.lst 
	$(ZIP) -r0 -xr@$(ORANGEC)/src/xclude.lst a $(DISTSRC) $(ORANGEC)/src/ocl/ocl.lic $(ORANGEC)/src/clibs/cpp/* $(ORANGEC)/src/clibs/platform/copyrght.asm $(ORANGEC)/src/help/*.* $(ORANGEC)/doc/*.*
	$(ZIP) -r0 a $(DISTSRC) $(ORANGEC)/src/clibs/platform/dos32/extender/*.* $(ORANGEC)/src/clibs/platform/dos32/extender/*
	$(ZIP) -r0 a $(DISTSRC) $(ORANGEC)/src/*.vcxproj $(ORANGEC)/src/*.vcxproj.filters $(ORANGEC)/src/*.sln
	$(ZIP) a $(DISTSRC) $(ORANGEC)/src/ocide/fonts/ttf-bitstream-vera-1.10/*.* $(ORANGEC)/src/ocide/res/*.* $(ORANGEC)/src/ocide/rule/*.rul $(ORANGEC)/src/ocide/rule/*.props
	$(ZIP) a $(DISTSRC) $(ORANGEC)/src/*.exe $(ORANGEC)/src/*.iss $(ORANGEC)/src/*.bmp $(ORANGEC)/src/config.bat $(ORANGEC)/src/ocidehld.bat
	$(ZIP) a $(DISTSRC) $(ORANGEC)/src/LICENSE.TXT $(ORANGEC)/license/*.* $(ORANGEC)/src/readme.txt $(ORANGEC)/src/relnotes.txt
	$(ZIP) a $(DISTSRC) $(ORANGEC)/src/clibs/repobj.bat $(ORANGEC)/src/copying $(ORANGEC)/src/ocl.lic $(ORANGEC)/src/addon.txt
ifneq "$(MSYSTEM)" "MSYS"
	"/program files (x86)/inno setup 6/iscc" /Q /FWindowsInstaller$(VERNAME) /O$(ORANGEC)/dist $(ORANGEC)/src/occ.iss
endif
endif
	-rmdir $(DISTROOT)\appdata
endif
