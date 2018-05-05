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

SHELL=cmd.exe

ZIP:="c:/program files/7-zip/7z" -tzip 

VERNAME := $(word 3, $(shell type,\orangec\src\version.h))
VERNAME := $(subst ",,$(VERNAME))
VERNAME := $(subst .,,$(VERNAME))

DISTEXE=/orangec/dist/occ$(VERNAME)e.zip
DISTSRC=/orangec/dist/occ$(VERNAME)s.zip

DISTRIBUTE:
	-del $(DISTEXE)
	-del $(DISTSRC)
	-mkdir $(DISTROOT)\appdata
	$(ZIP) -r0 a $(DISTEXE) orangec/bin/*.exe orangec/bin/*.app orangec/bin/*.spc orangec/bin/*.dll orangec/bin/*.cfg  orangec/bin/general.props
	$(ZIP) a $(DISTEXE) orangec/lib/*.* orangec/include/*.* orangec/examples/*.* orangec/doc/*.* orangec/license/*.* 
	$(ZIP) a $(DISTEXE) orangec/lib/* orangec/include/* orangec/examples/* orangec/doc/* orangec/license/*
	$(ZIP) d $(DISTEXE) doc/tools.htm
	$(ZIP) a $(DISTEXE) orangec/rule/*.rul orangec/help/*.* orangec/addon/*.* orangec/appdata
	$(ZIP) a $(DISTEXE) orangec/license.txt orangec/readme.txt orangec/relnotes.txt
	$(ZIP) -r0 -xr@orangec/src/xclude.lst a $(DISTSRC) orangec/src/*.adl orangec/src/*.cpp orangec/src/*.c orangec/src/*.h orangec/src/*.nas orangec/src/*.ase orangec/src/*.asi orangec/src/*.inc orangec/src/*.p orangec/src/*.rc orangec/src/*.spc orangec/src/*.app orangec/src/*.cfg orangec/src/makefile* orangec/src/*.mak orangec/src/*.txt orangec/src/copying orangec/src/*.def orangec/src/*.lst 
	$(ZIP) -r0 -xr@orangec/src/xclude.lst a $(DISTSRC) orangec/src/ocl/ocl.lic orangec/src/clibs/cpp/* orangec/src/clibs/platform/copyrght.asm orangec/src/help/*.* orangec/src/doc/*.*
	$(ZIP) -r0 a $(DISTSRC) orangec/src/clibs/platform/dos32/extender/*.* orangec/src/clibs/platform/dos32/extender/*
	$(ZIP) -r0 a $(DISTSRC) orangec/src/*.vcxproj orangec/src/*.vcxproj.filters orangec/src/*.sln
	$(ZIP) a $(DISTSRC) orangec/src/ocide/fonts/ttf-bitstream-vera-1.10/*.* orangec/src/ocide/res/*.* orangec/src/ocide/rule/*.rul orangec/src/ocide/rule/*.props
	$(ZIP) a $(DISTSRC) orangec/src/*.exe orangec/src/*.iss orangec/src/*.bmp orangec/src/config.bat orangec/src/ocidehld.bat
	$(ZIP) a $(DISTSRC) orangec/src/LICENSE.TXT orangec/license/*.* orangec/src/readme.txt orangec/src/relnotes.txt
	$(ZIP) a $(DISTSRC) orangec/src/clibs/repobj.bat orangec/src/copying orangec/src/ocl.lic orangec/src/addon.txt
	"/program files (x86)/inno setup 5/iscc" /Focc$(VERNAME) /O/orangec/dist orangec/src/occ.iss
	-rmdir $(DISTROOT)\appdata
