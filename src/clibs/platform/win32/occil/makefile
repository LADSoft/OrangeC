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

H_FILES = $(wildcard *.h)
C_DEPENDENCIES = $(wildcard *.c)

all:    lsmsilcrtl.dll occmsil.dll
$(eval $(FORMATTER))

occmsil.dll:
	-del occmsil.dll
	occ /Wd $(STDINCLUDE) /L$(SYSOBJECT) occmsil.c
	move occmsil.dll $(SYSOBJECT)

lsmsilcrtl.dll:
	copy lsmsilcrtl.dll $(SYSOBJECT)
	copy lsmsilcrtl6.dll $(SYSOBJECT)
	copy lsmsilcrtl7.dll $(SYSOBJECT)
	copy lsmsilcrtl8.dll $(SYSOBJECT)

build_lsmsilcrtl:
	csc /unsafe /langversion:7.3 /keyfile:lsmsilcrtl\occil.snk /t:library lsmsilcrtl\lsmsilcrtl.cs lsmsilcrtl\properties\assemblyinfo.cs
	dotnet build lsmsilcrtl\lsmsilcrtl6.csproj --configuration Release
	copy lsmsilcrtl\bin\x86\Release\net6.0\*.dll
	dotnet build lsmsilcrtl\lsmsilcrtl7.csproj --configuration Release
	copy lsmsilcrtl\bin\x86\Release\net7.0\*.dll
	dotnet build lsmsilcrtl\lsmsilcrtl8.csproj --configuration Release
	copy lsmsilcrtl\bin\x86\Release\net8.0\*.dll

DISTRIBUTE:
