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
//#include <dos.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

#define FILTER_BUF_LEN _DTA_BUF_DEFAULT
extern int __uiflags[HANDLE_MAX],__uimodes[HANDLE_MAX] ;

int _RTL_FUNC write(int __handle, void *__buf, unsigned __len)
{
   char *pos=__buf ;
   char obuf[FILTER_BUF_LEN] ;
   int olen=0 ;
   int lenleft = __len,writelen, ohand = __handle ;
   int i ;
   int totalwritten = 0 ;
   __ll_enter_critical() ;
   __handle = __uiohandle(__handle) ;
   if (__handle == -1) {
      __ll_exit_critical() ;
      return -1 ;
   }
   if (__uimodes[ohand] & O_RDONLY) {
      errno = EBADF ;
      __ll_exit_critical() ;
      return -1 ;
   }
   if (__len == 0) {
      __ll_exit_critical() ;
      return 0 ;
   }
   if (__uimodes[ohand] & O_APPEND) {
      if (lseek(ohand,0,SEEK_END) < 0) {
         __ll_exit_critical() ;
         return -1 ;
      }
	  if (!__ll_isatty(__handle))
	      __uiflags[ohand] |= UIF_EOF ;
   }
   if (__uimodes[ohand] & O_BINARY) {
      while (lenleft >= FILTER_BUF_LEN) {
         for (i=0; i< 3; i++) {
            writelen = __ll_write(__handle,pos,FILTER_BUF_LEN) ;
            if (writelen) {
               if  (writelen != FILTER_BUF_LEN) {
                  __ll_exit_critical() ;
                  return writelen + totalwritten ;
               } else {
                  totalwritten += FILTER_BUF_LEN ;
                  lenleft -= FILTER_BUF_LEN ;
                  pos += FILTER_BUF_LEN ;
                  break ;
               }
            }
//            sleep(1) ;
         }
         if (i >= 3) {
            __ll_exit_critical() ;
            return totalwritten ;
         } 
      }
      if (lenleft) {
         for (i=0; i< 3; i++) {
            writelen = __ll_write(__handle,pos,lenleft) ;
            if (writelen)
			{
				__ll_exit_critical();
               return writelen + totalwritten ;
			}
//            sleep(1) ;
         }
      }
      __ll_exit_critical() ;
      return totalwritten ;
   }
   for (i= 0 ; i < __len; i++) {
      if (*pos == '\n') {
         obuf[olen++]= '\r' ;
         totalwritten-- ;
         if (i < __len-1 && *(pos+1) == 0) {  /* wchar? */
           obuf[olen++] = 0;
           totalwritten--;
         }
      }
      obuf[olen++]= *pos++ ;
      if (olen >= FILTER_BUF_LEN-1) {
         int j ;
         for (j=0; j< 3; j++) {
            writelen = __ll_write(__handle,obuf,olen) ;
            if (writelen) {
               if (writelen != olen)
                  return totalwritten + writelen ;
               totalwritten += writelen ;
               break ;
            }
//            sleep(1) ;
         }
         if (j >= 3) {
            __ll_exit_critical() ;
            return totalwritten ;
         }
         olen = 0 ;
      }
   }
   if (olen) {
         int j ;
         for (j=0; j< 3; j++) {
            writelen = __ll_write(__handle,obuf,olen) ;
            if (writelen) {
               __ll_exit_critical() ;
               return totalwritten + writelen ;
            }
//            sleep(1) ;
         }
   }
   __ll_exit_critical() ;
   return totalwritten ;
}
int _RTL_FUNC _write(int __handle, void *__buf, unsigned __len)
{
	return write(__handle, __buf, __len);
}
