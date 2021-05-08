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

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <dir.h>
#include "io.h"
#include "libp.h"

char* _RTL_FUNC tmpnam(char* buf)
{
    int n = 0;
    if (!buf)
        buf = &__getRtlData()->tmpfilnam;
    __ll_enter_critical();
    do
    {
        sprintf(buf, "tmp%05d.$$$", n++);
    } while (!access(buf, 0) && n <= TMP_MAX);
    __ll_exit_critical();
    if (n <= TMP_MAX)
        return buf;
    return NULL;
}
char* _RTL_FUNC tempnam(char* dir, char* prefix)
{
    int n = 0;
    char buf[MAXPATH], *p;
    char* s = getenv("TMP");
    struct ffblk ff;
    if (!s || findfirst(s, &ff, 0xff))
    {
        s = dir;
        if (!s || findfirst(s, &ff, 0xff))
        {
            s = buf;
            buf[0] = 0;
        }
    }
    strcpy(buf, s);
    s = buf;
    p = s + strlen(s);
    if (p != s && p[-1] != '\\')
        *p++ = '\\';
    __ll_enter_critical();
    do
    {
        sprintf(p, "%s%05d", prefix, n++);
    } while (!access(buf, 0) && n <= TMP_MAX);
    __ll_exit_critical();
    if (n <= TMP_MAX)
        return strdup(buf);
    return NULL;
}
char* _RTL_FUNC _tempnam(char* dir, char* prefix) { return tempnam(dir, prefix); }
FILE* _RTL_FUNC tmpfile(void)
{
    char* aa;
    FILE* rv;
    int len;

    aa = tempnam(NULL, "tmf");
    if (__insertTempName(aa))
    {
        free(aa);
        return 0;
    }
    rv = fopen(aa, "wb+");
    free(aa);
    return rv;
}
