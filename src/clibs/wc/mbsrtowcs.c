/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *     
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
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

size_t mbsrtowcs (wchar_t *restrict dst, const char **restrict src, size_t len, mbstate_t *restrict p)
{
  unsigned char b;
  size_t used = 0;
  const char *r = *src;

  if (!p)
    p = &__getRtlData()->mbsrtowcs_st;

  if (dst == NULL)
    len = (size_t)-1;

  while (used < len) {
    b = (unsigned char)*r++;
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
