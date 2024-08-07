/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

/*  setjmp.h

    Defines typedef and functions for setjmp/longjmp.

*/

#ifndef __SETJMP_H
#define __SETJMP_H

#pragma pack(1)

#ifndef __STDDEF_H
#    include <stddef.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif
#ifndef RC_INVOKED

    typedef struct __jmp_buf
    {
#ifdef _i386_
        unsigned j_eax;
        unsigned j_ecx;
        unsigned j_edx;
        unsigned j_ebx;
        unsigned j_esp;
        unsigned j_ebp;
        unsigned j_esi;
        unsigned j_edi;
        unsigned j_flag;
        unsigned j_pc;
#else
    unsigned j_d0;
    unsigned j_d1;
    unsigned j_d2;
    unsigned j_d3;
    unsigned j_d4;
    unsigned j_d5;
    unsigned j_d6;
    unsigned j_d7;
    unsigned j_a0;
    unsigned j_a1;
    unsigned j_a2;
    unsigned j_a3;
    unsigned j_a4;
    unsigned j_a5;
    unsigned j_a6;
    unsigned j_a7;
    unsigned j_flag;
    unsigned j_pc;
#endif

    } jmp_buf[1];

    void _RTL_FUNC _IMPORT longjmp(jmp_buf __jmpb, int __retval);
    int _RTL_FUNC _IMPORT setjmp(jmp_buf __jmpb);
    int _RTL_FUNC _IMPORT _setjmp(jmp_buf __jmpb);
#endif

#define setjmp(__jmpb) setjmp(__jmpb)
#define _setjmp(__jmpb) setjmp(__jmpb);

#ifdef __cplusplus
};
#endif

#pragma pack()

#endif /* __SETJMP_H */