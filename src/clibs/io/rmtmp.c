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
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include <string.h>
#include "io.h"
#include "libp.h"

extern int _abterm;
extern int __maxfiles;
extern FILE* _pstreams[_NFILE_];

static struct _rmvlist
{
    struct _rmvlist* next;
    char* name;
} * remove_list;
/* must happen before fcloseall */
#pragma rundown __removealltemps 31

int __insertTempName(char* name)
{
    struct _rmvlist* rmv;
    char* aa = strdup(name);
    if (!aa)
        return 1;
    rmv = malloc(sizeof(struct _rmvlist));
    if (!rmv)
    {
        free(aa);
        return 1;
    }
    __ll_enter_critical();
    rmv->next = remove_list;
    rmv->name = aa;
    remove_list = rmv;
    __ll_exit_critical();
    return 0;
}
static int __removeTempFiles(void)
{
    int count = 0;
    while (remove_list)
    {
        struct _rmvlist* next = remove_list->next;
        unlink(remove_list->name);
        free(remove_list->name);
        free(remove_list);
        remove_list = next;
        count++;
    }
    return count;
}
static void __closeTempFiles(void)
{
    struct _rmvlist* rmv = remove_list;
    while (rmv)
    {
        wchar_t buf[256];
        wchar_t *p = buf;
        char *q = rmv->name;
        while (*q)
            *p++ = *q++;
        *p = q;
        int i;
        for (i = __maxfiles - 1; i >= 3; i--)
        {
            if (!strcmp(_pstreams[i]->extended->name, buf))
            {
                fclose(_pstreams[i]);
                break;
            }
        }
        rmv = rmv->next;
    }
}
int _RTL_FUNC rmtmp(void)
{
    __closeTempFiles();
    return __removeTempFiles();
}
int _RTL_FUNC _rmtmp(void) { return rmtmp(); }
void __removealltemps(void)
{
    if (!_abterm)
    {
        __removeTempFiles();
    }
}
