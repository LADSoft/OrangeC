/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/* The prototypes in this file which start with __ll are OS dependent
 * and some of the C functions will require them to be implemented
 * notably:
 *   memory allocation
 *   file handling
 *   time & date
 *   system-level functions
 *
 * C functions which don't depend on the above mechanisms are self-contained
 * and should work generically
 */
 
#pragma pack(1)
 
#ifndef __STDDEF_H
#include <stddef.h>
#endif

#include <signal.h>
#ifdef __cplusplus
extern "C" {
#endif

#define _DTA_BUF_DEFAULT 8192 /* must be a power of two <= 64K */

#if  __STDC_VERSION__ >= 199901L
#define LLONG_TYPE long long
#else
#define LLONG_TYPE long
#endif
/* Initialize the file stuff */
void __ll_init(void);

/* File open close */
int __ll_open(const char *__name, int flags, int shflags);
int __ll_creat(const char *__name, int flags, int shflags);
int __ll_close(int __fd);

/* Convert C-style open flags to os-style open flags */
int __ll_flags(int __oldflags);

/* File read/write */
int __ll_write(int __fd,void *__buf,size_t __size);
int __ll_read(int __fd,void *__buf,size_t __len);
int __appendedwrite(FILE *stream, char *buf, int len);

/* File positioning */
size_t __ll_getpos(int __fd);
int __ll_seek(int __fd, size_t __pos, int __origin);

/* File utilities */
int __ll_rename(const char *__old, const char *__new);
int __ll_remove(const char *__name);
int __ll_rmdir(const char *name);

/* malloc stuff */
void *__ll_malloc(size_t __size);
void __ll_free(void *__blk);

/* System stuff */
int __ll_getenvsize(int id);
int __ll_getenv(char *buf, int id);
int __ll_system(const char *string);
void __ll_gettempdir(char *buf);

/* Time & date stuff */
void __ll_gettime(struct tm *__time);
int __ll_settime(struct tm *__time);
__int64 __ll_ticks(void);
void __ll_tzset(void) ;
void __ll_xtime (struct _timeb * buf);

/* Spawn function */
int __ll_spawn(char *name, char *parms, char **env, int mode);

/* assert */
void __ll_assertfail( const char *__who, const char *__file, 
                     int __line, const char *__func, const char *__msg ) ;

/* critical sections */
int __ll_enter_critical(void) ;
int __ll_exit_critical(void) ;

/* threads */
#ifdef __THREADS_H
int __ll_thrdstart(struct ithrd **thr, thrd_start_t *func, void *arglist );
void __ll_thrdexit(unsigned retval);
void __ll_thrdsleep(unsigned ms);
#endif

/* stat */
int __ll_stat(int handle, void *__statbuf) ;
int __ll_namedstat(const char *name, void *__statbuf) ;
int __ll_writeable(const char *path) ;

/* Internal functions, already implemented */
FILE *__basefopen(const char *name, const char *mode,FILE *stream, int fd, int share);
int __basefclose(FILE *stream,int release);
int __writebuf(FILE *__stream);
int __readbuf(FILE *stream) ;
int __insertTempName(char *name);
void __closeall(void);
int __wgetc(FILE *stream);
int __fputwc(int c, FILE *stream);

struct __rtl_data {
    struct __rtldata *link;
    void *handle;
    void *threadhand;
    char tmpfilnam[32];
    char setlocaledescriptor[256];
    struct lconv lconvbuf;
    char lconvstringbuf[100];
    struct tm gmtime_buf;
    char asctime_buf[26];
    char ecvt_buf[100];
    unsigned char *ctype;
    int x_errno;
    int x_doserrno;
    int x_signgam;
    mbstate_t wctomb_st;
    mbstate_t wcstombs_st;
    mbstate_t mbtowc_st;
    mbstate_t mbstowcs_st;
    mbstate_t mblen_st;
    mbstate_t wcrtomb_st;
    mbstate_t mbsrtowcs_st;
    mbstate_t mbrtowc_st;
    mbstate_t mbrlen_st;
    char *strtok_pos;
    void *thrd_id;
    unsigned char *thread_local_data;
};
#define STRT_ERR_RET_CV 0
#define STRT_ERR_RET_SIGNED 1
#define STRT_ERR_RET_UNSIGNED 2

LLONG_TYPE __xstrtoimax(FILE *fil, int count, int *ch, int *chars,
        int radix, unsigned LLONG_TYPE max, int full, int errmode);
long double __xstrtod(FILE *fil, int count, int *ch, int *chars,
            long double max,  int exp2, int exp10, int full);
char *__onetostr(FILE *fil, const char *format, void *arg,
            int *count, int *written);
char *__strtoone(FILE *fil, const char *format, void *arg, int *count,int *chars,int *argn, int *ch);
int __scanf(FILE *fil, const char *format,void *arglist);
wchar_t *__wonetostr(FILE *fil, const wchar_t *format, void *arg,
            int *count, int *written);
wchar_t *__wstrtoone(FILE *restrict fil, const wchar_t *restrict format, void *restrict arg, int *restrict count,int *restrict chars,int *restrict argn, int *restrict ch);
int __wscanf(FILE *fil, const wchar_t *format,void *arglist);
LLONG_TYPE __xwcstoimax(FILE *fil, int count, int *ch, int *chars,
        int radix, unsigned LLONG_TYPE max, int full, int errmode);
long double __xwcstod(FILE *fil, int count, int *ch, int *chars,
            long double max,  int exp2, int exp10, int full);
void __threadinit(void);
void __threadrundown(void);
struct __rtl_data *__getRtlData(void);
struct __rtl_data *__threadTlsAlloc(int cs);
void __threadTlsFree(int cs);
void __threadTlsFreeAll(void);

/* signal */
void __ll_signal(int signum, sighandler_t func) ;
void __ll_cancelsleep(void);

/*
 * Now for math funcs used in sprintf, scanf, atof
 */
int __fextract(long double *val, int *exp, int *sign, unsigned char *BCD);
int __fnd(unsigned char *BCD, int index);
long double __fpow(int exp);

#ifdef __cplusplus
};
#endif

