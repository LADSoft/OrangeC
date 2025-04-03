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
#include <threads.h>
#include <stdlib.h>
#include "libp.h"
#include <windows.h>
extern int __thrd_rel_delay(const struct timespec* xt);

#define MTX_SIG 0x58544d5e  // "_MTX"
typedef struct
{
    int sig;
    int mode;
    thrd_t owner;
    int lockcount;
    long long handle;
} imtx;
struct mlist
{
    struct mlist* next;
    mtx_t mtx;
} * mtxs;
static void remove_thrd(mtx_t* mtx, thrd_t thrd)
{
    mtx_t * cur = (((imtx*)mtx)->owner);
        if (thrd == cur)
    {
        (((imtx*)mtx)->owner) = NULL;
    }
}
void __mtx_remove_thrd(thrd_t thrd)
{
    struct mlist* cur = mtxs;
    while (cur)
    {
        remove_thrd(cur->mtx, thrd);
        cur = cur->next;
    }
}
int __mtxThisThread(mtx_t* mtx)
{
    imtx* p = (imtx*)*mtx;
    __ll_enter_critical();
    if (p->sig == MTX_SIG)
    {
        if (p->owner && p->owner == __ll_thrdcurrent())
        {
            __ll_exit_critical();
            return thrd_success;
        }
        __ll_exit_critical();
        return thrd_busy;
    }
    __ll_exit_critical();
    return thrd_error;
}
void _RTL_FUNC mtx_destroy(mtx_t* mtx)
{
    imtx* p = (imtx*)*mtx;
    __ll_enter_critical();
    if (p->sig == MTX_SIG)
    {
        struct mlist** mcur = &mtxs;
        struct mlist* tofree = NULL;
        while (*mcur)
        {
            if ((*mcur)->mtx == mtx)
            {
                tofree = *mcur;
                *mcur = (*mcur)->next;
                break;
            }
            mcur = &(*mcur)->next;
        }
        if (tofree)
            free(tofree);
        __ll_mtxFree(p->handle);
        p->sig = 0;

        free(p);
    }
    __ll_exit_critical();
}
int _RTL_FUNC mtx_init(mtx_t* mtx, int type)
{
    imtx* p = calloc(1, sizeof(imtx));
    struct mlist* x = calloc(1, sizeof(struct mlist));
    if (p && x)
    {
        if ((p->handle = __ll_mtxAlloc()) != 0)
        {
            p->sig = MTX_SIG;
            p->mode = type;
            *mtx = (mtx_t)p;
            x->mtx = (mtx_t)p;
            x->next = mtxs;
            mtxs = x;
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
int _RTL_FUNC mtx_timedlock(mtx_t* mtx, const struct timespec* xt)
{
    imtx* p = (imtx*)*mtx;
    __ll_enter_critical();
    if (p->sig == MTX_SIG && (xt == (void*)-1 || (xt != (void*)-1 && ((p->mode & ~mtx_recursive) == mtx_try || xt != NULL && (p->mode & ~mtx_recursive) == mtx_timed))))
    {
        if (!(p->mode & mtx_recursive) && p->owner && p->owner == __ll_thrdcurrent())
        {
            __ll_exit_critical();
            return thrd_busy;
        }
        int t;
        if (xt == NULL)
            t = 0;
        else if (xt == (void*)-1)
            t = -1;
        else
            t = __thrd_rel_delay(xt);
        if (t < 0 && t != -1)
        {
            __ll_exit_critical();
            return thrd_error;
        }
        __ll_exit_critical();
        switch (__ll_mtxTake(p->handle, t))
        {
            case 1:
                __ll_enter_critical();
                if (p->sig == MTX_SIG)
                {
                    p->owner = __ll_thrdcurrent();
                    p->lockcount++;
                    __ll_exit_critical();
                    return thrd_success;
                }
                __ll_exit_critical();
                return thrd_error;
            case 0:
                __ll_enter_critical();
                if (p->sig == MTX_SIG)
                {
                    __ll_exit_critical();
                    if (xt == NULL)
                        return thrd_busy;
                    else
                        return thrd_timeout;
                }
                __ll_exit_critical();
                // fall through
            default:
                    return thrd_error;
        }
       __ll_exit_critical();
        return thrd_nomem;
    }
    __ll_exit_critical();
    return thrd_error;
}
int _RTL_FUNC mtx_lock(mtx_t* mtx) { return mtx_timedlock(mtx, (void*)-1); }
int _RTL_FUNC mtx_trylock(mtx_t* mtx) { return mtx_timedlock(mtx, 0); }
int _RTL_FUNC mtx_unlock(mtx_t* mtx)
{
    imtx* p = (imtx*)*mtx;
    __ll_enter_critical();
    if (p->sig == MTX_SIG)
    {
        if (p->owner == __ll_thrdcurrent())
        {
           if (!--p->lockcount)
           {
                p->owner = NULL;
                __ll_mtxRelease(p->handle);
            }
            __ll_exit_critical();
            return thrd_success;
        }
    }
    __ll_exit_critical();
    return thrd_error;
}
