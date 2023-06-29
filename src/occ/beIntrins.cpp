/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <unordered_map>
#include <functional>
#include "be.h"
#include "peep.h"
#include "gen.h"
#include "FNV_hash.h"
namespace occx86
{
namespace Util = OrangeC::Utils;
typedef bool (*BUILTIN)();

typedef struct builtins
{
    const char* name;
    BUILTIN func;
} BUILTINS;
#define PROTO(PROT, NAME, FUNC) bool FUNC();
#include "beIntrinsicProtos.h"
#define PROTO(PROT, NAME, FUNC) {#NAME, FUNC},
std::unordered_map<const char*, BUILTIN, Util::fnv1a64, Util::str_eql> builtin_map = {
#include "beIntrinsicProtos.h"
};

// This function moves the ZF flag to EAX, same method MSVC does, used for BSR and BSF from MSVC
// These do the same thing as CLZ and CTZ, but more direct
void promoteToBoolean(AMODE* addr)
{
    addr->length = ISZ_UCHAR;
    gen_code(op_setne, addr, nullptr);
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
bool handleROTL()
{
    gen_code(op_xchg, makedreg(EAX), makedreg(EDX));
    gen_code(op_xchg, makedreg(EAX), makedreg(ECX));
    gen_code(op_rol, makedreg(EAX), makedregSZ(ECX, ISZ_UCHAR));
    return true;
}
bool handleROTR()
{
    gen_code(op_xchg, makedreg(EAX), makedreg(EDX));
    gen_code(op_xchg, makedreg(EAX), makedreg(ECX));
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
    gen_code(op_bswap, makedreg(EAX), nullptr);  // slow, but...
    return true;
}
bool handleBSWAP64()
{
    gen_code(op_mov, makedreg(EAX), makedreg(EDX));
    gen_code(op_mov, makedreg(EDX), makedreg(ECX));
    gen_code(op_bswap, makedreg(EAX), nullptr);
    gen_code(op_bswap, makedreg(EDX), nullptr);
    return true;
}
bool handlePAUSE()
{
    gen_code(op_pause, nullptr, nullptr);
    return true;
}
// for __fastcall, first arg is in ECX, second arg is in EDX and third arg is in EAX
// more args will be pushed on the stack, but if you do that you have to leave them there so they can get cleaned up properly.
//
// as always return a value in EAX
//
bool BackendIntrinsic(Optimizer::QUAD* q)
{
    const char* name = q->dc.left->offset->sp->name;
    auto thing = builtin_map.find(name);
    if (thing != builtin_map.end())
    {
        return thing->second();
    }
    return false;
}
}  // namespace occx86