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

ZIP:="c:\program files\7-zip\7z" -tzip 
all:
	$(ZIP) -r -x@src\dosxclude.lst a src\doszip bin\*.* bin\*
	$(ZIP) -r -x@src\dosxclude.lst a src\doszip doc\tools.htm
	$(ZIP) -r -x@src\dosxclude.lst a src\doszip examples\*.* examples\*
	$(ZIP) -r -x@src\dosxclude.lst a src\doszip include\*.* include\*
	$(ZIP) -r -x@src\dosxclude.lst a src\doszip lib\*.* lib\*
	$(ZIP) -r -x@src\dosxclude.lst a src\doszip license\*.* license\*
	$(ZIP) -r -x@src\dosxclude.lst a src\doszip addon\hx.*
	$(ZIP) a src\doszip LICENSE.TXT
	$(ZIP) a src\doszip readme.txt
	$(ZIP) a src\doszip relnotes.txt
#	$(ZIP) -r -x@src\dosxclude.lst a src\doszip rule\*.* rule\*
