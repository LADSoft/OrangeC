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
