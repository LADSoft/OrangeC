/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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
#include <io.h>
#include <dos.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
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