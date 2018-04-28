/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 */

#ifndef __PROCESS_H
#define __PROCESS_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifndef __STDINT_H
#include <stdint.h>
#endif

#define P_WAIT    0
#define P_NOWAIT  1
#define P_OVERLAY 2
#define P_NOWAITO 3
#define P_DETACH  4

#define _P_WAIT    P_WAIT
#define _P_NOWAIT  P_NOWAIT
#define _P_OVERLAY P_OVERLAY
#define _P_NOWAITO P_NOWAITO
#define _P_DETACH  P_DETACH

#define WAIT_CHILD      0
#define WAIT_GRANDCHILD 1

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif
void       _RTL_FUNC _IMPORT abort(void);
void       _RTL_FUNC _IMPORT _cexit(void);
void       _RTL_FUNC _IMPORT _c_exit(void);
void       _RTL_FUNC _IMPORT exit(int __status);
void       _RTL_FUNC _IMPORT _Exit(int __status);
void       _RTL_FUNC _IMPORT _exit(int __status);

int		   _RTL_FUNC _IMPORT cwait(int *, int, int);
int        _RTL_FUNC _IMPORT execl(const char *__path, const char *__argv0,...);
int        _RTL_FUNC _IMPORT execle(const char *__path, const char *__argv0,...);
int        _RTL_FUNC _IMPORT execlp(const char *__path, const char *__argv0,...);
int        _RTL_FUNC _IMPORT execlpe(const char *__path, const char *__argv0,...);
int        _RTL_FUNC _IMPORT execv(const char *__path, const char **__argv);
int        _RTL_FUNC _IMPORT execve(const char *__path, const char **__argv, const char **__env);
int        _RTL_FUNC _IMPORT execvp(const char *__path, const char **__argv);
int        _RTL_FUNC _IMPORT execvpe(const char *__path, const char **__argv, const char **__env);

int        _RTL_FUNC _IMPORT spawnl(int __mode,const char *__path, const char *__argv0,...);
int        _RTL_FUNC _IMPORT spawnle(int __mode,const char *__path, const char *__argv0,...);
int        _RTL_FUNC _IMPORT spawnlp(int __mode,const char *__path, const char *__argv0,...);
int        _RTL_FUNC _IMPORT spawnlpe(int __mode,const char *__path, const char *__argv0,...);
int        _RTL_FUNC _IMPORT spawnv(int __mode,const char *__path, const char **__argv);
int        _RTL_FUNC _IMPORT spawnve(int __mode,const char *__path, const char **__argv, const char **__env);
int        _RTL_FUNC _IMPORT spawnvp(int __mode,const char *__path, const char **__argv);
int        _RTL_FUNC _IMPORT spawnvpe(int __mode,const char *__path, const char **__argv, const char **__env);

int _RTL_FUNC _IMPORT _cwait(int *, int, int);
int _RTL_FUNC _IMPORT _execl(const char *, const char *, ...);
int _RTL_FUNC _IMPORT _execle(const char *, const char *, ...);
int _RTL_FUNC _IMPORT _execlp(const char *, const char *, ...);
int _RTL_FUNC _IMPORT _execlpe(const char *, const char *, ...);
int _RTL_FUNC _IMPORT _execv(const char *, const char * const *);
int _RTL_FUNC _IMPORT _execve(const char *, const char * const *, const char * const *);
int _RTL_FUNC _IMPORT _execvp(const char *, const char * const *);
int _RTL_FUNC _IMPORT _execvpe(const char *, const char * const *, const char * const *);
int _RTL_FUNC _IMPORT _spawnl(int, const char *, const char *, ...);
int _RTL_FUNC _IMPORT _spawnle(int, const char *, const char *, ...);
int _RTL_FUNC _IMPORT _spawnlp(int, const char *, const char *, ...);
int _RTL_FUNC _IMPORT _spawnlpe(int, const char *, const char *, ...);
int _RTL_FUNC _IMPORT _spawnv(int, const char *, const char * const *);
int _RTL_FUNC _IMPORT _spawnve(int, const char *, const char * const *,
        const char * const *);
