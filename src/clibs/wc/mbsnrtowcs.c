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
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

size_t mbsnrtowcs (wchar_t *restrict dst, const char **restrict src, size_t nms, size_t len, mbstate_t *restrict p)
{
  unsigned char b;
  size_t used = 0;
  const char *r = *src;

  if (!p)
    p = &__getRtlData()->mbsrtowcs_st;

  if (dst == NULL)
    len = (size_t)-1;

  while (used < len && p->left <= nms) {
    b = (unsigned char)*r++;
    nms--;
    if (p->left) {
        if ((b & 0xc0) != 0x80) {
            errno = EILSEQ;
            return (size_t)-1;
        }
        p->value <<=6;
        p->value |= b & 0x3f;
        if (!--p->left) {
            if (dst)
                *dst++ = p->value;
            if (p->value == L'\0') {
                *src = NULL;
                return used;
            }
            used++;
        }
    } else {
        if (b < 0x80) {
	    if (!nms)
            {
                *src = r-1;
                return used;
            }
            if (dst)
                *dst++ = (wchar_t)b;
            if (b == L'\0') {
                *src = NULL;
                return used;
            }
            used++;
        } else {
            if ((b & 0xc0) == 0x80 || b == 0xfe || b == 0xff) {
                errno = EILSEQ ;
                return (size_t) -1;
            }
            b <<= 1;
            while (b & 0x80) {
                p->left++ ;
                b <<= 1;
            }
            p->value = b >> (p->left + 1);
        }
    }
  }

  *src = r;

  return used;
}
