# Software License Agreement
# 
#     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

NAME=ocpplib
DISTNAME=ocpp
MAIN_FILE=ppMain.cpp
INCLUDES=..$(PATHEXT2)util
CPP_DEPENDENCIES=$(wildcard *.cpp)
LIB_DEPENDENCIES=util
DEFINES=
H_FILES=$(wildcard *.h)

include ../redirect.mak

DISTRIBUTE:
	copy ocpplib.exe $(DISTBIN)$(PATHEXT2)ocpp.exe
	copy target.cfg $(DISTBIN)$(PATHEXT2)ocpp.cfg