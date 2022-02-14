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

ifneq "$(COMPILER)" "gcc-linux"
SHELL=cmd.exe
endif

ZIP:="c:/program files/7-zip/7z" -tzip -bd

OCC_VERSION := $(word 3, $(shell cmd /C type \orangec\src\version.h))
OCC_VERSION := $(subst ",,$(OCC_VERSION))
export OCC_VERSION
VERNAME := $(subst .,,$(OCC_VERSION))

DEST=/orangec/dist/ZippedBinaries$(VERNAME).7z
SRC=/orangec/dist/ZippedBinaries$(VERNAME).zip

DISTRIBUTE:
ifneq "$(ORANGEC_ONLY)" "YES"

	7z x $(SRC) *
	7z -r -mx=9 a $(DEST) orangec\* 
	cd ..
endif
