# Software License Agreement
# 
#     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

include ../pathext2.mak

NAME=occparse
MAIN_FILE=occparse.cpp
INCLUDES=..$(PATHEXT2)util ..$(PATHEXT2)ocpp ..$(PATHEXT2)occopt ..$(PATHEXT2)occ ..$(PATHEXT2)occil ..$(PATHEXT2)objlib ..$(PATHEXT2)netlib ..$(PATHEXT2)oasm
CPP_DEPENDENCIES=$(wildcard *.cpp)
LIB_DEPENDENCIES=occ ocpplib oasm occopt util objlib occil netlib
ifneq "$(COMPILER)" "gcc-linux"
LIB_DEPENDENCIES:= $(LIB_DEPENDENCIES) libhostfxr
endif
ifeq "$(COMPILER)" "MINGW64"
LIB_DEPENDENCIES:=$(LIB_DEPENDENCIES) occparse occil libhostfxr
endif
ifeq "$(COMPILER)" "gcc-linux"
LIB_DEPENDENCIES:=$(LIB_DEPENDENCIES) occparse occil
endif
DEFINES=x64 USE_LONGLONG
H_FILES=$(wildcard *.h)

include ../redirect.mak

DISTRIBUTE: copyexe
