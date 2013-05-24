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
#include <time.h>
#include <utime.h>
#include <io.h>
#include <fcntl.h>
#include <locale.h>
#include <wchar.h>
#include "libp.h"

int _RTL_FUNC _futime(int __handle, struct utimbuf *times)
{
  int rv;
  struct tm *p, stm, sta;

  if (!times) {
    time_t now = time (NULL);
    stm = sta = *localtime (&now);
  }
  else {
    p = localtime (&times->modtime);
    if (!p) return -1;
    stm = *p;
    p = localtime (&times->actime);
    if (!p) return -1;
    sta = *p;
  }

   __ll_enter_critical() ;
   __handle = __uiohandle(__handle) ;
   if (__handle == -1)   {
      __ll_exit_critical() ;
      return -1 ;
   }
   rv = __ll_utime(__handle, &sta, &stm);
   __ll_exit_critical() ;
   return rv ;

}
int _RTL_FUNC _utime(const char *path, struct _utimbuf *times)
{
  int handle;
  int rv;
  
  if ((handle = open (path, O_RDWR)) == -1)
    return -1;

  rv = _futime(handle, times);

  close (handle);
  return rv;
}
int _RTL_FUNC utime (const char * path, struct utimbuf * times)
{
    return _utime(path, times);
}
