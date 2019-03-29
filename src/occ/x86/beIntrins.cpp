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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unordered_map>
#include <string>
#include "be.h"

typedef bool (*BUILTIN)();
#define PROTO(PROT, NAME, FUNC) bool FUNC();
#include "beIntrinsicProtos.h"
#define PROTO(PROT, NAME, FUNC) {#NAME, FUNC},
static std::unordered_map<std::string, BUILTIN> builtins = {
#include "beIntrinsicProtos.h"
};
char BackendIntrinsicPrototypes[] =
#include "beIntrinsicProtos.h"
    ;

// This function moves the ZF flag to EAX, same method MSVC does, used for BSR and BSF from MSVC
// These do the same thing as CLZ and CTZ, but more direct
void promoteToBoolean(AMODE* addr)
{
    addr->length = ISZ_UCHAR;
    gen_code(op_setne, addr, NULL);
}
bool handleBSR()
{
    AMODE* al = makedreg(EAX);
    AMODE* ecx = makedreg(ECX);
    ecx->mode = am_indisp;
    gen_code(op_bsr, makedreg(EAX), makedreg(EDX));
    gen_code(op_mov, ecx, makedreg(EAX));
    promoteToBoolean(al);
    return true;
}
bool handleBSF()
{
    AMODE* al = makedreg(EAX);
    AMODE* ecx = makedreg(ECX);
    ecx->mode = am_indisp;
    gen_code(op_bsf, makedreg(EAX), makedreg(EDX));
    gen_code(op_mov, ecx, makedreg(EAX));
    promoteToBoolean(al);
    return true;
}
bool handleINB()
{
    AMODE* dx = makedreg(EDX);
    AMODE* al = makedregSZ(EAX, ISZ_UCHAR);
    gen_code(op_mov, dx, makedreg(ECX));
    gen_code(op_in, al, dx);
    gen_code(op_movzx, makedreg(EAX), al);
    return true;
}
bool handleINW()
{
    AMODE* dx = makedreg(EDX);
    AMODE* ax = makedregSZ(EAX, ISZ_USHORT);
    gen_code(op_mov, dx, makedreg(ECX));
    gen_code(op_in, ax, dx);
    gen_code(op_movzx, makedreg(EAX), ax);
    return true;
}
bool handleIND()
{
    AMODE* dx = makedreg(EDX);
    AMODE* eax = makedreg(EAX);
    gen_code(op_mov, dx, makedreg(ECX));
    gen_code(op_in, eax, dx);
    return true;
}
bool handleOUTB()
{
    AMODE* al = makedregSZ(EAX, ISZ_UCHAR);
    AMODE* dx = makedregSZ(EDX, ISZ_USHORT);
    gen_code(op_mov, al, makedreg(EDX));
    gen_code(op_mov, dx, makedreg(ECX));
    gen_code(op_out, dx, al);
    return true;
}
bool handleOUTW()
{
    AMODE* dx = makedregSZ(EDX, ISZ_USHORT);
    gen_code(op_mov, makedreg(EAX), makedreg(EDX));
    gen_code(op_mov, dx, makedreg(ECX));
    gen_code(op_out, dx, makedregSZ(EAX, ISZ_USHORT));
    return true;
}
bool handleOUTD()
{
    AMODE* eax = makedreg(EAX);
    AMODE* dx = makedreg(EDX);
    dx->length = ISZ_USHORT;
    gen_code(op_mov, eax, makedreg(EDX));
    gen_code(op_mov, dx, makedreg(ECX));
    gen_code(op_out, dx, eax);
    return true;
}
bool handleROTL8()
{
    AMODE* cl = makedregSZ(ECX, ISZ_UCHAR);
    AMODE* al = makedregSZ(EAX, ISZ_UCHAR);
    gen_code(op_mov, al, cl);
    gen_code(op_mov, cl, makedregSZ(EDX, ISZ_UCHAR));
    gen_code(op_rol, al, cl);
    return true;
}
bool handleROTL16()
{
    AMODE* cl = makedregSZ(ECX, ISZ_UCHAR);
    gen_code(op_movzx, makedreg(EAX), cl);
    gen_code(op_mov, cl, makedregSZ(EDX, ISZ_UCHAR));
    gen_code(op_rol, makedregSZ(EAX, ISZ_USHORT), cl);
    return true;
}
void handleROT64(bool);
bool handleROTL64()
{
    handleROT64(false);
    return true;
}
bool handleROTR8()
{
    AMODE* cl = makedregSZ(ECX, ISZ_UCHAR);
    AMODE* al = makedregSZ(EAX, ISZ_UCHAR);
    gen_code(op_mov, al, cl);
    gen_code(op_mov, cl, makedregSZ(EDX, ISZ_UCHAR));
    gen_code(op_ror, al, cl);
    return true;
}
bool handleROTR16()
{
    AMODE* cl = makedregSZ(ECX, ISZ_UCHAR);
    gen_code(op_movzx, makedreg(EAX), cl);
    gen_code(op_mov, cl, makedregSZ(EDX, ISZ_UCHAR));
    gen_code(op_ror, makedregSZ(EAX, ISZ_USHORT), cl);
    return true;
}
bool handleROTR64()
{

    handleROT64(true);
    return true;
}
void handleROT64(bool right)
{
    AMODE* eax = makedreg(EAX);
    AMODE* edx = makedreg(EDX);
    AMODE* ecx = makedreg(ECX);
    AMODE* ebx = makedreg(EBX);
    AMODE* esi = makedreg(ESI);
    /*
    MSVC does ROTR64 AND ROTL64 as:
    unsigned __int64 rotr(unsigned __int64,int) PROC                             ; rotr, COMDAT
        mov     edx, DWORD PTR _num$[esp]
        push    ebx
        mov     ebx, ecx
        push    esi
        mov     esi, DWORD PTR _num$[esp+4]
        test    bl, 32                              ; 00000020H
        je      SHORT $LN3@rotr
        mov     eax, esi
        mov     esi, edx
        mov     edx, eax
$LN3@rotr:
        mov     eax, esi
        and     bl, 31                                    ; 0000001fH
        je      SHORT $LN4@rotr
        mov     cl, bl
        shrd    eax, edx, cl
        shrd    edx, esi, cl
$LN4@rotr:
        pop     esi
        pop     ebx
        ret     8

        The only diff is between shrd and shld
        */
    gen_code(op_mov, edx, make_stack(0));
    gen_code(op_push, ebx, nullptr);
    gen_code(op_mov, ebx, ecx);
    gen_code(op_push, esi, nullptr);
    gen_code(op_mov, esi, make_stack(4));
    ebx->length = ISZ_UCHAR;
    gen_code(op_test, ebx, aimmed(32));
    int lab = beGetLabel;
    gen_branch(op_je, lab);
    gen_code(op_mov, eax, esi);
    gen_code(op_mov, esi, edx);
    gen_code(op_mov, edx, eax);
    ecx->length = ISZ_UCHAR;
    oa_gen_label(lab);
    int lab1 = beGetLabel;
    gen_code(op_mov, eax, esi);
    gen_code(op_and, ebx, aimmed(31));
    gen_branch(op_je, lab1);
    ecx->length = ISZ_UCHAR;
    gen_code(op_mov, ecx, ebx);
    auto op = right ? op_shrd : op_shld;
    gen_code3(op, eax, edx, ecx);
    gen_code3(op, edx, esi, ecx);
    oa_gen_label(lab1);
    ebx->length = ISZ_UINT;
    gen_code(op_pop, esi, nullptr);
    gen_code(op_pop, ebx, nullptr);
}
bool handleROTL()
{
    gen_code(op_mov, makedreg(EAX), makedreg(ECX));
    gen_code(op_mov, makedreg(ECX), makedreg(EDX));
    gen_code(op_rol, makedreg(EAX), makedregSZ(ECX, ISZ_UCHAR));
    return true;
}
bool handleROTR()
{
    gen_code(op_mov, makedreg(EAX), makedreg(EDX));
    gen_code(op_mov, makedreg(ECX), makedreg(ECX));
    gen_code(op_ror, makedreg(EAX), makedregSZ(ECX, ISZ_UCHAR));
    return true;
}
bool handleCTZ()
{
    gen_code(op_bsf, makedreg(EAX), makedreg(ECX));
    return true;
}
bool handleCLZ()
{
    gen_code(op_bsr, makedreg(EAX), makedreg(ECX));
    gen_code(op_xor, makedreg(EAX), aimmed(31));
    return true;
}
bool handleBSWAP16()
{
    gen_code(op_mov, makedreg(EAX), makedreg(ECX));
    AMODE* al = makedregSZ(EAX, ISZ_UCHAR);
    AMODE* ah = makedregSZ(EAX + 4, ISZ_UCHAR);
    gen_code(op_xchg, al, ah);
    return true;
}
bool handleBSWAP32()
{
    gen_code(op_mov, makedreg(EAX), makedreg(ECX));
    gen_code(op_bswap, makedreg(EAX), NULL);  // slow, but...
    return true;
}
bool handleBSWAP64()
{
    gen_code(op_mov, makedreg(EAX), makedreg(EDX));
    gen_code(op_mov, makedreg(EDX), makedreg(ECX));
    gen_code(op_bswap, makedreg(EAX), NULL);
    gen_code(op_bswap, makedreg(EDX), NULL);
    return true;
}
// for __fastcall, first arg is in ECX, second arg is in EDX and third arg is in EAX
// more args will be pushed on the stack, but if you do that you have to leave them there so they can get cleaned up properly.
//
// as always return a value in EAX
//
bool BackendIntrinsic(QUAD* q)
{
    const char* name = q->dc.left->offset->v.sp->name;
    auto v = builtins.find(name);
    if (v != builtins.end())
    {
        return v->second();
    }
    return false;
}
