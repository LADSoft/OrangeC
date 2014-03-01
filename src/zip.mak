#	Software License Agreement (BSD License)
#	
#	Copyright (c) 1997-2009, David Lindauer, (LADSoft).
#	All rights reserved.
#	
#	Redistribution and use of this software in source and binary forms, 
#	with or without modification, are permitted provided that the following 
#	conditions are met:

#	* Redistributions of source code must retain the above
#	  copyright notice, this list of conditions and the
#	  following disclaimer.

#	* Redistributions in binary form must reproduce the above
#	  copyright notice, this list of conditions and the
#	  following disclaimer in the documentation and/or other
#	  materials provided with the distribution.

#	* Neither the name of LADSoft nor the names of its
#	  contributors may be used to endorse or promote products
#	  derived from this software without specific prior
#	  written permission of LADSoft.

#	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
#	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
#	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
#	PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
#	OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
#	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
#	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
#	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
#	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
#	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#	contact information:
#		email: TouchStone222@runbox.com <David Lindauer>
VERNAME := $(word 3, $(shell type,\orangec\src\version.h))
VERNAME := $(subst ",,$(VERNAME))
VERNAME := $(subst .,,$(VERNAME))

DISTEXE=\orangec\dist\occ$(VERNAME)e.zip
DISTSRC=\orangec\dist\occ$(VERNAME)s.zip

DISTRIBUTE:
	-del $(DISTEXE)
	-del $(DISTSRC)
	$(ZIP) -r0 a $(DISTEXE) orangec\bin\*.exe orangec\bin\*.app orangec\bin\*.spc orangec\bin\*.dll orangec\bin\*.cfg orangec\lib\*.* orangec\include\*.* orangec\examples\*.* orangec\doc\*.* orangec\license\*.* 
   	$(ZIP) a $(DISTEXE) orangec\lib\* orangec\include\* orangec\examples\* orangec\doc\* orangec\license\*
	$(ZIP) d $(DISTEXE) doc\tools.htm
	$(ZIP) a $(DISTEXE) orangec\rule\*.rul orangec\help\*.* orangec\addon\*.*
	$(ZIP) a $(DISTEXE) orangec\license.txt orangec\readme.txt orangec\relnotes.txt
	$(ZIP) -r0 -xr@orangec\src\xclude.lst a $(DISTSRC) orangec\src\*.adl orangec\src\*.cpp orangec\src\*.c orangec\src\*.h orangec\src\*.nas orangec\src\*.ase orangec\src\*.asi orangec\src\*.inc orangec\src\*.p orangec\src\*.rc orangec\src\*.spc orangec\src\*.app orangec\src\makefile* orangec\src\*.mak orangec\src\*.txt orangec\src\copying orangec\src\*.def orangec\src\*.lst
	$(ZIP) -r0 -xr@orangec\src\xclude.lst a $(DISTSRC) orangec\src\clibs\platform\dos32\extender\*.bin orangec\src\clibs\platform\dos32\extender\*.exe orangec\src\help\*.* orangec\src\doc\*.*
	$(ZIP) -r0 a $(DISTSRC) orangec\src\*.vcxproj orangec\src\*.vcxproj.filters orangec\src\*.sln
	$(ZIP) a $(DISTSRC) orangec\src\ocide\fonts\ttf-bitstream-vera-1.10\*.* orangec\src\ocide\res\*.* orangec\src\ocide\rule\*.rul orangec\src\ocide\rule\*.props
	$(ZIP) a $(DISTSRC) orangec\src\*.exe orangec\src\*.iss orangec\src\*.bmp orangec\src\config.bat orangec\src\ocidehld.bat
	$(ZIP) a $(DISTSRC) orangec\src\license.txt orangec\src\relnotes.txt orangec\src\clibs\repobj.bat
	iscc /Focc$(VERNAME) /O\orangec\dist orangec\src\occ.iss
