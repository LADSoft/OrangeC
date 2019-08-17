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

#ifndef _OPCODES_H
#define _OPCODES_H

#include "utype.h"

#define OP(x) (x)
#define NEED(x) (x)
#define VAL_ALL 0xff
#define VAL_NO8086 0x3f
#define VAL_VX 0x0c
#define VAL_286 0x03
#define VAL_386 0x01
#define VAL_8086 0x80
#define VAL_FLOAT 0x100
#define VAL_FLOAT386 0x200

#define OP_CODEONLY 0
#define OP_WREG02 1
#define OP_ACCREG02 2
#define OP_SEG35 3
#define OP_REGRMREG 4
#define OP_RM 5
#define OP_RMSHIFT 6
#define OP_REGRM 7
#define OP_WORDREGRM 8
#define OP_INTR 9
#define OP_SHORTBRANCH 10
#define OP_RMIMM 11
#define OP_ACCIMM 12
#define OP_ABSACC 13
#define OP_RMIMMSIGNED 14
#define OP_ACCIMMB3 15
#define OP_SEGRMSEG 16
#define OP_RET 17
#define OP_SEGBRANCH 18
#define OP_ESC 19
#define OP_BRANCH 20
#define OP_ACCDX 21
#define OP_DXACC 22
#define OP_PORTACCPORT 23
#define OP_ACCABS 24
#define OP_IMM 25
#define OP_ENTER 26
#define OP_INSWORDSIZE 27
#define OP_REGMOD 28
#define OP_MODIMM 29
#define OP_RMSHIFTB3 30
#define OP_IMUL 31
#define OP_386REG 32
#define OP_REGRMSHIFT 33
#define OP_NOSTRICTRM 34
#define OP_FLOATRM 35
#define OP_SIZEFLOATRM 36
#define OP_FLOATMATH 37
#define OP_FLOATNOPTR OP_NOSTRICTRM
#define OP_FARRM 38
#define OP_WORDRMREG 39
#define OP_RMREG 40
#define OP_BITNUM 41
#define OP_MIXEDREGRM 42
#define OP_REGRMBW 43

typedef struct _optable_
{
    uint mask;
    uint compare;
    char* mnemonic;
    BYTE operands;
    BYTE length;
    uint validator;
} OPTABLE;

#ifdef __cplusplus
extern "C"
{
#endif
    OPTABLE* FindOpcode(BYTE* stream, uint validator);
#ifdef __cplusplus
}
#endif
#endif /* _OPCODES_H */
