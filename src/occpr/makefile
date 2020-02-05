# Software License Agreement
# 
#     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

NAME=occpr
MAIN_FILE=occparse.cpp
INCLUDES=..$(PATHEXT2)util ..$(PATHEXT2)ocpp ..$(PATHEXT2)occparse ..$(PATHEXT2)occopt ..$(PATHEXT2)objlib ..$(PATHEXT2)netlib ..$(PATHEXT2)oasm
CPP_DEPENDENCIES=$(wildcard *.cpp) \
     beinterf.cpp \
     browse.cpp \
     ccerr.cpp \
     constopt.cpp \
     cppbltin.cpp \
     cpplookup.cpp \
     crc32.cpp \
     debug.cpp \
     declare.cpp \
     declcons.cpp \
     declcpp.cpp \
     expr.cpp \
     exprcpp.cpp \
     floatconv.cpp \
     help.cpp \
     import.cpp \
     inasm.cpp \
     init.cpp \
     initbackend.cpp \
     inline.cpp \
     lambda.cpp \
     lex.cpp \
     libcxx.cpp \
     mangle.cpp \
     osutil.cpp \
     property.cpp \
     rtti.cpp \
     stmt.cpp \
     SymbolManager.cpp \
     symtab.cpp \
     template.cpp \
     types.cpp \
     unmangle.cpp \
     wseh.cpp

LIB_DEPENDENCIES=ocpplib objlib occopt util netlib oasm sqlite3
DEFINES=x64 PARSER_ONLY
H_FILES=$(wildcard *.h)

vpath %.cpp ..$(PATHEXT2)occparse

include ../redirect.mak

DISTRIBUTE: copyexe
