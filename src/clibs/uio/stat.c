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
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <ctype.h>
#include <dos.h>
#include "libp.h"
#include <sys/stat.h>
#include <dir.h>
extern char __uidrives[HANDLE_MAX] ;
extern int __uiflags[HANDLE_MAX] ;
extern int __uihandles[HANDLE_MAX];

int _RTL_FUNC _fstat(int handle, struct _stat *__statbuf)
{
   int hand,rv ;

   __ll_enter_critical() ;   
   hand = __uiohandle(handle) ;
   if (hand < 0) {
      __ll_exit_critical() ;
      return -1 ;
   }

   memset (__statbuf,0,sizeof (*__statbuf)) ;

   __statbuf->st_mode = 0 ;
 
   if (__ll_isatty(hand)) {
      __statbuf->st_mode |= S_IFCHR ;
      __statbuf->st_rdev = __statbuf->st_dev = handle ;
   } else {
      __statbuf->st_mode = S_IREAD ;
      if (__uiflags[handle] & UIF_WRITEABLE)
         __statbuf->st_mode |= S_IWRITE ;
      __statbuf->st_mode |= S_IFREG ;
      __statbuf->st_rdev = __statbuf->st_dev = __uidrives[handle] ;
      if ((__statbuf->st_size = filelength(handle)) == -1) {
         errno = EBADF ;
         __ll_exit_critical() ;
         return -1 ;
      }
   }
   __statbuf->st_ino = 0 ;
   __statbuf->st_uid = 0 ;
   __statbuf->st_nlink = 1 ;

   /* llstat will return times */
   rv = __ll_stat(hand, __statbuf) ;
   __ll_exit_critical() ;
   return rv ;
}
int _RTL_FUNC fstat(int handle, struct stat *__statbuf)
{
   return _fstat(handle, (struct _stat *)__statbuf) ;
}
int _RTL_FUNC _stat(char *path, struct _stat *__statbuf)
{
	int fd, rv, rootdir;
	char pbuf[265];
	memset(__statbuf, 0, sizeof(__statbuf));
	if (!strcmp(path,"."))
		path = getcwd(pbuf, 265);
	rootdir = ((path[1] == ':' &&
			(path[2] == '\\' || path[2] == '/') &&
			(path[3] == '.' || path[3] == '\0')) ||
			((path[0] == '\\' || path[0] == '/') &&
			(path[1] == '.' || path[1] == '\0')));
	if (!rootdir)
	{
		int l = strlen(path);
		if ((path[l-1] == '/' || path[l-1] == '\\'))
		{
			path = strcpy(pbuf, path);
			path[l-1] = '\0';
		}
	}
	rv = __ll_namedstat(path, __statbuf);   
	if (rv == -1)
	{
		if (rootdir)
		{
			memset(__statbuf, 0, sizeof(*__statbuf));
			__statbuf->st_atime = __statbuf->st_ctime = __statbuf->st_mtime =
					(10 * 365 + 2) * 60 * 60 * 24 - _timezone ;
			__statbuf->st_mode = S_IFDIR ;
			rv = 0;
		}
		else
		{
			fd = open(path, O_RDONLY, 0);
			if (fd != -1)
			{
				if (__ll_isatty(__uihandles[fd]))
				{
					__statbuf->st_mode |= S_IFREG | S_IREAD | S_IWRITE;
					rv = 0;
				}
				close(fd);
			}
		}
	}
        if (rv == 0)
	{
		int drive;
		if (path[1] == ':')
		{
			drive = toupper(path[0]) - 'A';
		}
		else
		{
			_dos_getdrive(&drive);
			drive--;
		}
		__statbuf->st_ino = 0 ;
		__statbuf->st_uid = 0 ;
		__statbuf->st_nlink = 1;
		__statbuf->st_rdev = __statbuf->st_dev = drive;
	}
        if (rv == -1)
                errno = ENOENT;
	return rv;
}
int _RTL_FUNC stat(char *path, struct stat *__statbuf)
{
   return _stat(path, (struct _stat *)__statbuf) ;
}
int _RTL_FUNC _stat32(char *path, struct stat *__statbuf)
{
   return _stat(path, (struct _stat *)__statbuf) ;
}
