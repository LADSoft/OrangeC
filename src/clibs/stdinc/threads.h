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

#pragma GCC system_header

#ifndef __THREADS_H
#define __THREADS_H

#    include <stddef.h>
#    include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define ONCE_FLAG_INIT \
    {                  \
        0              \
    }
#define TSS_DTOR_ITERATIONS 20

#ifndef RC_INVOKED
    typedef void* _THAND;
    typedef _THAND* cnd_t;
    typedef _THAND* thrd_t;
    typedef _THAND* tss_t;
    typedef _THAND* mtx_t;

    typedef void (*tss_dtor_t)(void*);
    typedef int (*thrd_start_t)(void*);

    typedef struct
    {
        char called;
    } once_flag;

    enum mtx_e
    {
        mtx_plain,
        mtx_timed,
        mtx_try,
        mtx_recursive = 4,

    };
    enum thrd_e
    {
        thrd_success,
        thrd_error,
        thrd_nomem,
        thrd_timeout,
        thrd_busy,

    };

    void _RTL_FUNC _IMPORT call_once(once_flag* flag, void (*func)(void));

    int _RTL_FUNC _IMPORT cnd_broadcast(cnd_t* cond);
    void _RTL_FUNC _IMPORT cnd_destroy(cnd_t* cond);
    int _RTL_FUNC _IMPORT cnd_init(cnd_t* cond);
    int _RTL_FUNC _IMPORT cnd_signal(cnd_t* cond);
    int _RTL_FUNC _IMPORT cnd_timedwait(cnd_t* cond, mtx_t* mtx, const struct timespec* xt);
    int _RTL_FUNC _IMPORT cnd_wait(cnd_t* cond, mtx_t* mtx);

    void _RTL_FUNC _IMPORT mtx_destroy(mtx_t* mtx);
    int _RTL_FUNC _IMPORT mtx_init(mtx_t* mtx, int type);
    int _RTL_FUNC _IMPORT mtx_lock(mtx_t* mtx);
    int _RTL_FUNC _IMPORT mtx_timedlock(mtx_t* mtx, const struct timespec* xt);
    int _RTL_FUNC _IMPORT mtx_trylock(mtx_t* mtx);
    int _RTL_FUNC _IMPORT mtx_unlock(mtx_t* mtx);

    int _RTL_FUNC _IMPORT thrd_create(thrd_t* thr, thrd_start_t func, void* arg);
    thrd_t _RTL_FUNC _IMPORT thrd_current(void);
    int _RTL_FUNC _IMPORT thrd_detach(thrd_t thr);
    int _RTL_FUNC _IMPORT thrd_equal(thrd_t thr0, thrd_t thr1);
    void _RTL_FUNC _IMPORT _NORETURN thrd_exit(int res);
    int _RTL_FUNC _IMPORT thrd_join(thrd_t thr, int* res);
    void _RTL_FUNC _IMPORT thrd_sleep(const struct timespec* duration, struct timespec* remaining);
    void _RTL_FUNC _IMPORT thrd_yield(void);

    int _RTL_FUNC _IMPORT tss_create(tss_t* key, tss_dtor_t dtor);
    void _RTL_FUNC _IMPORT tss_delete(tss_t key);
    void* _RTL_FUNC _IMPORT tss_get(tss_t key);
    int _RTL_FUNC _IMPORT tss_set(tss_t key, void* val);
#endif

#ifdef __cplusplus
}
#endif

#endif