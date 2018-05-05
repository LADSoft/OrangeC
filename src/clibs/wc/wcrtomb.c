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
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

size_t wcrtomb (char *restrict s, wchar_t wc, mbstate_t *restrict ps)
{
  char xx[1];

  if (s == NULL) {
        return wcrtomb(xx,L'\0',ps);
  }
  if (ps == NULL)
    ps = &__getRtlData()->wcrtomb_st;

  if (wc < 0x80)
    {
      if (s != NULL)
    	*s = (char) wc;
      if (wc == 0)
        return 0;
      return 1;
    }

  if ((unsigned) wc <= 0x7ff) {
    if (s != NULL) {
        *s++ = 0xc0 + (wc >> 6) ;
        *s++ = (wc & 0x3f) | 0x80 ;
    }
    return 2;
  }
  if ((unsigned) wc <= 0xffff) {
    if (s != NULL) {
        *s++ = 0xe0 + (wc >> 12) ;
        *s++ = ((wc >> 6) & 0x3f) | 0x80 ;
        *s++ = (wc & 0x3f) | 0x80 ;
      
    }
    return 3;
  }
  errno = EILSEQ;
  return -1;
}
