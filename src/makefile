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

ifeq "$(COMPILER)" ""
ifdef MSYSTEM
export COMPILER:=gcc-linux
endif
endif

ifneq "$(COMPILER)" "gcc-linux"
SHELL=cmd.exe
endif

include pathext2.mak

ifeq "$(COMPILER)" "gcc-linux"
NULLDEV := /dev/null
else
NULLDEV := NUL
endif

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
	@$(MAKE) -f $(_TREEROOT) createdirs
	@$(MAKE) -f $(_TREEROOT) library
	@$(MAKE) -f $(_TREEROOT) exefile

distribute:
	@$(MAKE) -f $(_TREEROOT) distribute
	@$(MAKE) -f $(_TREEROOT) distribute_clibs
	@$(MAKE) -f $(_TREEROOT) zip

compare:
	@$(MAKE) -f $(_TREEROOT) compare

clean:  
	@$(MAKE) -f $(_TREEROOT) clean

library:
	@$(MAKE) -Cclibs

copyexes:
	@$(MAKE) -f $(_TREEROOT) distribute_exe

envbuild:
	@$(MAKE) -f $(_TREEROOT) distribute_self
	@$(MAKE) -f $(_TREEROOT) distribute_clibs_no_binary
	-copy orc.exe ..\bin

exebuild:
	@$(MAKE) -f $(_TREEROOT) distribute_self
	-copy orc.exe ..\bin
	@$(MAKE) -f $(_TREEROOT) createdirs
	@$(MAKE) -f $(_TREEROOT) library
	@$(MAKE) -f $(_TREEROOT) exefile
	@$(MAKE) -f $(_TREEROOT) distribute_exe

librarybuild:
	@$(MAKE) -Cclibs
	@$(MAKE) -f $(_TREEROOT) distribute_clibs

zip:
	@$(MAKE) -f $(_TREEROOT) zip

zip7z:
	@$(MAKE) -fzip7z.mak

fullbuild:
	@$(MAKE) -f $(_TREEROOT) distribute_self
	@$(MAKE) -f $(_TREEROOT) distribute_clibs_no_binary
	-copy orc.exe ..\bin
	@$(MAKE) -f $(_TREEROOT) createdirs
	@$(MAKE) -f $(_TREEROOT) library
	@$(MAKE) -f $(_TREEROOT) exefile
	@$(MAKE) -f $(_TREEROOT) distribute_exe
	@$(MAKE) -Cclibs
	@$(MAKE) -f $(_TREEROOT) distribute
	@$(MAKE) -f $(_TREEROOT) distribute_clibs
	@$(MAKE) -f $(_TREEROOT) zip

format:
	@$(MAKE) -f $(_TREEROOT) format

test:
	@$(MAKE) -Cclibs test

else
# when called from TREETOP.MAK do the below

DISTRIBUTE:
	$(COPYDIR) examples\*.* $(DISTEXAM) 2> $(NULLDEV)
	$(COPYDIR) examples\msdos\*.* $(DISTEXAM)\msdos 2> $(NULLDEV)
	$(COPYDIR) examples\system\*.* $(DISTEXAM)\system 2> $(NULLDEV)
	$(COPYDIR) examples\win32\*.* $(DISTEXAM)\win32 2> $(NULLDEV)
	$(COPYDIR) examples\win32\atc\*.* $(DISTEXAM)\win32\atc 2> $(NULLDEV)
	$(COPYDIR) examples\win32\listview\*.* $(DISTEXAM)\win32\listview 2> $(NULLDEV)
	$(COPYDIR) examples\win32\xmlview\*.* $(DISTEXAM)\win32\xmlview 2> $(NULLDEV)
	$(COPYDIR) examples\win32\RCDemo\*.* $(DISTEXAM)\win32\RCDemo 2> $(NULLDEV)
	$(COPYDIR) examples\win32\Huff\*.* $(DISTEXAM)\win32\Huff 2> $(NULLDEV)
	copy license.txt $(DISTROOT) 2> $(NULLDEV)
	copy readme.txt $(DISTROOT) 2> $(NULLDEV)
	copy occil.txt $(DISTROOT) 2> $(NULLDEV)
	copy relnotes.txt $(DISTROOT) 2> $(NULLDEV)
	copy addon.txt $(DISTADDON) 2> $(NULLDEV)
endif
