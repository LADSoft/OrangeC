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

#FALLBACK_HOST_OS="win10";FEATURE_APPHOST;UNICODE;TARGET_WINDOWS;REPO_COMMIT_HASH="static";HOST_POLICY_PKG_REL_DIR="static";HOST_POLICY_PKG_NAME="static";TARGET_X86;CURRENT_ARCH_NAME="x86";WIN32;NDEBUG;_LIB;%(PreprocessorDefinitions)

NAME=objlib
MAIN_FILE=test2.cpp
INCLUDES=..$(PATHEXT2)util
CPP_DEPENDENCIES=$(wildcard *.cpp)
LIB_DEPENDENCIES=util
EXCLUDE=ObjIO test2
H_FILES=$(wildcard *.h)
NO_COMPARE=yes
DEFINES=FALLBACK_HOST_OS=\"win10\" FEATURE_APPHOST UNICODE TARGET_WINDOWS TARGET_X86 CURRENT_ARCH_NAME=\"x86\"
include ../redirect.mak

DISTRIBUTE: