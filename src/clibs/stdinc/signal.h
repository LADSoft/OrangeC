/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#ifndef __SIGNAL_H
#define __SIGNAL_H

#ifndef __STDDEF_H
#    include <stddef.h>
#endif

#ifndef __TYPES_H
#    include <sys/types.h>
#endif

#ifndef __TIME_H
#    include <time.h>
#endif
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef RC_INVOKED
    typedef int sig_atomic_t; /* Atomic entity type (ANSI) */

    typedef void (*sighandler_t)(int);
#endif

#define SIG_DFL ((sighandler_t)0)  /* Default action   */
#define SIG_IGN ((sighandler_t)1)  /* Ignore action    */
#define SIG_ERR ((sighandler_t)-1) /* Error return     */

#define SIGABRT 22
#define SIGFPE 8 /* Floating point trap  */
#define SIGILL 4 /* Illegal instruction  */
#define SIGINT 2
#define SIGSEGV 11 /* Memory access violation */
#define SIGBUS 12  /* data alignment error */
#define SIGTERM 15
#define SIGUSR1 16  /* User-defined signal 1 */
#define SIGUSR2 17  /* User-defined signal 2 */
#define SIGUSR3 20  /* User-defined signal 3 */
#define SIGBREAK 21 /* Control-Break interrupt */
#define SIGALRM 23 /* alarm */

#ifndef RC_INVOKED
    int _RTL_FUNC _IMPORT raise(int __sig);
    sighandler_t _RTL_FUNC _IMPORT signal(int __sig, sighandler_t __func);
#endif

#define NSIG 24 /* highest defined signal no. + 1 */

#define SA_SIGINFO 1
#define SA_NOCLDSTOP 2
#define SA_NOCLDWAIT 4
#define SA_NODEFER 8
#define SA_ONSTACK 16
#define SA_RESETHAND 32
#define SA_RESTART 64

#define SI_USER 0
#define SI_KERNEL 1
#define SI_QUEUE 2
#define SI_TIMER 3
#define SI_MESGQ 4
#define SI_ASYNCIO 5
#define SI_SIGIO 6
#define SI_TKILL 7

#define ILL_ILLOPC 0
#define ILL_ILLOPN 1
#define ILL_ILLADR 2
#define ILL_ILLTRP 3
#define ILL_PRVOPC 4
#define ILL_PRVREG 5
#define ILL_COPROC 6
#define ILL_BADSTK 7
#define FPE_INTDIV 0
#define FPE_INTOVF 1
#define FPE_FLTDIV 2
#define FPE_FLTOVF 3
#define FPE_FLTUND 4
#define FPE_FLTRES 5
#define FPE_FLTINV 6
#define FPE_FLTSUB 7
#define SEGV_MAPERR 0
#define SEGV_ACCERR 1
#define BUS_ADRALN 0
#define BUS_ADRERR 1
#define BUS_OBJERR 2
#define BUS_MCEERR_AR 3
#define BUS_MCEERR_AO 4
#define TRAP_BRKPT 0
#define TRAP_TRACE 1
#define TRAP_BRANCH 2
#define TRAP_HWBKPT 3
#define CLD_EXITED 0
#define CLD_KILLED 1
#define CLD_DUMPED 2
#define CLD_TRAPPED 3
#define CLD_STOPPED 4
#define CLD_CONTINUED 5
#define POLL_IN 0
#define POLL_OUT 1
#define POLL_MSG 2
#define POLL_ERR 3
#define POLL_PRI 4
#define POLL_HUP 5

#ifndef RC_INVOKED
    typedef unsigned sigval_t;
    typedef unsigned sigset_t;  // there are a max of 24 signals in this implementation

    typedef struct _siginfo_t
    {
        int si_signo; /* Signal number */
        int si_errno; /* An errno value */
        int si_code;  /* Signal code */
        union
        {
            struct
            {
                pid_t si_pid;     /* Sending process ID */
                uid_t si_uid;     /* Real user ID of sending process */
                int si_int;       /* POSIX.1b signal */
                void* si_ptr;     /* POSIX.1b signal */
                int si_status;    /* Exit value or signal */
                clock_t si_utime; /* User time consumed */
                clock_t si_stime; /* System time consumed */
            };
            sigval_t si_value; /* Signal value */
            struct
            {
                int si_overrun; /* Timer overrun count; POSIX.1b timers */
                int si_timerid; /* Timer ID; POSIX.1b timers */
            };

            struct
            {
                void* si_addr;     /* Memory location which caused fault */
                int si_trapno;     /* Trap number that caused
                                      hardware-generated signal
                                      (unused on most architectures) */
                short si_addr_lsb; /* Least significant bit of address
                                      (since kernel 2.6.32) */
            };
            struct
            {
                long si_band; /* Band event (was int in
                                 glibc 2.3.2 and earlier) */
                int si_fd;    /* File descriptor */
            };
        };
    } siginfo_t;

    struct sigaction
    {
        union
        {
            sighandler_t sa_handler;
            void (*sa_sigaction)(int, siginfo_t*, void*);
        };
        sigset_t sa_mask;
        int sa_flags;
        void (*sa_restorer)(void);
    };
#endif

#define SIGEV_NONE 0
#define SIGEV_SIGNAL 1
#define SIGEV_THREAD 2

#ifndef RC_INVOKED
    struct sigevent
    {
        int segev_notify;
        int segev_signo;
        union sigval
        {
            int sival_int;
            void* sival_ptr;
        } sigev_value;
        void (*sigev_notify_function)(union sigval);
        pthread_attr_t* sigev_notify_attributes;
    };

    typedef struct
    {
        void* ss_Sp;
        size_t ss_size;
        int ss_flags;
    } stack_t;
    typedef struct
    {
        void* unused;
    } mcontext_t;
    typedef struct _ucontext_t
    {
        struct _ucontext_t* uc_link;
        sigset_t uc_sigmask;
        stack_t uc_stack;
        mcontext_t uc_mcontext;
    } uncontext_t;

    int _RTL_FUNC kill(pid_t, int);
    /*
    void   psiginfo(const siginfo_t *, const char *);
    void   psignal(int, const char *);
    int    pthread_kill(pthread_t, int);
    int    pthread_sigmask(int, const sigset_t *restrict,
               sigset_t *restrict);
    */
    int _RTL_FUNC sigaction(int signum, const struct sigaction* act, struct sigaction* oldact);
    int _RTL_FUNC sigaddset(sigset_t*, int);
    int _RTL_FUNC sigdelset(sigset_t*, int);
    int _RTL_FUNC sigemptyset(sigset_t*);
    int _RTL_FUNC sigfillset(sigset_t*);
    int _RTL_FUNC sigismember(const sigset_t*, int);
    /*
    int    sigpending(sigset_t *);
    int    sigprocmask(int, const sigset_t *restrict, sigset_t *restrict);
    int    sigqueue(pid_t, int, union sigval);
    int    sigsuspend(const sigset_t *);
    int    sigtimedwait(const sigset_t *restrict, siginfo_t *restrict,
               const struct timespec *restrict);
    int    sigwait(const sigset_t *restrict, int *restrict);
    int    sigwaitinfo(const sigset_t *restrict, siginfo_t *restrict);
    */
#endif

#ifdef __cplusplus
};
#endif

#endif /* __SIGNAL_H */