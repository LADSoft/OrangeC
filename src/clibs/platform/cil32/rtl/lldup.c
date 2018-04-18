/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <errno.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <sys\stat.h>

using namespace System;
using namespace System::IO;

int __llAllocateHandle(__string name, FileStream file, FileAccess access, FileShare share);
FileStream __llGetHandle(int n);
void __SetFileErrno(Exception e);
size_t __ll_getpos(int);
int __ll_seek(int, size_t, int);
FileAccess __llGetAccess(int);
FileShare __llGetShare(int);

int __ll_dup(int handle)
{
    int rv = 0;
    FileStream strm = __llGetHandle(handle);
    __try
    {
	FileAccess access = __llGetAccess(handle);
	FileShare share = __llGetShare(handle);
        FileStream newstrm = File::Open(strm.Name, FileMode::Open, access, share);
        rv = __llAllocateHandle(newstrm.Name, newstrm, access, share);
        if (rv < 0)
        {
             strm.Dispose();
             errno = ENOMEM;
             rv = 0;
        }
        else
        {
             __ll_seek(rv, __ll_getpos(handle), SEEK_SET);
        }
    }
    __catch(Exception e)
    {
        __SetFileErrno(e);
    }
    return rv;
}
