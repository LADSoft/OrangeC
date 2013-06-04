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
#include <string.h>
#include <ctype.h>
#include <string.h>
#include <io.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dir.h>
#include <wchar.h>
#include <locale.h>
#include <errno.h>
#include "libp.h"

extern int __uimodes[HANDLE_MAX],__uiflags[HANDLE_MAX], __uihandles[HANDLE_MAX] ;
extern char __uinames[HANDLE_MAX][256], __uidrives[HANDLE_MAX] ;

int __umask_perm = 0;

int _RTL_FUNC sopen  (const char *__path, int __access, int __shmode, ... /*unsigned mode*/)
{
   va_list ap ;
   int __amode,handle ;
   int i,h; 
   if (!__path) {
      errno = ENOENT;
      return -1;
   }
   __ll_enter_critical() ;
   h = __uinewhandpos() ;
   if (h == -1) {
      __ll_exit_critical() ;
      return h ;
   }
   __uiflags[h] = 0 ;
   if (__path[1] == ':')
      __uidrives[h] = toupper(__path[0]) - 'A' ;
   else
      __uidrives[h] = getdisk() ;
   memcpy(&__uinames[h][0],__path,strlen(__path)) ;
   if (!(__access & (O_BINARY | O_TEXT)))
      __access |= _fmode ;
   if (__ll_writeable(__path))
      __uiflags[h] |= UIF_WRITEABLE ;
   if (__access & O_CREAT) {
      if (__umask_perm & S_IWRITE) {
         errno = EACCES;
         __ll_exit_critical() ;
         return -1 ;
      }
      handle = __ll_open(__path,__ll_uioflags(__access),__ll_shflags(__shmode) ) ;
      if (__access & O_EXCL) {
         if (handle) {
			errno = EEXIST;
            __ll_close(handle) ;
            __ll_exit_critical() ;
            return -1 ;
         }
      }
      if (handle) {
         __uimodes[h] = __access ;
         __uihandles[h] = handle ;
         if (__access & O_TRUNC)
            chsize(h,0) ;
		 if (!__ll_isatty(handle))
	         __uiflags[h] |= UIF_EOF ;
      } else {
         handle = __ll_creat(__path,__ll_uioflags(__access),__ll_shflags(__shmode) ) ;
         if (handle == 0) {
            __ll_exit_critical() ;
            return -1 ;
         }
         __uimodes[h] = __access ;
         __uihandles[h] = handle ;
         if (__access & O_TRUNC)
            chsize(h,0) ;
         va_start(ap, __shmode) ;
         __amode = *(int *)ap ;
         if (!(__amode & S_IWRITE))
            __uiflags[h] |= UIF_RO ;
		if (!__ll_isatty(handle))
	         __uiflags[h] |= UIF_EOF ;
         va_end(ap) ;
      }
   } else {
      handle = __ll_open(__path,__ll_uioflags(__access),__ll_shflags(__shmode)) ;
      if (handle == 0) {
		  errno = ENOENT;
         __ll_exit_critical() ;
         return -1 ;
      }
      __uimodes[h] = __access ;
      __uihandles[h] = handle ;
      if (__access & O_TRUNC)
         chsize(h,0) ;
   }
   if (__uimodes[h] & O_APPEND) {
      if (lseek(h,0,SEEK_END) < 0) {
         close(h) ;
         __uihandles[h] = -1 ;
         __ll_exit_critical() ;
         return -1 ;
      }
	  if (!__ll_isatty(handle))
	      __uiflags[h] |= UIF_EOF ;
   }
   __ll_exit_critical() ;
   return h ;
}
int _RTL_FUNC _sopen  (const char *__path, int __access, int __shmode, ... /*unsigned mode*/)
{
   int mode ;
   va_list ap ;
   va_start(ap, __shmode) ;
   mode = *(int *)ap ;
   va_end(ap) ;
   return sopen(__path,__access, __shmode, mode) ;
}
