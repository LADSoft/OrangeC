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

#include <locale.h>
#include <_locale.h>
#include <string.h>

size_t strxfrm(char* out, const char* restrict in, size_t buflen)
{
    COLLATE_DATA* cd = __locale_data[LC_COLLATE];
    struct collstr colldat;
    colldat.levels = cd->data[2];
    colldat.lvlflags = &cd->data[4];
    colldat.tbl = &cd->data[15];
    colldat.dest = out;
    for (colldat.curlevel = 0; colldat.curlevel < colldat.levels; colldat.curlevel++)
    {
        int ch;
        unsigned char* current;
        colldat.stateflags = 0;
        colldat.source = in;
        colldat.buflen = buflen;
        colldat.fetched = 0;
        if (colldat.lvlflags[colldat.curlevel] & CONTROL)
        {
            *colldat.dest++ = 1;
        }
        current = colldat.dest;
        while (1)
        {
            ch = __nextWeight(&colldat);
            if (!(colldat.stateflags & EOS))
            {
                if (colldat.lvlflags[colldat.curlevel] & CONTROL)
                {
                    if (colldat.dest > buflen - 2 + out)
                    {
                        *colldat.dest = 0;
                        return buflen;
                    }
                    *(((short*)colldat.dest)++) = 0x101 + colldat.fetched;
                    if (colldat.dest == buflen + out)
                        return buflen;
                }
                *(colldat.dest)++ = ch + 1;
                if (colldat.dest == buflen + out)
                    return buflen;
            }
            else
            {
                if (colldat.lvlflags[colldat.curlevel] & CONTROL)
                {
                    if (colldat.dest == current)
                    {
                        colldat.dest--;
                        current--;
                    }
                }
                else
                {
                    int len = colldat.dest - current;
                    if (len)
                    {
                        int cmp = colldat.tbl[colldat.levels * 0x101 + 1];
                        while (len && colldat.dest[-1] == cmp)
                            len--, colldat.dest--;
                    }
                }
                if (colldat.lvlflags[colldat.curlevel] & REVERSE)
                {
                    int len = colldat.dest - current;
                    if (!(colldat.lvlflags[colldat.curlevel] & CONTROL))
                    {
                        if (len != 0)
                        {
                            if (len < 3)
                            {
                                int temp = colldat.dest[-1];
                                colldat.dest[-1] = *current;
                                *current = temp;
                            }
                            else
                            {
                                int i;
                                char *p1 = current, *p2 = colldat.dest - 1;
                                len &= ~3;
                                len /= 2;
                                for (i = 0; i < len; i++)
                                {
                                    int temp = *p1;
                                    *p1 = p2[0];
                                    p2[0] = temp;
                                    p1++, p2--;
                                }
                            }
                        }
                    }
                    else
                    {
                        len >>= 2;
                        if (len >= 2)
                        {
                            char *p1 = current, *p2 = colldat.dest;
                            len /= 2;
                            while (len--)
                            {
                                int temp = *(int*)p1;
                                *(short*)p1 = *(short*)(p2 - 3);
                                p1[2] = p2[-1];
                                ((short*)p2)[-3] = temp & 0xffff;
                                p2[-1] = temp >> 16;
                                p1 += 3;
                                p2 -= 3;
                            }
                        }
                    }
                }
                break;
            }
        }
    }
    *colldat.dest = 0;
    return colldat.dest - out;
}
size_t _lstrxfrm(char* out, const char* restrict in, size_t buflen) { return strxfrm(out, in, buflen); }
