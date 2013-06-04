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
#include <windows.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <wchar.h>
#include <locale.h>
#include <io.h>
#include "libp.h"

time_t __to_timet(FILETIME *time);

static void format_finddata(struct _finddata_t *buf, LPWIN32_FIND_DATA data)
{
	buf->attrib = 0;
	if (data->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		buf->attrib |= _A_ARCH ;
	if (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		buf->attrib |= _A_SUBDIR ;
	if (data->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
		buf->attrib |= _A_HIDDEN ;
	if (data->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
		buf->attrib |= _A_RDONLY ;
	if (data->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
		buf->attrib |= _A_SYSTEM ;
	buf->time_access = __to_timet(&data->ftLastAccessTime) ;
	buf->time_write = __to_timet(&data->ftLastWriteTime) ;
	buf->time_create = __to_timet(&data->ftCreationTime) ;
	if (data->nFileSizeHigh)
		*(DWORD *)(&buf->size) = 0xffffffff ;
	else
		*(DWORD *)(&buf->size) = data->nFileSizeLow;
	strcpy(buf->name,data->cFileName);
}
long _RTL_FUNC _findfirst(const char *string, struct _finddata_t *buf)
{
	WIN32_FIND_DATA find_data ;
	HANDLE val ;
	if (!buf)
	{
		errno = EINVAL;
		return -1;
	}
	val = FindFirstFile(string,&find_data);
	memset(buf,0,sizeof(*buf));
	if (val == INVALID_HANDLE_VALUE)
	{
		errno = ENOENT;
		return -1;
	}
	format_finddata(buf,&find_data);
	return (int)val;
	
}
int _RTL_FUNC _findnext(long handle, struct _finddata_t *buf)
{
	WIN32_FIND_DATA find_data ;
	if (!buf)
	{
		errno = EINVAL;
		return -1;
	}
	if (!FindNextFile((HANDLE)handle, &find_data)) {
		errno = ENOENT;
		return -1;
	}
	format_finddata(buf,&find_data) ;

	return 0 ;
}
int _RTL_FUNC _findclose(long handle)
{
	FindClose((HANDLE)handle);
	return 0;
}
