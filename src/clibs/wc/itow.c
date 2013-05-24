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
#include <wchar.h>

wchar_t * _RTL_FUNC ltow(long __value, wchar_t *__string, int __radix)
{
   char buf2[36] ;
   int len = 0, pos = 0 ;
   if (__value < 0) {
      __string[pos++] = '-' ;
      __value = - __value ;
   }
   if (__radix < 2 || __radix > 36)
      __string[pos] = 0 ;
   else if (!__value) {
         __string[pos++] = '0' ;
         __string[pos] = 0 ;
   } else {
      while (__value) {
         buf2[len++] = __value % __radix ;
         __value /= __radix ;
      }
      while (len) {
         int ch = buf2[--len] ;
         ch+= '0' ;
         if (ch > '9')
            ch += 7 ;
         __string[pos++] = ch ;
      }
      __string[pos] = 0 ;
   }
   return __string ;
}
wchar_t * _RTL_FUNC itow(int __value, wchar_t *__string, int __radix)
{
    return ltow(__value,__string,__radix);
}
