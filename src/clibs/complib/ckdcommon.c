/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include <float.h>
#include "_locale.h"
#include "libp.h"

void __stdcall ___ckd_set_value(unsigned int buf[CKD_BUF_SIZE/sizeof(int)], const void *val, int typeid_val)
{
    int sz;
    switch(typeid_val)
    {
        case -1:
        case 1:
            sz = sizeof(char);
            break;
        case -2:
        case 2:
            sz = sizeof(short);
            break;
        case -3:
        case 3:
            sz = sizeof(int);
            break;
        case -4:
        case 4:
            sz = sizeof(long);
            break;
        case -5:
        case 5:
            sz = sizeof(long long);
            break;
        default:
           sz = 1;
           break;
    }
    memcpy((void *)buf, val, sz);
    // assume unsigned
    int ch = 0;
    // this next line is little endian biased...
    if (typeid_val < 0 && ((char *)buf)[sz-1] & 0x80)
    {
        // signed with a sign bit
        ch = 0xff;
    }
    memset((void *)((char *)buf+sz), ch, CKD_BUF_SIZE-sz); 
}
int __stdcall ___ckd_get_value(const unsigned int buf[CKD_BUF_SIZE/sizeof(int)], void *result, int typeid_val)
{
    int sz;
    switch(typeid_val)
    {
        case -1:
        case 1:
            sz = sizeof(char);
            break;
        case -2:
        case 2:
            sz = sizeof(short);
            break;
        case -3:
        case 3:
            sz = sizeof(int);
            break;
        case -4:
        case 4:
            sz = sizeof(long);
            break;
        case -5:
        case 5:
            sz = sizeof(long long);
            break;
        default:
           sz = 1;
           break;
    }
    memcpy(result, (void *)buf, sz);
     // assume unsigned
    int ch = 0;
    // this next line is little endian biased...
    if (typeid_val < 0 && (((char *)buf)[sz-1] & 0x80))
    {
        // signed with a sign bit
        ch = 0xff;
    }
    for (int i=sz; i < CKD_BUF_SIZE; i++)
        if (((unsigned char*)buf)[i] != ch)
            return 1;
    return 0;
}
