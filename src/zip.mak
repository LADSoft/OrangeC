# Software License Agreement
# 
#     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
# 

ifneq "$(COMPILER)" "gcc-linux"
SHELL=cmd.exe
endif
#	$(ZIP) -r0 a $(DISTSRC) orangec/src/clibs/platform/dos32/extender/*.* orangec/src/clibs/platform/dos32/extender/*

ZIP:="c:/program files/7-zip/7z" -tzip -bd

ifeq "$(GITHUB_WORKSPACE)" ""
OCC_VERSION := $(word 3, $(shell cmd /C type orangec\src\version.h))
OCC_VERSION := $(subst ",,$(OCC_VERSION))
export OCC_VERSION
VERNAME := $(subst .,,$(OCC_VERSION))
endif

DISTEXE=orangec/dist/ZippedBinaries$(VERNAME).zip
DISTSRC=orangec/dist/ZippedSources$(VERNAME).zip

DISTRIBUTE:
	$(MAKE) -C..\.. -forangec\src\zip.mak maindist
maindist:
ifeq "$(WITHOUT_ZIP)" ""
	-del $(subst /,\,$(DISTEXE))
	-del $(subst /,\,$(DISTSRC))
	-mkdir $(DISTROOT)\appdata
	$(ZIP) -r0 a $(DISTEXE) orangec/bin/*.exe orangec/bin/*.pdb orangec/bin/*.app orangec/bin/*.spc orangec/bin/*.dll orangec/bin/*.cfg  orangec/bin/general.props
	$(ZIP) a $(DISTEXE) orangec/lib/*.* orangec/include/*.* orangec/examples/*.* orangec/doc/*.* orangec/license/*.* 
	$(ZIP) a $(DISTEXE) orangec/lib/* orangec/include/* orangec/examples/* orangec/doc/* orangec/license/*
	$(ZIP) d $(DISTEXE) orangec/doc/tools.htm
	$(ZIP) a $(DISTEXE) orangec/rule/*.rul orangec/help/*.* orangec/addon/*.* orangec/appdata
	$(ZIP) a $(DISTEXE) orangec/license.txt orangec/readme.txt orangec/relnotes.txt
	$(ZIP) -r0 -xr@orangec/src/xclude.lst a $(DISTSRC) orangec/src/*.adl orangec/src/*.cpp orangec/src/*.c orangec/src/*.h orangec/src/*.nas orangec/src/*.ase orangec/src/*.asi orangec/src/*.inc orangec/src/*.p orangec/src/*.rc orangec/src/*.spc orangec/src/*.app orangec/src/*.cfg orangec/src/makefile* orangec/src/*.mak orangec/src/*.txt orangec/src/copying orangec/src/*.def orangec/src/*.lst 
	$(ZIP) -r0 -xr@orangec/src/xclude.lst a $(DISTSRC) orangec/src/ocl/ocl.lic orangec/src/clibs/cpp/* orangec/src/clibs/platform/copyrght.asm orangec/src/help/*.* orangec/doc/*.*
	$(ZIP) -r0 a $(DISTSRC) orangec/src/*.vcxproj orangec/src/*.vcxproj.filters orangec/src/*.sln
	$(ZIP) a $(DISTSRC) orangec/src/*.exe orangec/src/*.iss orangec/src/*.bmp orangec/src/config.bat 
	$(ZIP) a $(DISTSRC) orangec/src/LICENSE.TXT orangec/license/*.* orangec/src/readme.txt orangec/src/relnotes.txt
	$(ZIP) a $(DISTSRC) orangec/src/clibs/repobj.bat orangec/src/copying orangec/src/ocl.lic orangec/src/addon.txt
ifeq "$(GITHUB_WORKSPACE)" ""
	"/program files (x86)/inno setup 6/iscc" /Q /FWindowsInstaller$(VERNAME) /Oorangec/dist orangec/src/occ.iss
endif
	-rmdir $(DISTROOT)\appdata
endif
