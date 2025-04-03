/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <dir.h>
#include "io.h"
#include "libp.h"

void __cpp_tmpnam(char* buf);

char* _RTL_FUNC tmpnam(char* buf)
{
    if (!buf)
        buf = &__getRtlData()->tmpfilnam;
    int n = 0;
    do
    {
#ifdef __MSIL__
        sprintf(buf, "tmp%05d.$$$", n);
#else
	__cpp_tmpnam(buf);
#endif
        n++;
    } while (!access(buf, 0) && n <= TMP_MAX);
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
    do
    {
#ifdef __MSIL__
        sprintf(p, "%s%05d", prefix, n);
#else
     strcpy(p, prefix);
	__cpp_tmpnam(p+strlen(p));
#endif
         n++;
    } while (!access(buf, 0) && n <= TMP_MAX);
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
