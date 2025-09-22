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

include pathext2.mak

ifeq "$(COMPILER)" "gcc-linux"
NULLDEV := /dev/null
COPY := cp
else
NULLDEV := NUL
COPY := copy
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


ifneq "$(ORANGEC_ONLY)" ""
WITHOUT_OCCIL:=$(ORANGEC_ONLY)
WITHOUT_ZIP:=$(ORANGEC_ONLY)
export WITHOUT_OCCIL
export WITHOUT_ZIP
endif

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

dist-clean:
	@$(MAKE) -f $(_TREEROOT) dist-clean
ifneq "COMPILER" "gcc-linux"
	@$(MAKE) -f $(_TREEROOT) COMPILER=OCC clean
	@$(MAKE) -f $(_TREEROOT) COMPILER=MINGW64 clean
	@$(MAKE) -f $(_TREEROOT) COMPILER=CLANG clean
	@$(MAKE) -f $(_TREEROOT) COMPILER=MS clean
else
	@$(MAKE) -f $(_TREEROOT) COMPILER=gcc-linux clean
endif
 
library:
	@$(MAKE) -Cclibs

copyexes:
	@$(MAKE) -f $(_TREEROOT) distribute_exe

envbuild:
	@$(MAKE) -f $(_TREEROOT) distribute_self
	@$(MAKE) -f $(_TREEROOT) distribute_clibs_no_binary
	-$(COPY) orc.exe ..$(PATHEXT2)bin

exebuild:
	@$(MAKE) -f $(_TREEROOT) distribute_self
	-$(COPY) orc.exe ..$(PATHEXT2)bin
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
	-$(COPY) orc.exe ..$(PATHEXT2)bin
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
	$(COPYDIR) examples$(PATHEXT2)%.% $(DISTEXAM) 2> $(NULLDEV)
	$(COPYDIR) examples$(PATHEXT2)msdos$(PATHEXT2)%.% $(DISTEXAM)$(PATHEXT2)msdos 2> $(NULLDEV)
	$(COPYDIR) examples$(PATHEXT2)system$(PATHEXT2)%.% $(DISTEXAM)$(PATHEXT2)system 2> $(NULLDEV)
	$(COPYDIR) examples$(PATHEXT2)win32$(PATHEXT2)%.% $(DISTEXAM)$(PATHEXT2)win32 2> $(NULLDEV)
	$(COPYDIR) examples$(PATHEXT2)win32$(PATHEXT2)atc$(PATHEXT2)%.% $(DISTEXAM)$(PATHEXT2)win32$(PATHEXT2)atc 2> $(NULLDEV)
	$(COPYDIR) examples$(PATHEXT2)win32$(PATHEXT2)listview$(PATHEXT2)%.% $(DISTEXAM)$(PATHEXT2)win32$(PATHEXT2)listview 2> $(NULLDEV)
	$(COPYDIR) examples$(PATHEXT2)win32$(PATHEXT2)xmlview$(PATHEXT2)%.% $(DISTEXAM)$(PATHEXT2)win32$(PATHEXT2)xmlview 2> $(NULLDEV)
	$(COPYDIR) examples$(PATHEXT2)win32$(PATHEXT2)RCDemo$(PATHEXT2)%.% $(DISTEXAM)$(PATHEXT2)win32$(PATHEXT2)RCDemo 2> $(NULLDEV)
	$(COPYDIR) examples$(PATHEXT2)win32$(PATHEXT2)Huff$(PATHEXT2)%.% $(DISTEXAM)$(PATHEXT2)win32$(PATHEXT2)Huff 2> $(NULLDEV)
	$(COPY) license.txt $(DISTROOT) 2> $(NULLDEV)
	$(COPY) readme.txt $(DISTROOT) 2> $(NULLDEV)
	$(COPY) occil.txt $(DISTROOT) 2> $(NULLDEV)
	$(COPY) relnotes.txt $(DISTROOT) 2> $(NULLDEV)
	$(COPY) addon.txt $(DISTADDON) 2> $(NULLDEV)
endif
