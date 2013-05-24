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
#include <dos.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

extern int __uiflags[HANDLE_MAX],__uimodes[HANDLE_MAX] ;

int _RTL_FUNC read(int __handle, void *__buf, unsigned __len)
{
   char *pos=__buf ;
   char *dest = __buf ;
   int readlen ;
   int ohand = __handle ;
   __ll_enter_critical() ;
   __handle = __uiohandle(__handle) ;
   if (__handle == -1) {
      __ll_exit_critical() ;
      return -1 ;
   }
   if (__uimodes[ohand] & O_WRONLY) {
      __ll_exit_critical() ;
      errno = EBADF ;
      return -1 ;
   }
   if (__uiflags [ohand] & UIF_EOF)
   {
      __ll_exit_critical();
      return 0 ;
   }
   if (__uimodes[ohand] & O_BINARY) {
      int i ;
      for (i=0 ; i < 3; i++) {
         readlen = __ll_read(__handle,dest,__len) ;
         if (readlen >= 0)
            break ;
         sleep(1) ;
      }
      if (readlen == -1) {
         __ll_exit_critical() ;
         return -1 ;
      }
      if (readlen < __len && !__ll_isatty(__handle)) 
         __uiflags[ohand] |= UIF_EOF ;
      __ll_exit_critical() ;
      return readlen ;
         
   } else {
      int i ;
      do
      {
      for (i=0 ; i < 3; i++) {
         readlen = __ll_read(__handle,dest,__len) ;
         if (readlen >= 0)
            break ;
         sleep(1) ;
      }
      if (readlen == -1) {
         __ll_exit_critical() ;
         return -1 ;
      }
      if (readlen < __len && !__ll_isatty(__handle))
         __uiflags[ohand] |= UIF_EOF ;
      pos = __buf;
      for (i=0; i < readlen; i++) {
         switch (*pos) {
            case '\x1a':
        if (!__ll_isatty(__handle))
                __uiflags[ohand] |= UIF_EOF ;
                __ll_exit_critical() ;
                return dest - __buf ;
            case '\r':
                  pos++ ;
                break ;
            default:
                *dest++ = *pos++ ;
                break ;
         }
      }
      } while (dest - __buf == 0);
      __ll_exit_critical() ;
      return dest - __buf ;
   }
}
int _RTL_FUNC _read(int __handle, void *__buf, unsigned __len)
{
    return read(__handle, __buf, __len);
}
