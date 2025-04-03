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

#include <malloc.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <string.h>
#include "libp.h"
#include "_locale.h"
#include "env.h"

int __Inflate(char* out, char* in);

void __ctype_init(void);
void __locale_reinit(void);
extern void* __locale_data[LC_LAST + 1];
extern LOCALE_HEADER _C_locale_data;

static char* names[] = {"LC_ALL", "LC_COLLATE", "LC_CTYPE", "LC_MONETARY", "LC_NUMERIC", "LC_TIME", "LC_MESSAGES", "LC_userdef"};
static void fixups(unsigned char* env)
{
    int len = *(int*)env, i, j;
    int ofs = (int)env;
    LOCALE_HEADER **p, *p1;
    COLLATE_DATA* p2;
    p = env + 4;
    for (i = 0; i < len; i++)
    {
        *(char**)p += ofs;
        (*p)->qual_name += ofs;
        (*p)->unqual_name += ofs;
        p1 = *p;
        for (j = 1; j <= LC_LAST; j++)
        {
            ((char**)p1)[j + 1] += ofs;
            p2 = ((COLLATE_DATA**)p1)[j + 1];
            p2->qualname += ofs;
        }
        p++;
    }
}
static unsigned char* loadenv(void)
{
    static unsigned char* __env;
    int size = *(int*)__envlocale;
    if (__env)
        return __env;
    __ll_enter_critical();
    __env = malloc(size + 32);
    if (!__env)
    {
        __ll_exit_critical();
        return 0;
    }
    if (!__Inflate(__env, __envlocale + 4))
    {
        __ll_exit_critical();
        return 0;
    }
    fixups(__env);
    __ll_exit_critical();
    return __env;
}
static LOCALE_HEADER* lookup(char* p, int __category)
{
    int size;
    unsigned char* data;
    LOCALE_HEADER** head;
    int i;
    if (!strcmp(p, "C"))
        return &_C_locale_data;
    if (!(data = loadenv()))
        return 0;
    size = *((int*)data)++;
    head = data;
    for (i = 0; i < size; i++, head++)
    {
        if (!strcmp((*head)->qual_name, p) || !strcmp((*head)->unqual_name, p))
            return *head;
    }
    return 0;
}
char* _RTL_FUNC _lsetlocale(int __category, const char* __locale)
{
    int start = 1, end = LC_LAST;
    int i;
    char* p;
    char buf2[64];
    LOCALE_HEADER* dat;
    char* descriptor = __getRtlData()->setlocaledescriptor;
    if (__category != LC_ALL)
        start = end = __category;
    if (__locale && __locale[0] == 0)
    {
        __locale = getenv(names[LC_ALL]);
        if (!__locale && __category != LC_ALL)
            __locale = getenv(names[__category]);
        if (!__locale)
            __locale = getenv("LANG");
        if (!__locale)
            return 0;
    }
    p = __locale;
    if (p)
    {
        if (__category == LC_ALL)
        {
            if (strchr(p, ';'))
            {
                int done = 0;
                for (i = 1; i <= LC_LAST && !done; i++)
                {
                    int n;
                    strncpy(buf2, p, 63);
                    buf2[63] = 0;
                    n = strcspn(buf2, ";");
                    if (buf2[n] == 0)
                        done = 1;
                    else
                        buf2[n] = 0;
                    if (n != 0 && !(dat = lookup(buf2, i)))
                        return 0;
                    p += n + 1;
                }
                done = 0;
                p = __locale;
                for (i = 1; i <= LC_LAST && !done; i++)
                {
                    int n;
                    strncpy(buf2, p, 63);
                    buf2[63] = 0;
                    n = strcspn(buf2, ";");
                    if (buf2[n] == 0)
                        done = 1;
                    else
                        buf2[n] = 0;
                    if (n != 0)
                    {
                        if (!(dat = lookup(buf2, i)))
                            return 0;
                        __locale_data[i] = ((void**)dat)[i + 1];
                    }
                    p += n + 1;
                }
            }
            else
            {
                if (!(dat = lookup(p, __category)))
                    return 0;
                for (i = 1; i <= LC_LAST; i++)
                {
                    __locale_data[i] = ((void**)dat)[i + 1];
                }
            }
        }
        else
        {
            if (strchr(p, ';'))
            {
                for (i = 1; *p && i < __category; i++)
                {
                    p += strcspn(p, ";");
                    if (*p)
                        p++;
                }
                if (!*p)
                    return 0;
                strncpy(buf2, p, 63);
                buf2[63] = 0;
                i = strcspn(buf2, ";");
                buf2[i] = 0;
                p = buf2;
            }
            if (!(dat = lookup(p, __category)))
                return 0;
            __locale_data[__category] = ((void**)dat)[__category + 1];
        }
        //        __locale_reinit();
        __ctype_init();
    }
    descriptor[0] = 0;
    for (i = start; i <= end; i++)
    {
        if (i != start)
            strcat(descriptor, ";");
        strcat(descriptor, ((COLLATE_DATA*)__locale_data[i])->qualname);
    }
    return descriptor;
}