int _RTL_FUNC _IMPORT _spawnvp(int, const char *, const char * const *);
int _RTL_FUNC _IMPORT _spawnvpe(int, const char *, const char * const *,
        const char * const *);

#ifdef _WIN32
uintptr_t _RTL_FUNC _IMPORT _beginthread(void( __cdecl *__start_address )( void * ),
   unsigned __stack_size,
   void *__arglist 
);
uintptr_t _RTL_FUNC _IMPORT _beginthreadex(void *__security, unsigned __stack_size,
   unsigned ( __stdcall *__start_address )( void * ),
   void *__arglist, unsigned __initflag, unsigned *__thrdaddr);

void _RTL_FUNC _IMPORT _endthread( void );
void _RTL_FUNC _IMPORT _endthreadex(unsigned retval);

int _RTL_FUNC _IMPORT getpid(void);
int _RTL_FUNC _IMPORT _getpid(void);
int _RTL_FUNC _IMPORT _threadid(void);
int _RTL_FUNC _IMPORT __threadid(void);
#endif
   
#ifdef __cplusplus
};
};
#endif

#endif /* __PROCESS_H */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__PROCESS_H_USING_LIST)
#define __PROCESS_H_USING_LIST
	using __STD_NS_QUALIFIER abort;
	using __STD_NS_QUALIFIER _cexit;
	using __STD_NS_QUALIFIER _c_exit;
	using __STD_NS_QUALIFIER exit;
	using __STD_NS_QUALIFIER _exit;
	using __STD_NS_QUALIFIER cwait;
	using __STD_NS_QUALIFIER execl;
	using __STD_NS_QUALIFIER execle;
	using __STD_NS_QUALIFIER execlp;
	using __STD_NS_QUALIFIER execlpe;
	using __STD_NS_QUALIFIER execv;
	using __STD_NS_QUALIFIER execve;
	using __STD_NS_QUALIFIER execvp;
	using __STD_NS_QUALIFIER execvpe;
	using __STD_NS_QUALIFIER spawnl;
	using __STD_NS_QUALIFIER spawnle;
	using __STD_NS_QUALIFIER spawnlp;
	using __STD_NS_QUALIFIER spawnlpe;
	using __STD_NS_QUALIFIER spawnv;
	using __STD_NS_QUALIFIER spawnve;
	using __STD_NS_QUALIFIER spawnvp;
	using __STD_NS_QUALIFIER spawnvpe;
	using __STD_NS_QUALIFIER _cwait;
	using __STD_NS_QUALIFIER _execl;
	using __STD_NS_QUALIFIER _execle;
	using __STD_NS_QUALIFIER _execlp;
	using __STD_NS_QUALIFIER _execlpe;
	using __STD_NS_QUALIFIER _execv;
	using __STD_NS_QUALIFIER _execve;
	using __STD_NS_QUALIFIER _execvp;
	using __STD_NS_QUALIFIER _execvpe;
	using __STD_NS_QUALIFIER _spawnl;
	using __STD_NS_QUALIFIER _spawnle;
	using __STD_NS_QUALIFIER _spawnlp;
	using __STD_NS_QUALIFIER _spawnlpe;
	using __STD_NS_QUALIFIER _spawnv;
	using __STD_NS_QUALIFIER _spawnve;
	using __STD_NS_QUALIFIER _spawnvp;
	using __STD_NS_QUALIFIER _spawnvpe;
	using __STD_NS_QUALIFIER _beginthread;
	using __STD_NS_QUALIFIER _beginthreadex;
	using __STD_NS_QUALIFIER _endthread;
	using __STD_NS_QUALIFIER _endthreadex;
    using __STD_NS_QUALIFIER uintptr_t;
#endif
