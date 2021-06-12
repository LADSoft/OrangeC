/* Software License Agreement
 * 
 *     Copyright(C) 1994-2021 David Lindauer, (LADSoft)
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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

char* _RTL_FUNC ecvt(double val, int len, int* decimal, int* sign)
{
    char* ecvtbuf = __getRtlData()->ecvt_buf;
    char *p = ecvtbuf, *q;
    int i;
    if (!len || !decimal || !sign)
    {
        errno = EINVAL;
        return NULL;
    }
    memset(ecvtbuf, 0, 100);
    if (len > 60)
        len = 60;
    sprintf(ecvtbuf, "%.*e", len - 1, val);
    if (*p == '-')
    {
        *sign = 1;
        p++;
    }
    else
        *sign = 0;
    *decimal = 0;
    for (i = 0; i < 100; i++)
    {
        if (p[i] == '.')
        {
            *decimal = i;
            strcpy(p + i, p + i + 1);
            break;
        }
    }
    q = strchr(ecvtbuf, 'e');
    if (q)
    {
        *decimal += atoi(q + 1);
        *q = 0;
    }
    return p;
}
char* _RTL_FUNC fcvt(double val, int len, int* decimal, int* sign)
{
    char* ecvtbuf = __getRtlData()->ecvt_buf;
    char *p = ecvtbuf, *q;
    int i;
    if (!len || !decimal || !sign)
    {
        errno = EINVAL;
        return NULL;
    }
    memset(ecvtbuf, 0, 100);
    if (len > 60)
        len = 60;
    if ((int)val)
        sprintf(ecvtbuf, "%.*f", len, val);
    else
        sprintf(ecvtbuf, "%.*e", len - 1, val);
    if (*p == '-')
    {
        *sign = 1;
        p++;
    }
    else
        *sign = 0;
    *decimal = 0;
    for (i = 0; i < 100; i++)
    {
        if (p[i] == '.')
        {
            *decimal = i;
            strcpy(p + i, p + i + 1);
            break;
        }
    }
    q = strchr(ecvtbuf, 'e');
    if (q)
    {
        *decimal += atoi(q + 1);
        *q = 0;
    }
    return p;
}
char* _RTL_FUNC gcvt(double val, int len, char* result)
{
    if (!len || !result)
    {
        errno = EINVAL;
        return NULL;
    }
    sprintf(result, "%.*g", len, val);
    return result;
}

char* _RTL_FUNC _ecvt(double val, int len, int* decimal, int* sign) { return ecvt(val, len, decimal, sign); }
char* _RTL_FUNC _fcvt(double val, int len, int* decimal, int* sign) { return fcvt(val, len, decimal, sign); }
char* _RTL_FUNC _gcvt(double val, int len, char* result) { return gcvt(val, len, result); }
