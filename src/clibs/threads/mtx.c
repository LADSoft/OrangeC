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

extern int __thrd_rel_delay(const xtime *xt);

typedef struct _tlist {
    struct _tlist *next;
    thrd_t thrd;
} tlist ;

#define MTX_SIG 0x58544d5e // "_MTX"
typedef struct
{
    int sig;
    int mode;
    tlist *owners;
    long long handle;
} imtx;
struct mlist
{
    struct mlist *next;
    mtx_t mtx;
} *mtxs;
static void remove_thrd(mtx_t mtx, thrd_t thrd)
{
   tlist **cur = &(((imtx *)mtx)->owners);
   while (*cur)
   {       
       if (thrd == (*cur)->thrd)
       {
           tlist *toremove = *cur;
           *cur = (*cur)->next;
           free(toremove);
       }
       else
       {
           cur = &(*cur)->next;
       }
   }
}
void __mtx_remove_thrd(thrd_t thrd)
{
    struct mlist *cur = mtxs;
    while (cur)
    {
        remove_thrd(cur->mtx, thrd);
        cur = cur->next;
    }
}
int __mtxThisThread(mtx_t *mtx)
{
    imtx *p = (imtx *)*mtx;
    __ll_enter_critical();
    if (p->sig == MTX_SIG)
    {
        if (p->owners && p->owners->thrd == __getRtlData()->thrd_id)
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
void    _RTL_FUNC mtx_destroy(mtx_t *mtx)
{
    imtx *p = (imtx *)*mtx;
    __ll_enter_critical();
    if (p->sig == MTX_SIG)
    {
        struct mlist **mcur = &mtxs;
        struct mlist *tofree = NULL;
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
        tlist *cur = p->owners;
        while (cur)
        {
            tlist *next = cur->next;
            free(cur);
            cur = next;
        }
        __ll_mtxFree(p->handle);
        p->sig = 0;
        
        free(p);
    }
    __ll_exit_critical();
}
int     _RTL_FUNC mtx_init(mtx_t *mtx, int type)
{
    imtx *p = calloc(1, sizeof(imtx));
    struct mlist *x = calloc(1, sizeof(struct mlist));
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
static int isAvailable(imtx *mtx)
{
    thrd_t *me = (thrd_t *)__getRtlData()->thrd_id;
    tlist *cur = mtx->owners;
    if (!(mtx->mode & mtx_recursive))
    {
        while (cur)
        {
            if (cur->thrd == me)
                return 0;
            cur = cur->next;
        }
    }
    return 1;
}
int     _RTL_FUNC mtx_timedlock(mtx_t *mtx, const xtime *xt)
{
    imtx *p = (imtx *)*mtx;
    int n = p->mode & ~mtx_recursive;
    __ll_enter_critical();
    if (p->sig == MTX_SIG && (xt == (void *)-1 || (xt != (void *)-1 && (p->mode == mtx_try || xt != NULL && p->mode == mtx_timed))))
    {
        if (isAvailable(p))
        {
            int t;
            if (xt == NULL)
                t = 0;
            else if (xt == (void *)-1)
                t = -1;
            else
            {
                t = __thrd_rel_delay(xt);
                if (t < 0)
                {
                    __ll_exit_critical();
                    return thrd_error;
                }
            }
            tlist *x = calloc(1, sizeof(tlist));
            if (x)
            {
                __ll_exit_critical();
                switch (__ll_mtxTake(p->handle, t))
                {
                    case 1:
                        __ll_enter_critical();
                        if (p->sig == MTX_SIG)
                        {
                            x->next = p->owners;
                            p->owners = x;
                            x->thrd = __getRtlData()->thrd_id;
                            __ll_exit_critical();
                            return thrd_success;
                        }
                        free(x);
                         __ll_exit_critical();
                        return thrd_error;
                    case 0:
                        __ll_enter_critical();
                        if (p->sig == MTX_SIG)
                        {
                            free(x);
                            __ll_exit_critical();
                            if (xt == NULL)
                                return thrd_busy;
                            else
                                return thrd_timeout;
                            
                        }
                        __ll_exit_critical();
                        // fall through
                    default:
                        free(x);
                        return thrd_error;
                }
            }
            __ll_exit_critical();
            return thrd_nomem;
        }
        __ll_exit_critical();
        return thrd_busy;
    }
    __ll_exit_critical();
    return thrd_error;
}
int     _RTL_FUNC mtx_lock(mtx_t *mtx)
{
    return mtx_timedlock(mtx, (void *)-1);
}
int     _RTL_FUNC mtx_trylock(mtx_t *mtx)
{
    return mtx_timedlock(mtx, 0);
}
int     _RTL_FUNC mtx_unlock(mtx_t *mtx)
{
    imtx *p = (imtx *)*mtx;
    __ll_enter_critical();
    if (p->sig == MTX_SIG)
    {
       if (p->owners && p->owners->thrd == __getRtlData()->thrd_id)
       {
           tlist *x = p->owners;
           p->owners = p->owners->next;
           free(x);
           __ll_exit_critical();
           __ll_mtxRelease(p->handle);
           return thrd_success;
       }
    }
    __ll_exit_critical();
    return thrd_error;
}
