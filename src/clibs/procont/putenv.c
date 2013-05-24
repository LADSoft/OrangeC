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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"


extern char _RTL_DATA **_environ;
int _RTL_FUNC putenv(const char *name)
{
   char **q = _environ, **p ;
   int len = 0, has = 0,count = 0 ;
   __ll_enter_critical() ;
   if (!strchr(name,'=')) {
      __ll_exit_critical() ;
      return -1 ;
   }
   while (name[len] && name[len] != '=') {
      if ((*q)[len] == '=') {
         if ((*q)[len] != 0)
            has = 1 ;
      }   
      len++ ;
   }
    while (*q) {
      count++ ;
      if (!strnicmp(name,*q,len)) {
         if (has) {
            *q = name ;
            __ll_exit_critical() ;
            return 0 ;
         } else {
            while (*q) {
               *q = *(q+1) ;
               q++ ;
            }
            __ll_exit_critical() ;
            return 0 ;
         }
      }
        q++;
    }
   p = (char **)realloc(_environ,(count+2)*sizeof(char **)) ;
   if (!p) {
      __ll_exit_critical() ;
      return -1 ;
   }
   p[count-1] = NULL ;
   p[count-2] = name ;
   _environ = p ;
   __ll_exit_critical() ;
    return 0;
}
int _RTL_FUNC _putenv(const char *name)
{
    return putenv(name);
}