/* The following constants and structures govern the memory allocation
 * mechanism
 */
#define ALLOCSIZE 256*1024	/* size of a minimum os-level allocation */
#define MEMCHAINS 32		/* number of hash chains */

typedef struct _freelist {
    struct _freelist *next;
    size_t size;
} FREELIST;

typedef struct _blkhead {
    struct _freelist *freemem ;
    struct _blkhead *next;
} BLKHEAD;

/* Unix IO function declarations */
void __ll_uioinit(void) ;
int __ll_uio_flags(int);
void __uio_rundown(void) ;
int __ll_chmod(const char *path, int amode) ;
int __ll_chsize(int handle, int size) ;
int __ll_dup(int handle) ;
int __ll_setftime(int handle,void *ftimep) ;
int __ll_getftime(int handle,void *ftimep) ;
int __ll_utime(int handle, struct tm *access, struct tm * modify);
int __ll_isatty(int handle) ;
int __ll_lock(int handle, int offset, int length) ;
int __ll_unlock(int handle, int offset, int length) ;
int __ll_uioflags(int flags) ;
int __ll_shflags(int mode) ;
void __ll_findclose(void *buf) ;
int __uiohandle(int __handle) ;
int __uinewhandpos(void) ;
void __uio_clearerr(int __handle);

#define UIF_EOF 1
#define UIF_RO  2
#define UIF_WRITEABLE 0x40000000

void __ll_mtxFree(long long handle);
long long __ll_mtxAlloc(void);
int __ll_mtxTake(long long handle, unsigned ms);
int __ll_mtxRelease(long long handle);

void __ll_cndFree(long long handle);
long long __ll_cndAlloc(void);
int __ll_cndWait(long long handle, unsigned ms);
int __ll_cndSignal(long long handle, int cnt);

struct ithrd
{
    int sig;
    void *start;
    void *arglist;
    void *handle;
    struct itsslst {
        struct itsslst *next;
        void *tss;
    } *tsslst;
    char join;
    char detach;
    
} ;
//int __ll_thrdstart(struct ithrd **thr, thrd_start_t *func, void *arglist );
//int __ll_thrdexit(int res);
int __ll_thrdexitcode(struct ithrd *thrd, int *rv);
int __ll_thrdwait(struct ithrd *thrd);
int __ll_thrd_detach(struct ithrd *thrd);
struct ithrd *__ll_thrdcurrent(void);
//int __ll_thrdsleep(int ms);
#pragma pack()