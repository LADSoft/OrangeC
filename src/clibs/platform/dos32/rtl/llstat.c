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
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <time.h>
#include <dpmi.h>
#include <wchar.h>
#include <locale.h>
#include <dos.h>
#include "libp.h"

int __ll_stat(int handle, void *__statbuf)
{
   struct _stat *sb = __statbuf ;
   if (sb->st_mode & S_IFREG) {
      struct ftime ft ;
      int timex ;
      struct tm tmx ;
      if (__ll_getftime(handle,&ft) == -1) {
         errno = EBADF ;
         return -1 ;
      }
      tmx.tm_year = ft.ft_year +1980 - 1900;
      tmx.tm_mon = ft.ft_month-1 ;
      tmx.tm_mday = ft.ft_day ;
      tmx.tm_hour = ft.ft_hour ;
      tmx.tm_min = ft.ft_min ;
      tmx.tm_sec = 2 * ft.ft_tsec ;
      timex = mktime(&tmx) ;
      sb->st_atime = sb->st_mtime = sb->st_ctime = timex ;
   } 
   return 0;
}
int __ll_namedstat(const char *file, void *__statbuf)
{
    struct stat *sb = __statbuf;
    struct find_t finddata;
    if (!_dos_findfirst(file, 0x17, &finddata))
    {
      struct ftime *ft = &finddata.wr_time;
      int timex;
    struct tm tmx;
      tmx.tm_year = ft->ft_year +1980 - 1900;
      tmx.tm_mon = ft->ft_month-1 ;
      tmx.tm_mday = ft->ft_day ;
      tmx.tm_hour = ft->ft_hour ;
      tmx.tm_min = ft->ft_min ;
      tmx.tm_sec = 2 * ft->ft_tsec ;
      timex = mktime(&tmx) ;
      sb->st_atime = sb->st_mtime = sb->st_ctime = timex ;
      if (finddata.attrib & FA_DIREC)
        sb->st_mode = S_IFDIR;
      else 
          sb->st_mode = S_IFREG;
      sb->st_mode |= S_IREAD;
      if (!(finddata.attrib & FA_RDONLY))
          sb->st_mode |= S_IWRITE;
      sb->st_size = *(int *)(&finddata.size);
      return 0;
    }
    return -1;	
}
