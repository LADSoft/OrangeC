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

ifndef _TREEROOT
# by default this makefile just switches omake to 'TREE' mode
# That will load TREETOP.MAK which will eventually load CONFIG.MAK to 
# load compiler scripts
#
# beware - I haven't got dependency checking for include files implemented yet
#
tools: 
	@$(MAKE) /T

distribute:
	@$(MAKE) /T distribute

fullbuild: buildenv tools copyexes library distribute

clean:  
	@$(MAKE) /T clean

library:
	@$(MAKE) /Cclibs

copyexes:
	@$(MAKE) /T exeDISTRIBUTE

buildenv:
	@$(MAKE) /T -DBUILDENV distribute
	-copy orc.exe ..\bin

else
# when called from TREETOP.MAK do the below

DISTRIBUTE:
	$(COPYDIR) examples\%.% $(DISTEXAM)
	$(COPYDIR) examples\msdos\%.% $(DISTEXAM)\msdos
	$(COPYDIR) examples\system\%.% $(DISTEXAM)\system
	$(COPYDIR) examples\win32\%.% $(DISTEXAM)\win32
	$(COPYDIR) examples\win32\listview\%.% $(DISTEXAM)\win32\listview
	$(COPYDIR) examples\win32\xmlview\%.% $(DISTEXAM)\win32\xmlview
	$(COPYDIR) examples\win32\RCDemo\%.% $(DISTEXAM)\win32\RCDemo
	$(COPYDIR) examples\win32\Huff\%.% $(DISTEXAM)\win32\Huff
	$(COPYDIR) doc\%.% $(DISTDOC)
	$(COPYDIR) doc\general\%.% $(DISTDOC)\general
	$(COPYDIR) doc\oasm\%.% $(DISTDOC)\oasm
	$(COPYDIR) doc\occ\%.% $(DISTDOC)\occ
	$(COPYDIR) doc\ogrep\%.% $(DISTDOC)\ogrep
	$(COPYDIR) doc\olink\%.% $(DISTDOC)\olink
	$(COPYDIR) doc\omake\%.% $(DISTDOC)\omake
	copy license.txt $(DISTROOT)
	copy readme.txt $(DISTROOT)
	copy relnotes.txt $(DISTROOT)
	copy berkely.lic $(DISTLIC)
	copy copying $(DISTLIC)
	copy ocl.lic $(DISTLIC)
	copy addon.txt $(DISTADDON)
ifdef BUILDENV
	@$(MAKE) /C$(DISTROOT)\src\clibs DISTRIBUTE /DBUILDENV
else
	@$(MAKE) /C$(DISTROOT)\src\clibs DISTRIBUTE
ifdef WITHMSDOS
# this requires CC386 be installed since it relies on far pointer support
# so I don't make it a part of the default install
	@$(MAKE) /C$(DISTROOT)\src\..\ -f$(realpath .\doszip.mak)
	@$(MAKE) /C$(DISTROOT)\src\dos\install -fmakefile.le
endif
	@$(MAKE) /Cc:\ -f$(realpath .\zip.mak)
endif
endif
