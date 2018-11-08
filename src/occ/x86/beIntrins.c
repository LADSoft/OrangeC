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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "be.h"
typedef BOOLEAN (*BUILTIN)();
typedef struct builtins
{
    char* name;
    BUILTIN func;
} BUILTINS;
BOOLEAN handleROTL();
BOOLEAN handleROTR();
BOOLEAN handleCTZ();
BOOLEAN handleCLZ();
static BUILTINS builtins[] = {{"__builtin_clz", handleCLZ},  {"__builtin_clzl", handleCLZ}, {"__builtin_ctz", handleCTZ},
                              {"__builtin_ctzl", handleCTZ}, {"__rotl", handleROTL},        {"__rotr", handleROTR}};
char BackendIntrinsicPrototypes[] =
    "unsigned __fastcall __builtin_clz(unsigned val);"
    "unsigned __fastcall __builtin_clzl(unsigned long val);"
    "unsigned __fastcall __builtin_ctz(unsigned val);"
    "unsigned __fastcall __builtin_ctzl(unsigned long val);"
    "unsigned __fastcall __rotl(unsigned val, int count);"
    "unsigned __fastcall __rotr(unsigned val, int count);";
BOOLEAN handleROTL()
{
    AMODE* cl = makedreg(ECX);
    cl->length = ISZ_UCHAR;
    gen_code(op_xchg, makedreg(EAX), makedreg(EDX));
    gen_code(op_xchg, makedreg(EAX), makedreg(ECX));
    gen_code(op_rol, makedreg(EAX), cl);
    return TRUE;
}
BOOLEAN handleROTR()
{
    AMODE* cl = makedreg(ECX);
    cl->length = ISZ_UCHAR;
    gen_code(op_xchg, makedreg(EAX), makedreg(EDX));
    gen_code(op_xchg, makedreg(EAX), makedreg(ECX));
    gen_code(op_ror, makedreg(EAX), cl);
    return TRUE;
}
BOOLEAN handleCTZ()
{
    gen_code(op_bsf, makedreg(EAX), makedreg(ECX));
    return TRUE;
}
BOOLEAN handleCLZ()
{
    gen_code(op_bsr, makedreg(EAX), makedreg(ECX));
    gen_code(op_xor, makedreg(EAX), aimmed(31));
    return TRUE;
}
// for __fastcall, first arg is in ECX, second arg is in EDX and third arg is in EAX
// more args will be pushed on the stack, but if you do that you have to leave them there so they can get cleaned up properly.
//
// as always return a value in EAX
//
BOOLEAN BackendIntrinsic(QUAD* q)
{
    char* name = q->dc.left->offset->v.sp->name;
    for (int i = 0; i < ((sizeof(builtins) / sizeof(builtins[0])) - 1); i++)
    {
        if (!strcmp(name, builtins[i].name))
        {
            return builtins[i].func();
        }
    }
    return FALSE;
}
