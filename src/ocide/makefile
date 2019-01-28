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

include ../pathext2.mak

ifneq "$(COMPILER)" "MINGW64"
NAME=ocide
TARGET=GUI
MAIN_FILE= ocide.c
DEF_DEPENDENCIES=ocide.def
INCLUDES=..$(PATHEXT2)sqlite3
C_DEPENDENCIES= $(wildcard *.c)
RC_DEPENDENCIES=ociderc.rc
LIB_DEPENDENCIES=sqlite3
#OBJ_DEPENDENCIES=$(PATHEXT2)bcc55$(PATHEXT2)lib$(PATHEXT2)wildargs.obj
DEFINES=GUI
H_FILES=$(wildcard *.h)

vpath %.c ..$(PATHEXT2)util

include ../redirect.mak

DISTRIBUTE: copyexe
else
DISTRIBUTE:
endif
	copy rule$(PATHEXT2)*.rul $(DISTROOT)$(PATHEXT2)rule
	copy rule$(PATHEXT2)*.props $(DISTBIN)
	copy fonts$(PATHEXT2)ttf-bitstream-vera-1.10$(PATHEXT2)veramono.ttf $(DISTBIN)