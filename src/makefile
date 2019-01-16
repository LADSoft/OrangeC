# Software License Agreement
# 
#     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

include pathext2.mak

ifndef _TREEROOT
ifeq "$(COMPILER)" "gcc-linux"
CURRENT := $(CURDIR)
else
CURRENT := $(subst /,\,$(CURDIR))

endif

_TREEROOT := $(CURRENT)$(PATHEXT2)treetop.mak
_TREETARGET := $(CURRENT)
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
	@$(MAKE) -f $(_TREEROOT) library
	@$(MAKE) -f $(_TREEROOT) exefile

distribute:
	@$(MAKE) -f $(_TREEROOT) distribute
	@$(MAKE) -f $(_TREEROOT) distribute_clibs
	@$(MAKE) -f $(_TREEROOT) zip

clean:  
	@$(MAKE) -f $(_TREEROOT) clean

library:
	@$(MAKE) -Cclibs

copyexes:
	@$(MAKE) -f $(_TREEROOT) distribute_exe

buildenv:
	@$(MAKE) -f $(_TREEROOT) distribute_self
	@$(MAKE) -f $(_TREEROOT) distribute_clibs_no_binary
	-copy orc.exe ..\bin

buildenv1:
	@$(MAKE) -f $(_TREEROOT) distribute_self
	@$(MAKE) -f $(_TREEROOT) distribute_clibs_no_binary
	-copy orc.exe ..\bin
tools1: buildenv1 
	@$(MAKE) -f $(_TREEROOT) library
	@$(MAKE) -f $(_TREEROOT) exefile
copyexes1: tools1
	@$(MAKE) -f $(_TREEROOT) distribute_exe
library1: copyexes1
	@$(MAKE) -Cclibs
distribute1: library1
	@$(MAKE) -f $(_TREEROOT) distribute
	@$(MAKE) -f $(_TREEROOT) distribute_clibs

zip: distribute1
	@$(MAKE) -f $(_TREEROOT) zip

fullbuild: zip

format:
	@$(MAKE) -f $(_TREEROOT) format

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
	copy license.txt $(DISTROOT)
	copy readme.txt $(DISTROOT)
	copy relnotes.txt $(DISTROOT)
	copy addon.txt $(DISTADDON)
endif
