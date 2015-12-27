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
#include <wchar.h>

size_t wcsnrtombs (char *restrict dst, const wchar_t **restrict src, size_t nms, size_t len, mbstate_t *restrict p)
{
  wchar_t wc;
  size_t used = 0;
  const wchar_t *r = *src;
  (void)p;
  
  if (dst == NULL)
    len = (size_t)-1;

  while (used < len && nms > 0) {
      wc = *r++;
      nms--;
      if (wc == L'\0') {
    	  if (dst != NULL)
	        *dst = '\0';
	      *src = NULL;
    	  return used;
      } else if (wc < 0x80) {
    	  if (dst != NULL)
	        *dst++ = (char) wc;
	      used++;
      }
      else if ((unsigned)wc <= 0x7ff) {
          if (used + 2 <= len) {
              if (dst != NULL) {
                *dst++ = 0xc0 + (wc >> 6) ;
                *dst++ = (wc & 0x3f) | 0x80 ;
            }
            used+=2;
          } else {
            r-- ;
            break;
          }
      } else if ((unsigned)wc <= 0xffff) {
          if (used + 3 <= len) {
              if (dst != NULL) {
                *dst++ = 0xe0 + (wc >> 12) ;
                *dst++ = ((wc >> 6) & 0x3f) | 0x80 ;
                *dst++ = (wc & 0x3f) | 0x80 ;
            }
            used+=3;
          } else {
            r--;
            break;
          }
      } else {
          errno = EILSEQ;
          return -1;
      }
  }

  *src = r;

  return used;
}
