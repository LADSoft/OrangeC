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

UTIL:=../util
ifeq "$(COMPILER)" "gcc-linux"
CC1:=g++
CC1FLAGS := -c -O2 -I $(UTIL) -DHAVE_UNISTD_H=1 -std=c++17
L1FLAGS :=
OUTSPECIFIER:= -o
OBJ:=o
EXE:=adl
else
CC1:=cl.exe
CC1FLAGS := /c /O2 /EHs /nologo /I$(UTIL) /DTARGET_OS_WINDOWS /std:c++17
L1FLAGS :=
OUTSPECIFIER:= /Fe
OBJ:=obj
EXE:=adl.exe
endif

vpath %.cpp $(UTIL)

%.$(OBJ): %.cpp
	$(CC1) -c $(CC1FLAGS) $<

UTIL_dependencies = ToolChain.$(OBJ) CmdSwitch.$(OBJ) CmdFiles.$(OBJ) Utils.$(OBJ) xml.$(OBJ) 
EXE_dependencies = ADLMain.$(OBJ) GenParser.$(OBJ) Tokenizer.$(OBJ) Loader.$(OBJ)  

tools : $(EXE)

$(EXE) : $(EXE_dependencies) $(UTIL_dependencies) 
	$(CC1) $(L1FLAGS) $(OUTSPECIFIER) $*  $^

DISTRIBUTE:
