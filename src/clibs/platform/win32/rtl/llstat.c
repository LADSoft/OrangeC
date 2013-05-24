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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <limits.h>
#include <io.h>
#include "libp.h"

time_t __to_timet(FILETIME *time)
{
   TIME_ZONE_INFORMATION tzinfo ;
   int bias ;
   SYSTEMTIME stime ;
   struct tm tmx ;
   time_t tempTime;
   int temp = GetTimeZoneInformation(&tzinfo) ;
   if (temp != TIME_ZONE_ID_INVALID)
    bias = tzinfo.Bias ;
   else
    bias = 0 ;
   if (temp == TIME_ZONE_ID_DAYLIGHT)
     bias +=tzinfo.DaylightBias ;
   else
     bias +=tzinfo.StandardBias ;
   FileTimeToSystemTime(time,&stime) ;
   tmx.tm_hour = stime.wHour ;
   tmx.tm_min = stime.wMinute ;
   tmx.tm_sec = stime.wSecond ;
   tmx.tm_mon = stime.wMonth-1 ;
   tmx.tm_mday = stime.wDay ;
   tmx.tm_year = stime.wYear - 1900 ;
   tempTime =  mktime(&tmx) ;
   if (tempTime == (time_t)-1)
       return tempTime ;
   return tempTime - bias * 60;
}
int __ll_stat(int handle, void *__statbuf)
{
   struct _stat *sb = __statbuf ;
   BY_HANDLE_FILE_INFORMATION info ;
   FILETIME timex ;

   if (sb->st_mode & S_IFCHR)
      return 0 ;

   if (!GetFileInformationByHandle((HANDLE)handle,&info)) {
      errno = EBADF ;
      return -1 ;
   }
   if (sb->st_mode & S_IFREG) {
      sb->st_atime = __to_timet(&info.ftLastAccessTime) ;
      sb->st_mtime = __to_timet(&info.ftLastWriteTime) ;
      sb->st_ctime = __to_timet(&info.ftCreationTime) ;
   } 
    return 0;
}
int __ll_namedstat(const char *file, void *__statbuf)
{
    struct stat *sb = __statbuf;
    WIN32_FIND_DATA finddata;
    HANDLE handle;
    if ((handle = FindFirstFile(file, &finddata)) != INVALID_HANDLE_VALUE)
    {
      FindClose(handle);
      sb->st_atime = __to_timet(&finddata.ftLastAccessTime) ;
      sb->st_mtime = __to_timet(&finddata.ftLastWriteTime) ;
      sb->st_ctime = __to_timet(&finddata.ftCreationTime) ;

      if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        sb->st_mode = S_IFDIR;
      else 
          sb->st_mode = S_IFREG;
      sb->st_mode |= S_IREAD;
      if (!(finddata.dwFileAttributes & FILE_ATTRIBUTE_READONLY))
          sb->st_mode |= S_IWRITE;
      if (!finddata.nFileSizeHigh)
      {
          sb->st_size = finddata.nFileSizeLow;
          return 0;
      }
    }
    return -1;	
}
