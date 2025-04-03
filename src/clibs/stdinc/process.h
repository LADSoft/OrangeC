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

#ifndef __PROCESS_H
#define __PROCESS_H

#    include <stddef.h>
#    include <stdint.h>

#define P_WAIT 0
#define P_NOWAIT 1
#define P_OVERLAY 2
#define P_NOWAITO 3
#define P_DETACH 4

#define _P_WAIT P_WAIT
#define _P_NOWAIT P_NOWAIT
#define _P_OVERLAY P_OVERLAY
#define _P_NOWAITO P_NOWAITO
#define _P_DETACH P_DETACH

#define WAIT_CHILD 0
#define WAIT_GRANDCHILD 1

#ifndef RC_INVOKED
#ifdef __cplusplus
extern "C"
{
#endif
    void _RTL_FUNC _IMPORT _NORETURN abort(void);
    void _RTL_FUNC _IMPORT _NORETURN _cexit(void);
    void _RTL_FUNC _IMPORT _NORETURN _c_exit(void);
    void _RTL_FUNC _IMPORT _NORETURN exit(int __status);
    void _RTL_FUNC _IMPORT _NORETURN _Exit(int __status);
    void _RTL_FUNC _IMPORT _NORETURN _exit(int __status);

    int _RTL_FUNC _IMPORT cwait(int*, int, int);
    int _RTL_FUNC _IMPORT execl(const char* ZSTR __path, const char* ZSTR __argv0, ...);
    int _RTL_FUNC _IMPORT execle(const char* ZSTR __path, const char* ZSTR __argv0, ...);
    int _RTL_FUNC _IMPORT execlp(const char* ZSTR __path, const char* ZSTR __argv0, ...);
    int _RTL_FUNC _IMPORT execlpe(const char* ZSTR __path, const char* ZSTR __argv0, ...);
    int _RTL_FUNC _IMPORT execv(const char* ZSTR __path, const char* ZSTR* __argv);
    int _RTL_FUNC _IMPORT execve(const char* ZSTR __path, const char* ZSTR* __argv, const char* ZSTR* __env);
    int _RTL_FUNC _IMPORT execvp(const char* ZSTR __path, const char* ZSTR* __argv);
    int _RTL_FUNC _IMPORT execvpe(const char* ZSTR __path, const char* ZSTR* __argv, const char* ZSTR* __env);

    int _RTL_FUNC _IMPORT spawnl(int __mode, const char* ZSTR __path, const char* ZSTR __argv0, ...);
    int _RTL_FUNC _IMPORT spawnle(int __mode, const char* ZSTR __path, const char* ZSTR __argv0, ...);
    int _RTL_FUNC _IMPORT spawnlp(int __mode, const char* ZSTR __path, const char* ZSTR __argv0, ...);
    int _RTL_FUNC _IMPORT spawnlpe(int __mode, const char* ZSTR __path, const char* ZSTR __argv0, ...);
    int _RTL_FUNC _IMPORT spawnv(int __mode, const char* ZSTR __path, const char* ZSTR* __argv);
    int _RTL_FUNC _IMPORT spawnve(int __mode, const char* ZSTR __path, const char* ZSTR* __argv, const char* ZSTR* __env);
    int _RTL_FUNC _IMPORT spawnvp(int __mode, const char* ZSTR __path, const char* ZSTR* __argv);
    int _RTL_FUNC _IMPORT spawnvpe(int __mode, const char* ZSTR __path, const char* ZSTR* __argv, const char* ZSTR* __env);

    int _RTL_FUNC _IMPORT _cwait(int*, int, int);
    int _RTL_FUNC _IMPORT _execl(const char* ZSTR, const char* ZSTR, ...);
    int _RTL_FUNC _IMPORT _execle(const char* ZSTR, const char* ZSTR, ...);
    int _RTL_FUNC _IMPORT _execlp(const char* ZSTR, const char* ZSTR, ...);
    int _RTL_FUNC _IMPORT _execlpe(const char* ZSTR, const char* ZSTR, ...);
    int _RTL_FUNC _IMPORT _execv(const char* ZSTR, const char* ZSTR const*);
    int _RTL_FUNC _IMPORT _execve(const char* ZSTR, const char* ZSTR const*, const char* ZSTR const*);
    int _RTL_FUNC _IMPORT _execvp(const char* ZSTR, const char* ZSTR const*);
    int _RTL_FUNC _IMPORT _execvpe(const char* ZSTR, const char* ZSTR const*, const char* ZSTR const*);
    int _RTL_FUNC _IMPORT _spawnl(int, const char* ZSTR, const char* ZSTR, ...);
    int _RTL_FUNC _IMPORT _spawnle(int, const char* ZSTR, const char* ZSTR, ...);
    int _RTL_FUNC _IMPORT _spawnlp(int, const char* ZSTR, const char* ZSTR, ...);
    int _RTL_FUNC _IMPORT _spawnlpe(int, const char* ZSTR, const char* ZSTR, ...);
    int _RTL_FUNC _IMPORT _spawnv(int, const char* ZSTR, const char* ZSTR const*);
    int _RTL_FUNC _IMPORT _spawnve(int, const char* ZSTR, const char* ZSTR const*, const char* ZSTR const*);
    int _RTL_FUNC _IMPORT _spawnvp(int, const char* ZSTR, const char* ZSTR const*);
    int _RTL_FUNC _IMPORT _spawnvpe(int, const char* ZSTR, const char* ZSTR const*, const char* ZSTR const*);

#ifdef _WIN32
    uintptr_t _RTL_FUNC _IMPORT _beginthread(void(__cdecl* __start_address)(void*), unsigned __stack_size, void* __arglist);
    uintptr_t _RTL_FUNC _IMPORT _beginthreadex(void* __security, unsigned __stack_size, unsigned(__stdcall* __start_address)(void*),
                                               void* __arglist, unsigned __initflag, unsigned* __thrdaddr);

    void _RTL_FUNC _IMPORT _endthread(void);
    void _RTL_FUNC _IMPORT _endthreadex(unsigned retval);

    int _RTL_FUNC _IMPORT getpid(void);
    int _RTL_FUNC _IMPORT _getpid(void);
    int _RTL_FUNC _IMPORT _threadid(void);
    int _RTL_FUNC _IMPORT __threadid(void);
#endif

#ifdef __cplusplus
};
#endif
#endif

#endif /* __PROCESS_H */
