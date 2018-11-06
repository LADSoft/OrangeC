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
char BackendIntrinsicPrototypes[] = ""
"unsigned __fastcall rotateLeft(unsigned val, int count);"
"unsigned __fastcall rotateRight(unsigned val, int count);";

// for __fastcall, first arg is in ECX, second arg is in EDX and third arg is in EAX
// more args will be pushed on the stack, but if you do that you have to leave them there so they can get cleaned up properly.
//
// as always return a value in EAX
//
BOOLEAN BackendIntrinsic(QUAD *q)
{
    char *name = q->dc.left->offset->v.sp->name;
    if (!strcmp(name, "rotateLeft"))
    {
        AMODE *cl = makedreg(ECX);
        cl->length = ISZ_UCHAR;
        gen_code(op_xchg, makedreg(EAX), makedreg(EDX));
        gen_code(op_xchg, makedreg(EAX), makedreg(ECX));
        gen_code(op_rol, makedreg(EAX), cl);
        return TRUE;
    }
    else if (!strcmp(name, "rotateRight"))
    {
        AMODE *cl = makedreg(ECX);
        cl->length = ISZ_UCHAR;
        gen_code(op_xchg, makedreg(EAX), makedreg(EDX));
        gen_code(op_xchg, makedreg(EAX), makedreg(ECX));
        gen_code(op_ror, makedreg(EAX), cl);
        return TRUE;
    }
    return FALSE;
}
