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
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#define GUI 1
#define DLL 2

#include <windows.h>
#include <setjmp.h>
#include <string.h>

extern char INITSTART[], INITEND[], EXITSTART[], EXITEND[], BSSSTART[], BSSEND[];
extern int _argc;
extern char **_argv;
extern char **_environ;
char __export *_oscmd;
char __export *_osenv;
HINSTANCE 	__export __hInstance;
unsigned	_win32 = 0;
jmp_buf __exitbranch, __abortbranch;
static unsigned	dllexists = 0;
static int msvcrt_compat = 0;
unsigned _isDLL = 1;
void (*userRundown)();
void PASCAL __xceptinit(int *block);
void PASCAL __xceptrundown(void);
// in the follow, the args are ONLY valid for DLLs
int __stdcall ___startup(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved)
{
    __hInstance = hInst;
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            __threadinit();
        case DLL_THREAD_ATTACH:
            __threadTlsAlloc(FALSE);
            break;
        case DLL_PROCESS_DETACH:
            __threadTlsFreeAll();
            __threadrundown();
            break;
        case DLL_THREAD_DETACH:
            __threadTlsFree(FALSE);
            break;
    }
    return TRUE;
}
void __export __stdcall ___lsdllinit(DWORD flags, void (*rundown)(), int *exceptBlock)
{
    static int Flags ;
    static int rv;
    Flags = flags;
    if (flags & GUI)
        _win32 = 1;
    userRundown = rundown;
    if (!(flags & DLL) && !msvcrt_compat)
        __xceptinit(exceptBlock);

    if ((rv = setjmp(__abortbranch)) || (rv = setjmp(__exitbranch)))
    {
        // this is only safe because we aren't using the stack any more...
        rv--;
        if (!(Flags & DLL) && !msvcrt_compat)
        {
            userRundown();
            __xceptrundown();
        }
        __srproc(EXITSTART, EXITEND);
        ExitProcess(rv);
    }
    _osenv = GetEnvironmentStrings();
    _oscmd = GetCommandLine();		
    __hInstance = GetModuleHandle(0);
    _llfpinit();
    __srproc(INITSTART, INITEND);
}
void __export __getmainargs(int **pargc, char ***pargv, char ***penviron, int flags, void **newmode)
{
	msvcrt_compat = 1;
	___lsdllinit(0, 0, 0);
	*pargc = _argc;
	*pargv = _argv;
	*penviron = _environ;
	*newmode = 0;
}
