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

#ifdef __cplusplus
namespace std {
extern "C" {
#endif
void       _RTL_FUNC abort(void);
void       _RTL_FUNC _cexit(void);
void       _RTL_FUNC _c_exit(void);
void       _RTL_FUNC exit(int __status);
void       _RTL_FUNC _Exit(int __status);
void       _RTL_FUNC _exit(int __status);

int		   _RTL_FUNC cwait(int *, int, int);
int        _RTL_FUNC execl(const char *__path, const char *__argv0,...);
int        _RTL_FUNC execle(const char *__path, const char *__argv0,...);
int        _RTL_FUNC execlp(const char *__path, const char *__argv0,...);
int        _RTL_FUNC execlpe(const char *__path, const char *__argv0,...);
int        _RTL_FUNC execv(const char *__path, const char **__argv);
int        _RTL_FUNC execve(const char *__path, const char **__argv, const char **__env);
int        _RTL_FUNC execvp(const char *__path, const char **__argv);
int        _RTL_FUNC execvpe(const char *__path, const char **__argv, const char **__env);

int        _RTL_FUNC spawnl(int __mode,const char *__path, const char *__argv0,...);
int        _RTL_FUNC spawnle(int __mode,const char *__path, const char *__argv0,...);
int        _RTL_FUNC spawnlp(int __mode,const char *__path, const char *__argv0,...);
int        _RTL_FUNC spawnlpe(int __mode,const char *__path, const char *__argv0,...);
int        _RTL_FUNC spawnv(int __mode,const char *__path, const char **__argv);
int        _RTL_FUNC spawnve(int __mode,const char *__path, const char **__argv, const char **__env);
int        _RTL_FUNC spawnvp(int __mode,const char *__path, const char **__argv);
int        _RTL_FUNC spawnvpe(int __mode,const char *__path, const char **__argv, const char **__env);

int _RTL_FUNC _cwait(int *, int, int);
int _RTL_FUNC _execl(const char *, const char *, ...);
int _RTL_FUNC _execle(const char *, const char *, ...);
int _RTL_FUNC _execlp(const char *, const char *, ...);
int _RTL_FUNC _execlpe(const char *, const char *, ...);
int _RTL_FUNC _execv(const char *, const char * const *);
int _RTL_FUNC _execve(const char *, const char * const *, const char * const *);
int _RTL_FUNC _execvp(const char *, const char * const *);
int _RTL_FUNC _execvpe(const char *, const char * const *, const char * const *);
int _RTL_FUNC _spawnl(int, const char *, const char *, ...);
int _RTL_FUNC _spawnle(int, const char *, const char *, ...);
int _RTL_FUNC _spawnlp(int, const char *, const char *, ...);
int _RTL_FUNC _spawnlpe(int, const char *, const char *, ...);
int _RTL_FUNC _spawnv(int, const char *, const char * const *);
int _RTL_FUNC _spawnve(int, const char *, const char * const *,
        const char * const *);
int _RTL_FUNC _spawnvp(int, const char *, const char * const *);
int _RTL_FUNC _spawnvpe(int, const char *, const char * const *,
        const char * const *);

#ifndef __STDC__
uintptr_t _RTL_FUNC _beginthread(void( __cdecl *__start_address )( void * ),
   unsigned __stack_size,
   void *__arglist 
);
uintptr_t _RTL_FUNC _beginthreadex(void *__security, unsigned __stack_size,
   unsigned ( __stdcall *__start_address )( void * ),
   void *__arglist, unsigned __initflag, unsigned *__thrdaddr);
#endif
   
void _RTL_FUNC _endthread( void );
void _RTL_FUNC _endthreadex(unsigned retval);

#ifdef __cplusplus
};
};
#endif

#endif /* __PROCESS_H */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__PROCESS_H_USING_LIST)
#define __PROCESS_H_USING_LIST
    using std::abort;
    using std::_cexit;
    using std::_c_exit;
    using std::exit;
    using std::_exit;
    using std::cwait;
    using std::execl;
    using std::execle;
    using std::execlp;
    using std::execlpe;
    using std::execv;
    using std::execve;
    using std::execvp;
    using std::execvpe;
    using std::spawnl;
    using std::spawnle;
    using std::spawnlp;
    using std::spawnlpe;
    using std::spawnv;
    using std::spawnve;
    using std::spawnvp;
    using std::spawnvpe;
    using std::_cwait;
    using std::_execl;
    using std::_execle;
    using std::_execlp;
    using std::_execlpe;
    using std::_execv;
    using std::_execve;
    using std::_execvp;
    using std::_execvpe;
    using std::_spawnl;
    using std::_spawnle;
    using std::_spawnlp;
    using std::_spawnlpe;
    using std::_spawnv;
    using std::_spawnve;
    using std::_spawnvp;
    using std::_spawnvpe;
    using std::_beginthread;
    using std::_beginthreadex;
    using std::_endthread;
    using std::_endthreadex;
    using std::uintptr_t;
#endif
