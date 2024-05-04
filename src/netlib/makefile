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

include ../pathext2.mak

NAME=netlib
DISTNAME=netlink
MAIN_FILE=NetLinkMain.cpp
INCLUDES=..$(PATHEXT2)util ..$(PATHEXT2)exefmt ..$(PATHEXT2)
CPP_DEPENDENCIES=$(wildcard *.cpp)
H_DEPENDENCIES=$(wildcard *.h)
LIB_DEPENDENCIES=util

ifneq "$(COMPILER)" "gcc-linux"
DEFINES:=TARGET_OS_WINDOWS FALLBACK_HOST_OS=\"win10\" FEATURE_APPHOST TARGET_WINDOWS
#fixme this next won't run in an x64 image....
DEFINES:= $(DEFINES) TARGET_X86 CURRENT_ARCH_NAME=\"x86\"
LIB_DEPENDENCIES:= $(LIB_DEPENDENCIES) libhostfxr
endif

include ../redirect.mak
NO_COMPARE=yes

DISTRIBUTE: copyexe
