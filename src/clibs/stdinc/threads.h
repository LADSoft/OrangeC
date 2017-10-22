#ifndef __THREADS_H
#define __THREADS_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

#ifndef  _TIME_T
typedef long time_t;
#ifdef __cplusplus
#  define _TIME_T __STD_NS_QUALIFIER time_t
#else
#  define _TIME_T time_t
#endif /* __cplusplus */
#endif

#define ONCE_FLAG_INIT  { 0 }
#define TSS_DTOR_ITERATIONS 20

#define TIME_UTC 1

typedef void * _THAND;
typedef _THAND *cnd_t;
typedef _THAND *thrd_t;
typedef _THAND *tss_t;
typedef _THAND *mtx_t;

typedef void(*tss_dtor_t)(void *);
typedef int (*thrd_start_t)(void *);

typedef struct
{
    char called;
} once_flag;

typedef struct {
    time_t sec;
    long nsec;
} timespec;

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

void    _RTL_FUNC _IMPORT call_once(once_flag *flag, void (*func)(void));

int     _RTL_FUNC _IMPORT cnd_broadcast(cnd_t *cond);
void    _RTL_FUNC _IMPORT cnd_destroy(cnd_t *cond);
int     _RTL_FUNC _IMPORT cnd_init(cnd_t *cond);
int     _RTL_FUNC _IMPORT cnd_signal(cnd_t *cond);
int     _RTL_FUNC _IMPORT cnd_timedwait(cnd_t *cond, mtx_t *mtx, const timespec *xt);
int     _RTL_FUNC _IMPORT cnd_wait(cnd_t *cond, mtx_t *mtx);

void    _RTL_FUNC _IMPORT mtx_destroy(mtx_t *mtx);
int     _RTL_FUNC _IMPORT mtx_init(mtx_t *mtx, int type);
int     _RTL_FUNC _IMPORT mtx_lock(mtx_t *mtx);
int     _RTL_FUNC _IMPORT mtx_timedlock(mtx_t *mtx, const timespec *xt);
int     _RTL_FUNC _IMPORT mtx_trylock(mtx_t *mtx);
int     _RTL_FUNC _IMPORT mtx_unlock(mtx_t *mtx);

int     _RTL_FUNC _IMPORT thrd_create(thrd_t *thr, thrd_start_t func, void *arg);
thrd_t  _RTL_FUNC _IMPORT thrd_current(void);
int     _RTL_FUNC _IMPORT thrd_detach(thrd_t thr);
int     _RTL_FUNC _IMPORT thrd_equal(thrd_t thr0, thrd_t thr1);
void    _RTL_FUNC _IMPORT thrd_exit(int res);
int     _RTL_FUNC _IMPORT thrd_join(thrd_t thr, int *res);
void    _RTL_FUNC _IMPORT thrd_sleep(const timespec *duration, timespec *remaining);
void    _RTL_FUNC _IMPORT thrd_yield(void);

int     _RTL_FUNC _IMPORT tss_create(tss_t *key, tss_dtor_t dtor);
void    _RTL_FUNC _IMPORT tss_delete(tss_t key);
void    *_RTL_FUNC _IMPORT tss_get(tss_t key);
int     _RTL_FUNC _IMPORT tss_set(tss_t key, void *val);

int     _RTL_FUNC _IMPORT timespec_get(timespec *xt, int base);
#ifdef __cplusplus
}
}
#endif

#endif