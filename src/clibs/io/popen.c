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
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <sys\stat.h>
#include "libp.h"
#include <threads.h>

struct threads
{
    struct threads* next;
    char* cmd;
    thrd_t thread;
    FILE* handle;
    int pipeisstdin;
    int pipe;
};

static struct threads* threadList;
extern int __uimodes[HANDLE_MAX];
static int spawnThread(void* v)
{
    struct threads* t = (struct threads*)v;
    int rv = __ll_system(t->cmd, t->pipeisstdin ? t->pipe : fileno(stdin), t->pipeisstdin ? fileno(stdout) : t->pipe);
    return rv;
}
FILE* __basefopen(const wchar_t* restrict name, const wchar_t* restrict mode, FILE* restrict stream, int fd, int share);

FILE* _RTL_FUNC popen(const char* name, const char* restrict mode)
{
    int read;
    if (!strcmp(mode, "r"))
        read = 1;
    else if (!strcmp(mode, "w"))
        read = 0;
    else
    {
        errno = EINVAL;
        return NULL;
    }
    int pipeEnds[2];
    if (pipe(pipeEnds) != 0)
        return NULL;
    wchar_t buf[260], *p = buf, buf1[64], *q = buf1;
    while (*name)
        *p++ = *name++;
    *p = *name;
    while (*mode)
        *q++ = *mode++;
    *q = *mode;
    FILE* rv = __basefopen(buf, buf1, NULL, read ? pipeEnds[0] : pipeEnds[1], 0);

    struct threads* t = calloc(1, sizeof(struct threads));
    if (!t)
    {
        fclose(rv);
        close(read ? pipeEnds[1] : pipeEnds[0]);
        errno = ENOMEM;
        return -1;
    }
    t->handle = rv;
    t->pipeisstdin = !read;
    t->pipe = read ? pipeEnds[1] : pipeEnds[0];
    t->cmd = strdup(name);
    if (t->cmd && thrd_create(&t->thread, spawnThread, t) == thrd_success)
    {
        __ll_enter_critical();
        t->next = threadList;
        threadList = t;
        __ll_exit_critical();
        return rv;
    }
    fclose(rv);
    close(read ? pipeEnds[1] : pipeEnds[0]);
    free(t->cmd);
    errno = ENOMEM;
    return -1;
}
FILE* _RTL_FUNC _popen(const char* name, const char* restrict mode) { return popen(name, mode); }
int _RTL_FUNC pclose(FILE* fil)
{
    __ll_enter_critical();
    struct threads** t = &threadList;
    while (*t)
    {
        if ((*t)->handle == fil)
            break;
        t = &(*t)->next;
    }
    if (!*t)
    {
        errno = ENOENT;

        __ll_exit_critical();
        return -1;
    }
    struct threads* x = *t;
    *t = (*t)->next;
    __ll_exit_critical();
    fclose(x->handle);
    close(x->pipe);
    free(x->cmd);
    int result = -1;
    int rv = thrd_join(x->thread, &result);
    free(x);
    if (rv == thrd_success)
        return result;
    return -1;
}
int _RTL_FUNC _pclose(FILE* fil) { return pclose(fil); }
