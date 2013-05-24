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
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <errno.h>
#include "libp.h"

/*
 * Functions for searching for files. _findfirst returns -1 if no match
 * is found. Otherwise it returns a handle to be used in _findnext and
 * _findclose calls. _findnext also returns -1 if no match could be found,
 * and 0 if a match was found. Call _findclose when you are finished.
 */
static struct fdl {
   struct fdl *next ;
   int handle ;
   int done ;
   struct find_t data ;
   char name[FILENAME_MAX] ;
} *list ; 

void filldata(struct fdl *fh, struct _finddata_t *data)
{
   struct tm tmx ;
   char *s ;
   data->attrib = fh->data.attrib ;
   memcpy(&data->size, fh->data.size,sizeof(long)) ;
   tmx.tm_hour = fh->data.wr_time >> 11 ;
   tmx.tm_min =  (fh->data.wr_time >> 5) & 63 ;
   tmx.tm_sec =  (fh->data.wr_time & 31) * 2 ;
   tmx.tm_mon = ((fh->data.wr_date >> 5 ) & 15) - 1 ;
   tmx.tm_mday =   (fh->data.wr_date & 31) ;
   tmx.tm_year = (fh->data.wr_date >> 9) + 1980 - 1900 ;
   data->time_create = mktime(&tmx) ;
   data->time_write = data->time_create ;
   tmx.tm_hour = 0 ;
   tmx.tm_min = 0;
   tmx.tm_sec = 0 ;
   data->time_access = mktime(&tmx) ;
   strcpy(data->name,fh->name) ;
   s = strrchr(data->name,'\\') ;
   if (!s) {
      s = strrchr(data->name,'/') ;
      if (!s)
         s = data->name ;
      else
         s++ ;
   } else
      s++ ;
   strcpy(s,fh->data.name) ;
}
long _RTL_FUNC _findfirst (const char* szFilespec, struct _finddata_t* find)
{
   struct fdl *x, *nw ;
   int hand = 1 ;
   if (!find)
   {
       errno=EINVAL;
       return 0;
   }
   __ll_enter_critical() ;
   x = list ;
   while (x) {
      if (x->handle >= hand)
         hand = x->handle + 1 ;
      x = x->next ;
   }
   nw = calloc(sizeof(struct fdl),1) ;
   if (!nw) {
      __ll_exit_critical() ;
      errno = ENOMEM;
      return 0 ;
   }
   strcpy(nw->name,szFilespec) ;
   nw->handle = hand;
   if (_dos_findfirst(szFilespec,0, &nw->data)) {
      free(nw) ;
      __ll_exit_critical() ;
      errno = ENOENT;
      return 0 ;
   }
   nw->next = list ;
   list = nw ;
   filldata(nw,find) ;
   __ll_exit_critical() ;
   return nw->handle ;
}
int _RTL_FUNC _findnext (long nHandle, struct _finddata_t* find)
{
   struct fdl *x ;
   if (!find)
   {
       errno = EINVAL;
    return -1;
   }
   __ll_enter_critical() ;
   x = list ;
   while (x) {
      if (x->handle == nHandle)
         break ;
      x = x->next ;
   }
   if (!x || x->done) {
      __ll_exit_critical() ;
      errno = EINVAL;
      return -1 ;
   }
   if (_dos_findnext(&x->data)) {
      __ll_exit_critical() ;
      x->done = 1 ;
      errno = ENOENT;
      return -1 ;
   }
   filldata(x,find) ;
   __ll_exit_critical() ;
   return 0 ;
}
int _RTL_FUNC _findclose (long nHandle)
{
   
   struct fdl *x, **x1 = &list ;
   __ll_enter_critical() ;
   x = list ;
   while (x) {
      if (x->handle == nHandle)
         break ;
      x1 = &x->next ;
      x = x->next ;
   }
   if (!x) {
      __ll_exit_critical() ;
      errno = EINVAL;
      return -1 ;
   }
   (*x1) = (*x1)->next ;
   __ll_findclose(&x->data) ;
   free(x) ;
   __ll_exit_critical() ;
   return 0 ;
}