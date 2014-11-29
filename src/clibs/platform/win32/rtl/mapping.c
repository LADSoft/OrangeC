/*
	Software License Agreement (BSD License)
	
	Copyright (c) 1997-2013, David Lindauer, (LADSoft).
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
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <windows.h>
#include <sys/mman.h>

int _RTL_FUNC sysconf(int type)
{
    assert(type == _SC_PAGESIZE);
    return 4096;
}
void * _RTL_FUNC mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
{
    void *rv = NULL;
    HANDLE map;
    assert(addr == 0 && prot == PROT_READ && flags == MAP_PRIVATE);
    map = CreateFileMapping((HANDLE)_get_osfhandle(fildes), NULL, PAGE_READONLY | SEC_RESERVE, 0, 0, NULL);
    if (map != 0)
    {
        rv = MapViewOfFile(map, FILE_MAP_READ, 0, off, 0);
        CloseHandle(map);
    }
    return rv;
}
int	_RTL_FUNC munmap(void *addr, size_t len)
{
    UnmapViewOfFile(addr);
    return 0;
}
