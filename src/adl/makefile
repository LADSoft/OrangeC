# Software License Agreement
# 
#     Copyright(C) 1994-2026 David Lindauer, (LADSoft)
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

# this make file does not follow the pattern of other make files as it builds
# a program that is used during the build

ifeq "$(COMPILER)" "gcc-linux"
CC1:=g++
CC1FLAGS := -O2 -I ..\util -std=c++17
OUTSPECIFIER:= -o
else
CC1:=cl.exe
CC1FLAGS := /O2 /EHs /nologo /I../util /DTARGET_OS_WINDOWS /std:c++17
OUTSPECIFIER:= /Fe
endif

vpath %.cpp ..\util

EXE_dependencies = \
		CmdSwitch.cpp \
		CmdFiles.cpp \
		Utils.cpp \
		xml.cpp \
                ToolChain.cpp \
		GenParser.cpp \
		Loader.cpp \
		Tokenizer.cpp \
		ADLMain.cpp

tools : adl.exe

adl.exe: $(EXE_dependencies)
	$(CC1) $(CC1FLAGS) $(OUTSPECIFIER)$*  $(EXE_dependencies)

DISTRIBUTE: