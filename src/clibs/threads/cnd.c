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
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <wchar.h>
#include <locale.h>
#include <threads.h>
#include <sys\timeb.h>
#include <stdlib.h>
#include "libp.h"

extern int __mtxThisThread(mtx_t* mtx);
extern int __thrd_rel_delay(const struct timespec* xt);

typedef struct _tlist
{
    struct _tlist* next;
    thrd_t thrd;
} tlist;

#define CND_SIG 0x444e435e  // "_CND"
typedef struct
{
    int sig;
    tlist* waiting;
    tlist* tail;
    long long handle;
} icnd;

struct clist
{
    struct clist* next;
    cnd_t cnd;
} * conds;
static void remove_thrd(cnd_t cnd, thrd_t thrd)
{
    tlist** cur = &((icnd*)cnd)->waiting;
    while (*cur)
    {
        if (thrd == (*cur)->thrd)
        {
            tlist* toremove = *cur;
            *cur = (*cur)->next;
            free(toremove);
        }
        else
        {
            cur = &(*cur)->next;
        }
    }
}
void __cnd_remove_thrd(thrd_t thrd)
{
    struct clist* cur = conds;
    while (cur)
    {
        remove_thrd(cur->cnd, thrd);
        cur = cur->next;
    }
}
void _RTL_FUNC cnd_destroy(cnd_t* cnd)
{
    icnd* p = (icnd*)*cnd;
    __ll_enter_critical();
    if (p->sig == CND_SIG)
    {
        struct clist** ccur = &conds;
        struct clist* tofree = NULL;
        while (*ccur)
        {
            if ((*ccur)->cnd == cnd)
            {
                tofree = *ccur;
                *ccur = (*ccur)->next;
                break;
            }
            ccur = &(*ccur)->next;
        }
        if (tofree)
            free(tofree);
        tlist* cur = p->waiting;
        while (cur)
        {
            tlist* next = cur->next;
            free(cur);
            cur = next;
        }
        __ll_cndFree(p->handle);
        p->sig = 0;

        free(p);
    }
    __ll_exit_critical();
}
int _RTL_FUNC cnd_init(cnd_t* cnd)
{
    icnd* p = calloc(1, sizeof(icnd));
    struct clist* x = calloc(1, sizeof(struct clist));
    if (p && x)
    {
        if ((p->handle = __ll_cndAlloc()) != 0)
        {
            p->sig = CND_SIG;
            *cnd = (cnd_t)p;
            x->cnd = (cnd_t)p;
            x->next = conds;
            conds = x;
            return thrd_success;
        }
        free(p);
        free(x);
        return thrd_error;
    }
    free(p);
    free(x);
    return thrd_nomem;
}
int _RTL_FUNC cnd_broadcast(cnd_t* cond)
{
    icnd* p = (icnd*)*cond;
    __ll_enter_critical();
    if (p->sig == CND_SIG)
    {
        int i;
        tlist* cur = p->waiting;
        for (i = 0; cur; i++)
        {
            cur = cur->next;
        }
        if (i)
        {
            __ll_cndSignal(p->handle, i);
        }
        __ll_exit_critical();
        return thrd_success;
    }
    __ll_exit_critical();
    return thrd_error;
}
int _RTL_FUNC cnd_signal(cnd_t* cond)
{
    icnd* p = (icnd*)*cond;
    __ll_enter_critical();
    if (p->sig == CND_SIG)
    {
        __ll_cndSignal(p->handle, 1);
        __ll_exit_critical();
        return thrd_success;
    }
    __ll_exit_critical();
    return thrd_error;
}
int _RTL_FUNC cnd_timedwait(cnd_t* cond, mtx_t* mtx, const struct timespec* xt)
{
    icnd* p = (icnd*)*cond;
    __ll_enter_critical();
    if (p->sig == CND_SIG)
    {
        if (mtx_unlock(mtx) == thrd_success)
        {
            tlist* x = calloc(1, sizeof(tlist));
            if (x)
            {
                int rv;
                int t;
                x->thrd = __ll_thrdcurrent();
                if (p->waiting)
                    p->tail = p->tail->next = x;
                else
                    p->waiting = p->tail = x;

                if (xt == (void*)-1)
                    t = -1;
                else
                {
                    t = __thrd_rel_delay(xt);
                    if (t < 0)
                    {
                        __ll_exit_critical();
                        free(x);
                        return thrd_error;
                    }
                }
                __ll_exit_critical();
                rv = __ll_cndWait(p->handle, t);
                rv = rv == 1 ? thrd_success : rv == 0 ? thrd_timeout : thrd_error;
                __ll_enter_critical();
                if (p->sig == CND_SIG)
                {
                    tlist** cur = &p->waiting;
                    while (*cur)
                    {
                        if (*cur == x)
                        {
                            *cur = (*cur)->next;
                            break;
                        }
                        cur = &(*cur)->next;
                    }
                    free(x);
                }
                else
                {
                    rv = thrd_error;
                }
                if (mtx_lock(mtx) != thrd_success)
                    rv = thrd_error;
                __ll_exit_critical();
                return rv;
            }
            __ll_exit_critical();
            return thrd_nomem;
        }
    }
    __ll_exit_critical();
    return thrd_error;
}
int _RTL_FUNC cnd_wait(cnd_t* cond, mtx_t* mtx)
{
    switch (__mtxThisThread(mtx))
    {
        default:
            return thrd_error;
        case thrd_success:
            return cnd_timedwait(cond, mtx, (struct timespec*)-1);
        case thrd_busy:
            abort();
            // never gets here
            return thrd_error;
    }
}
