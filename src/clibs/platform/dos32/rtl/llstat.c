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

int __ll_stat(int handle, struct _stat64 *sb)
{
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
int __ll_namedstat(const wchar_t *file, struct _stat64 *sb)
{
    char buf[260], *p = buf;
    while (*file)
        *p++ = *file++;
    *p = *file;
    struct find_t finddata;
    if (!_dos_findfirst(buf, 0x17, &finddata))
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
