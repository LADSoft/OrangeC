/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
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
