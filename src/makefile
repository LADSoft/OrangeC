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

ifndef _TREEROOT
_TREEROOT := $(CURDIR)/treetop.mak
_TREETARGET := $(CURDIR)
export _TREEROOT
export _TREETARGET

# by default this makefile just switches omake to 'TREE' mode
# That will load TREETOP.MAK which will eventually load CONFIG.MAK to 
# load compiler scripts
#
# beware - I haven't got dependency checking for include files implemented yet
#
export NOMAKEDIR

tools: 
	@$(MAKE) -f $(_TREEROOT)

distribute:
	@$(MAKE) -f $(_TREEROOT) distribute


clean:  
	@$(MAKE) -f $(_TREEROOT) clean

library:
	@$(MAKE) -Cclibs

copyexes:
	@$(MAKE) -f $(_TREEROOT) exeDISTRIBUTE

buildenv:
	@$(MAKE) -f $(_TREEROOT) --eval="BUILDENV:=1" distribute
	-copy orc.exe ..\bin

buildenv1:
	@$(MAKE) -f $(_TREEROOT) --eval="BUILDENV:=1" distribute
	-copy orc.exe ..\bin
tools1: buildenv1 
	@$(MAKE) -f $(_TREEROOT)
copyexes1: tools1
	@$(MAKE) -f $(_TREEROOT) exeDISTRIBUTE
library1: copyexes1
	@$(MAKE) -Cclibs
distribute1: library1
	@$(MAKE) -f $(_TREEROOT) distribute

fullbuild: distribute1

else
# when called from TREETOP.MAK do the below

DISTRIBUTE:
	$(COPYDIR) examples\%.% $(DISTEXAM)
	$(COPYDIR) examples\msdos\%.% $(DISTEXAM)\msdos
	$(COPYDIR) examples\system\%.% $(DISTEXAM)\system
	$(COPYDIR) examples\win32\%.% $(DISTEXAM)\win32
	$(COPYDIR) examples\win32\atc\%.% $(DISTEXAM)\win32\atc
	$(COPYDIR) examples\win32\listview\%.% $(DISTEXAM)\win32\listview
	$(COPYDIR) examples\win32\xmlview\%.% $(DISTEXAM)\win32\xmlview
	$(COPYDIR) examples\win32\RCDemo\%.% $(DISTEXAM)\win32\RCDemo
	$(COPYDIR) examples\win32\Huff\%.% $(DISTEXAM)\win32\Huff
	$(COPYDIR) doc\%.% $(DISTDOC)
	$(COPYDIR) doc\general\%.% $(DISTDOC)\general
	$(COPYDIR) doc\oasm\%.% $(DISTDOC)\oasm
	$(COPYDIR) doc\occ\%.% $(DISTDOC)\occ
	$(COPYDIR) doc\ogrep\%.% $(DISTDOC)\ogrep
	$(COPYDIR) doc\olink\%.% $(DISTDOC)\olink
	$(COPYDIR) doc\omake\%.% $(DISTDOC)\omake    
	copy license.txt $(DISTROOT)
	copy readme.txt $(DISTROOT)
	copy relnotes.txt $(DISTROOT)
	copy addon.txt $(DISTADDON)
ifdef BUILDENV
	@$(MAKE) -C$(DISTROOT)\src\clibs DISTRIBUTE --eval="BUILDENV:=1"
else
	@$(MAKE) -C$(DISTROOT)\src\clibs DISTRIBUTE
ifdef WITHMSDOS
# this requires CC386 be installed since it relies on far pointer support
# so I don't make it a part of the default install
	@$(MAKE) -C$(DISTROOT)\src\..\ -f $(realpath .\doszip.mak)
	@$(MAKE) -f makefile.le -C$(DISTROOT)\src\dos\install
endif
	@$(MAKE) -C\ -f $(realpath .\zip.mak)
endif
endif
