/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

#include "c_types.h"
typedef void(__stdcall* sym_callback_t)(char* symname, char* filename, int linestart, int lineend, type_t* typeinfo,
                                        void* userdata);
typedef int(__stdcall* file_callback_t)(char* filename, int sys_inc, void** filefunc, void** filearg, unsigned char* linesused,
                                        int lines);

typedef void*(__stdcall* alloc_callback_t)(void* ptr_or_size, int alloc);

#ifndef DLLMAIN
void WINAPI _import parse(char* filename, file_callback_t fcallback, alloc_callback_t acallback, char* sysinc, char* userinc,
                          int defineCount, char* defines[], sym_callback_t callback, void* userdata);
int WINAPI _import enumTypes(type_t* type, sym_callback_t usercallback, void* userdata);
#endif
