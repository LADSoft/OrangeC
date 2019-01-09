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
include ../../../pathext2.mak

NAME=netlib
INCLUDES=..\..\..\util;..\..\..\exefmt
CPP_DEPENDENCIES=$(wildcard *.cpp)
C_DEPENDENCIES=$(wildcard *.c)
ifndef _TREEROOT
_TREEROOT := $(CURDIR)$(PATHEXT2)..$(PATHEXT2)..$(PATHEXT2)..$(PATHEXT2)treetop.mak
_TREETARGET := $(CURDIR)
export _TREEROOT
export _TREETARGET

redirect:
	@$(MAKE) -f$(_TREEROOT)
format:
	@$(MAKE) -f $(_TREEROOT) format
endif

DISTRIBUTE:
