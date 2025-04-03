/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#include <errno.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <io.h>
#include <sys/stat.h>
#include "libp.h"

#define MAX_COUNT 100
#define MAX_BUCKETS (MAX_COUNT * 3)
#define BUCKET_SKIP 7

#pragma netlib System;

using namespace System;
using namespace System::IO;
using namespace System::Text;

//time_t __to_timet(FILETIME *time);

static long handles[MAX_BUCKETS];
static __object[MAX_BUCKETS] fileCache, [MAX_BUCKETS]dirCache;
static int dirPos[MAX_BUCKETS], filePos[MAX_BUCKETS];
static int count = 0;

static int insertHandle(long handle)
{
	int n = handle % (MAX_BUCKETS);
	while (handles[n])
	{
		n += BUCKET_SKIP;
		if (n >= MAX_BUCKETS)
			n =- MAX_BUCKETS;
	}
	handles[n] = handle;
	return n;
}
static int findHandle(long handle)
{
	int n = handle % (MAX_BUCKETS);
	while (handles[n])
	{
		if (handles[n] == handle)
		{
			return n;
		}
		n += BUCKET_SKIP;
		if (n >= MAX_BUCKETS)
			n =- MAX_BUCKETS;
	}
	return -1 ;
}
static int format_finddata(struct _finddata_t *buf, __string name, int dir)
{
	struct _stat data;
	char aname[260];
	unsigned char[] bytes = Encoding::ASCII.GetBytes(name);
	int i,j;
	buf->attrib = 0;
	if (dir)
		buf->attrib |= _A_SUBDIR;
	else
	{
		for (i=0; i < bytes.Length; i++)
			aname[i] = bytes[i];
		aname[i] = 0;
		if (!_stat(aname, &data))
		{
			buf->time_access = data.st_atime ;
			buf->time_write = data.st_mtime ;
			buf->time_create = data.st_ctime;
			*(unsigned *)(&buf->size) = data.st_size;
			if (!(data.st_mode & S_IWRITE))
				buf->attrib |= _A_RDONLY ;
			// not supporting archive hidden or system flags in this implementation
/*
	if (data->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		buf->attrib |= _A_ARCH ;
	if (data->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
		buf->attrib |= _A_HIDDEN ;
	if (data->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
		buf->attrib |= _A_SYSTEM ;
*/
		}
	}
	for (i=0,j=0; i < bytes.Length; i++)
		if (j || i && bytes[i-1] == '\\')
		{
			buf->name[j++] = bytes[i];
		}
	if (!j)
		for (j=0; j < bytes.Length; j++)
			buf->name[j] = bytes[j];
	buf->name[j] = 0;
	return 0;
}
int _RTL_FUNC _findnext(long handle, struct _finddata_t *buf)
{
	if (!buf)
	{
		errno = EINVAL;
		return -1;
	}
	int index = findHandle(handle);
	if (index >= 0)
	{
		__string [] p = (__string[])(dirCache[index]);
		if (dirPos[index] < p.Length)
		{
			return format_finddata(buf, p[dirPos[index]++], 1);
		}
		else
		{ 
			p = (__string[])(fileCache[index]);
			if (filePos[index] < p.Length)
			{
				return format_finddata(buf, p[filePos[index]++], 0);
			}
		}
	}
	errno = ENOENT;
	return -1;
}
int _RTL_FUNC _findclose(long handle)
{
	int index = findHandle(handle);
	if (index >= 0)
	{
		count--;
		handles[index] = 0;
		dirCache[index] = NULL;
		fileCache[index] = NULL;		
		return 0;
	}
	errno = ENOENT;
	return -1;
}
long _RTL_FUNC _findfirst(const char *string, struct _finddata_t *buf)
{
	if (!buf)
	{
		errno = EINVAL;
		return -1;
	}
	if (count < MAX_COUNT )
	{
		Random random;
		__string[] files;
		int index;
		long handle;
		char path[260], file[260], *p;
		do
		{
			handle = random.Next();
		} while (findHandle(handle)>=0);
		index = insertHandle(handle);
		count ++;
		p = strrchr(string, '\\');
		if (p)
		{
			*p = 0;
			strcpy(path, string);
			strcpy(file, p+1);
		}
		else
		{
			strcpy(path, ".\\");
			strcpy(file, string);
		}
		fileCache[index] = (__object)Directory::GetFiles(path, file);
		dirCache[index] = (__object)Directory::GetDirectories(path, file);
		filePos[index] = 0;
		dirPos[index] = 0;
		if (!_findnext(handle, buf))
                     return handle;
                _findclose(handle);
	}
	errno = ENOENT;
	return -1;
}
