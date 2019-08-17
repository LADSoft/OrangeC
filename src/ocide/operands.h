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

#ifndef _OPERANDS_H
#define _OPERANDS_H

#include "opcodes.h"

#define TAB_ARGPOS 10

#define REG_eAX 0
#define REG_DX 2

#define OM_FSTREG 18
#define OM_CRX 17
#define OM_DRX 16
#define OM_TRX 15
#define OM_SUD 13
#define OM_PORT 12
#define OM_INT 11
#define OM_SHIFT 10
#define OM_RETURN 9
#define OM_SHORTBRANCH 8
#define OM_LONGBRANCH 7
#define OM_FARBRANCH 6
#define OM_ABSOLUTE 5
#define OM_IMMEDIATE 4
#define OM_REG 3
#define OM_SEGMENT 2
#define OM_BASED 1

/* Generic */
#define OMF_ADR32 0x01
#define OMF_OP32 0x02
#define OMF_BYTE 0x04
#define OMF_FST 0x08
#define OMF_FSTTAB 0x10
#define OM_FTAB 5
#define OMF_BASE 0x100
/* base & immediate specific */
#define OMF_SCALED OMF_BASE
#define OMF_SIGNED_OFFSET (OMF_BASE << 1)
#define OMF_WORD_OFFSET (OMF_BASE << 2)
#define OMF_OFFSET (OMF_SIGNED_OFFSET | OMF_WORD_OFFSET)
/* shift specific */
#define OMF_CL OMF_BASE
/* Port specific */
#define OMF_DX OMF_BASE
/* Immediate specific */
#define OMF_SIGNED OMF_BASE

#define SG_ES 1
#define SG_CS 2
#define SG_SS 4
#define SG_DS 8
#define SG_FS 16
#define SG_GS 32
#define SG_OPSIZ 64
#define SG_ADRSIZ 128
#define SG_REPNZ 256
#define SG_REPZ 512
#define SG_REPNC 1024
#define SG_REPC 2048
#define SG_TWOBYTEOP 4096

#define SY_SIGNEDOFS 1
#define SY_WORDOFS 2
#define SY_BYTEOFS 3
#define SY_ABSOLUTE 4
#define SY_SIGNEDIMM 5
#define SY_WORDIMM 6
#define SY_BYTEIMM 7
#define SY_PORT 8
#define SY_INTR 9
#define SY_RETURN 10
#define SY_ABSBRANCH 11
#define SY_LONGBRANCH 12
#define SY_SHORTBRANCH 13
#define SY_SHIFT 14
#define SY_SEGMENT 15

#define MOD_NOOFS 0
#define MOD_SIGNED 1
#define MOD_ADDR 2
#define MOD_REG 3

#define RM_16_ABSOLUTE 6
#define RM_32_ABSOLUTE 5
#define RM_32_SCALED 4
#define RM_32_STACKINDEX 4

#define MOD(x) (*(((BYTE*)x) + 1) >> 6)
#define REG(x) ((*(((BYTE*)x) + 1) >> 3) & 7)
#define RM(x) (*(((BYTE*)x) + 1) & 7)
#define LONG(x) (*((long*)(x)))
#define UINT(x) (*((uint*)(x)))
#define SIGNEDOFS(x) (*((char*)x))
#define B0(x) ((x)&1)
#define B1(x) (((x) >> 1) & 1)
#define B2(x) (((x) >> 2) & 1)
#define B3(x) (((x) >> 3) & 1)
#define B4(x) (((x) >> 4) & 1)
#define B5(x) (((x) >> 5) & 1)
#define B6(x) (((x) >> 6) & 1)
#define B01(x) ((x)&3)
#define B12(x) (((x) >> 1) & 3)
#define B02(x) ((x)&0x07)
#define B35(x) (((x) >> 3) & 0x07)

typedef struct _operand_
{

    uint flags;
    uint override;
    uint segment;
    BYTE code;
    unsigned int reg : 3;
    unsigned int scalereg : 3;
    unsigned int scale : 2;
    long address;
} OPERAND;

#ifdef __cplusplus
extern "C"
{
#endif
    char* TabTo(char* buffer, int pos);

    BYTE* ReadOverrides(BYTE* stream, uint validator);
    BYTE* DispatchOperand(BYTE* stream, OPTABLE* opcode, BOOL use32);
    void FormatDissassembly(char* buffer);
    void RegisterSymbols(void);

    void SegmentDisplay(char* buffer, long value);
    void FormatValue(char* buffer, OPERAND* record, uint segs, uint type);
    void AddSymbol(OPERAND* record, uint type);
#ifdef __cplusplus
}
#endif
#endif /* _OPERANDS_H */
