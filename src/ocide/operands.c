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

#include "opcodes.h"
#include "operands.h"
#include <string.h>
#include <stdio.h>

#define SZ_QWORD 2
#define SZ_TBYTE 4

long code_address;
static BOOL strict;
static uint segs, total_prefixes;

static char mnemonic[10];
static OPERAND extraoperand, source, dest;

static char* based[8] = {"bx+si", "bx+di", "bp+si", "bp+di", "si", "di", "bp", "bx"};
static char* st[8] = {"dword", "dword", "qword", "word", "tbyte"};

static void SetSeg(OPERAND* record, uint seg)
{
    strict = FALSE;
    record->code = OM_SEGMENT;
    record->reg = seg;
}

//-------------------------------------------------------------------------

static void SetReg(OPERAND* record, uint reg)
{
    strict = FALSE;
    record->code = OM_REG;
    record->reg = reg;
}

//-------------------------------------------------------------------------

uint ReadRM(BYTE* stream, OPERAND* record)
{
    int mod, ofs = 2;

    /* Init the register or base index */
    record->reg = RM(stream);

    /* if MOD = 3 it's a register, we get out */
    if ((mod = MOD(stream)) == MOD_REG)
    {
        record->code = OM_REG;
        strict = FALSE;
        return (0);
    }
    if (record->flags & OMF_ADR32)
        /* If is scaled op */
        if (record->reg == RM_32_SCALED)
        {
            /* bump the index ptr and get the SIB ops */
            ofs++;
            record->reg = RM(stream + 1);
            record->scalereg = REG(stream + 1);
            record->scale = MOD(stream + 1);
            /* Can only scale if scale reg not SP */
            if (!(record->scalereg == RM_32_STACKINDEX))
                record->flags |= OMF_SCALED;
            /* now see if it is absolute scaled */
            if ((record->reg == RM_32_ABSOLUTE) && (mod == MOD_NOOFS))
            {
                record->code = OM_ABSOLUTE;
                record->address = LONG(stream + 3);
                return (5);
            }
        }
        else
        {
            if ((mod == MOD_NOOFS) && (record->reg == RM_32_ABSOLUTE))
            {
                record->code = OM_ABSOLUTE;
                record->address = LONG(stream + 2);
                return (4);
            }
        }
    else if ((mod == MOD_NOOFS) && (record->reg == RM_16_ABSOLUTE))
    {
        record->code = OM_ABSOLUTE;
        record->address = UINT(stream + 2);
        return (2);
    }

    /* If we get this far we are based and possibly scaled */
    record->code = OM_BASED;

    if (mod == MOD_ADDR)
    {
        record->flags |= OMF_WORD_OFFSET;
        if (record->flags & OMF_ADR32)
        {
            record->address = LONG(stream + ofs);
            ofs += 4;
        }
        else
        {
            record->address = UINT(stream + ofs);
            ofs += 2;
        }
    }
    else if (mod == MOD_SIGNED)
    {
        record->flags |= OMF_SIGNED_OFFSET;
        record->address = SIGNEDOFS(stream + ofs);
        ofs++;
    }
    return (ofs - 2);
}

//-------------------------------------------------------------------------

uint RegRM(BYTE* stream, OPERAND* reg, OPERAND* rm)
{
    SetReg(reg, REG(stream));
    return (ReadRM(stream, rm));
}

//-------------------------------------------------------------------------

uint Immediate(BYTE* stream, OPERAND* imm)
{
    int ofs = 0;
    imm->code = OM_IMMEDIATE;
    if (imm->flags & OMF_BYTE)
        imm->address = *(stream + (ofs++));
    else if (imm->flags & OMF_OP32)
    {
        imm->address = LONG(stream + ofs);
        ofs += 4;
    }
    else
    {
        imm->address = UINT(stream + ofs);
        ofs += 2;
    }
    return (ofs);
}

//-------------------------------------------------------------------------

static void MnemonicChar(char theChar)
{
    int pos = strlen(mnemonic);
    mnemonic[pos + 1] = '\0';
    mnemonic[pos] = theChar;
}

/* op 1- word reg from bits 0 - 2 of opcode */
static uint OP1(BYTE* stream, OPERAND* dest)
{
    dest->flags &= ~OMF_BYTE;
    SetReg(dest, B02(*stream));
    return (0);
}

/* OP1 acc, reg bits 0-2 of opcode */
static uint OP2(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    source->flags = dest->flags = 0;
    SetReg(dest, REG_eAX);
    SetReg(source, B02(*stream));
    return (0);
}

/* op3 - seg from b3-5 of opcode */
static uint OP3(BYTE* stream, OPERAND* dest)
{
    SetSeg(dest, B35(*stream));
    return (0);
}

/* op4 - REGRM with b1 of opcode set reg is dest else source */
static uint OP4(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    if (B1(*stream))
        return (RegRM(stream, dest, source));
    /* Reg is dest */
    return (RegRM(stream, source, dest)); /* Reg is source */
}

/* op5 - use RM only */
static uint OP5(BYTE* stream, OPERAND* dest) { return (ReadRM(stream, dest)); }

/* op6 READRM for shift */
static uint OP6(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    int ofs = ReadRM(stream, dest);
    source->code = OM_SHIFT;
    if (B4(*stream))
        if (B1(*stream))
            source->flags |= OMF_CL;
        else
            source->address = 1;
    else
        source->address = *(stream + 2 + (ofs++));

    return (ofs);
}

/* op 7 regrm with reg dest */
static uint OP7(BYTE* stream, OPERAND* dest, OPERAND* source) { return (RegRM(stream, dest, source)); }

/* OP8 - word regrm with reg dest */
static uint OP8(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    dest->flags &= ~OMF_BYTE;
    source->flags &= ~OMF_BYTE;
    return (RegRM(stream, dest, source));
}

/* op 9 - interrupts */
static uint OP9(BYTE* stream, OPERAND* dest)
{
    strict = FALSE;
    dest->code = OM_INT;
    if (B0(*stream))
    {
        dest->address = *(stream + 1);
        return (0);
    }

    dest->address = 3;
    return (0);
}

/* op 10, short relative branch */
static uint OP10(BYTE* stream, OPERAND* dest)
{
    strict = FALSE;
    dest->code = OM_SHORTBRANCH;
    dest->address = code_address + SIGNEDOFS(stream + 1) + 2 + total_prefixes;
    if (!(dest->flags & OMF_OP32))
        dest->address = dest->address & 0xffff;
    return (0);
}

/* op 11 RM, immediate */
static uint OP11(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    int ofs = ReadRM(stream, dest);
    ofs += Immediate(stream + ofs + 2, source);
    return (ofs);
}

/* op 12 - acc, immediate */
static uint OP12(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    int ofs;
    SetReg(dest, REG_eAX);
    ofs = Immediate(stream + 1, source);
    return (ofs);
}

/* op 13 absolute, acc*/
static uint OP13(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    int ofs = 0;
    dest->code = OM_ABSOLUTE;
    if (dest->flags & OMF_ADR32)
    {
        dest->address = LONG(stream + 1);
        ofs += 2;
    }
    else
        dest->address = UINT(stream + 1);
    SetReg(source, REG_eAX);
    return (ofs);
}

/* op 14 - RM, immediate, b01 of opcode != 1 for byte */
static uint OP14(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    int ofs = ReadRM(stream, dest);

    if (B01(*stream) == 0)
    {
        source->flags |= OMF_BYTE;
        dest->flags |= OMF_BYTE;
    }
    else
    {
        if (B01(*stream) == 1)
            source->flags &= ~OMF_BYTE;
        else
            source->flags |= OMF_BYTE;
        dest->flags &= ~OMF_BYTE;
    }
    ofs += Immediate(stream + 2 + ofs, source);
    if (((*stream & 3) == 3) && (source->flags & OMF_BYTE))
    {
        source->flags |= OMF_SIGNED;
        source->address = (long)((char)source->address);
    }
    return (ofs);
}

/* op 15 - acc, immediate, B3 of opcode clear for byte */
static uint OP15(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    SetReg(dest, B02(*stream)); /* Not using OPSIZ */
    if (B3(*stream))
    {
        source->flags &= ~OMF_BYTE;
        dest->flags &= ~OMF_BYTE;
    }
    else
    {
        source->flags |= OMF_BYTE;
        dest->flags |= OMF_BYTE;
    }

    return (Immediate(stream + 1, source));
}

/* op 16 - seg,readrm, if B1 of opcode seg is dest else source */
static uint OP16(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    int ofs;
    if (B1(*stream))
    {
        SetSeg(dest, REG(stream));
        source->flags &= ~OMF_BYTE;
        ofs = ReadRM(stream, source);
    }
    else
    {
        dest->flags &= ~OMF_BYTE;
        ofs = ReadRM(stream, dest);
        SetSeg(source, REG(stream));
    }
    return (ofs);
}

/* op 17, far return */
static uint OP17(BYTE* stream, OPERAND* dest)
{
    strict = FALSE;
    dest->code = OM_RETURN;
    dest->flags &= ~(OMF_ADR32 | OMF_OP32 | OMF_BYTE);
    dest->address = UINT(stream + 1);
    return (0);
}

/* op 18, far branch/call */
static uint OP18(BYTE* stream, OPERAND* dest)
{
    int ofs = 0;
    strict = FALSE;
    dest->code = OM_FARBRANCH;
    dest->flags &= ~OMF_BYTE;
    if (dest->flags & OMF_OP32)
    {
        dest->address = LONG(stream + 1);
        ofs += 2;
    }
    else
        dest->address = UINT(stream + 1);
    dest->segment = UINT(stream + ofs + 3);
    return (ofs);
}

/* op 19 - ESC, mnem of bits 0-2 of opcode, imm,readrm */
static uint OP19(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    dest->code = OM_IMMEDIATE;
    dest->flags |= OMF_BYTE;
    dest->address = ((*(stream)&7) << 3) + ((*(stream + 1) >> 3) & 7);

    /* Note that byte mode and ESC code use the same bit... */
    return (ReadRM(stream, source));
}

/* op 20 - long branch */
static uint OP20(BYTE* stream, OPERAND* dest)
{
    strict = FALSE;
    dest->code = OM_LONGBRANCH;
    if (dest->flags & OMF_OP32)
    {
        dest->address = code_address + LONG(stream + 1) + 5 + total_prefixes;
        return (2);
    }
    else
    {
        dest->address = code_address + UINT(stream + 1) + 3 + total_prefixes;
        dest->address = dest->address & 0xffff;
    }
    return (0);
}

/* op21 acc,dx */
static uint OP21(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    stream++;
    SetReg(dest, REG_eAX);
    source->flags &= ~(OMF_OP32 | OMF_BYTE);
    SetReg(source, REG_DX);
    return (0);
}

/* op22 - dx,acc */
static uint OP22(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    stream++;
    dest->flags &= ~(OMF_OP32 | OMF_BYTE);
    SetReg(dest, REG_DX);
    SetReg(source, REG_eAX);
    return (0);
}

/* op23 - port,acc where B1 of opcode set is port dest */
static uint OP23(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    if (B1(*stream))
    {
        dest->flags |= OMF_BYTE;
        dest->code = OM_PORT;
        dest->address = *(stream + 1);
        SetReg(source, REG_eAX);
    }
    else
    {
        SetReg(dest, REG_eAX);
        source->flags |= OMF_BYTE;
        source->code = OM_PORT;
        source->address = *(stream + 1);
    }

    return (0);
}

/* op 24 acc, absolute */
static uint OP24(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    int ofs = 0;
    SetReg(dest, REG_eAX);
    source->code = OM_ABSOLUTE;
    if (source->flags & OMF_ADR32)
    {
        source->address = LONG(stream + 1);
        ofs += 2;
    }
    else
        source->address = UINT(stream + 1);
    return (ofs);
}

/* op 25 - immediate byte or word */
static uint OP25(BYTE* stream, OPERAND* dest)
{
    strict = FALSE;
    if (B1(*stream))
        dest->flags |= OMF_BYTE;
    else
        dest->flags &= ~OMF_BYTE;

    return (Immediate(stream + 1, dest));
}

/* op 26, immediate 2byte,byte */
static uint OP26(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    strict = FALSE;
    dest->flags &= ~(OMF_BYTE | OMF_OP32);
    Immediate(stream + 1, dest);
    source->flags |= OMF_BYTE;
    source->flags &= ~OMF_OP32;
    Immediate(stream + 3, source);
    return (0);
}

/* op 27 - string */
static uint OP27(BYTE* stream, OPERAND* dest)
{
    stream++;
    if (segs & SG_OPSIZ)
    {
        if (dest->flags & OMF_OP32)
            MnemonicChar('d');
        else
            MnemonicChar('w');
    }
    return (0);
}

/* op 28 - source = REG, dest = RM */
static uint OP28(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    SetReg(dest, REG(stream));
    SetReg(source, RM(stream));
    return (0);
}

/* op 29 - dest = RM, immediate */
static uint OP29(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    dest->flags |= OMF_BYTE;
    SetReg(dest, RM(stream));
    source->flags |= OMF_BYTE;
    Immediate(stream + 2, source);
    return (0);
}

/* op30 - RM, shift with B3 of stream selecting COUNT or CL*/
static uint OP30(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    int ofs = ReadRM(stream, dest);

    source->code = OM_SHIFT;
    if (B3(*stream))
        /* The original author didn't let the RM field have any offsets... */
        source->address = *(stream + (ofs++) + 2);
    else
        source->flags |= OMF_CL;

    return (ofs);
}

/* op 31- reg, rm, count wher B1 of opcode = byte/word */
static uint OP31(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    int ofs;
    extraoperand = *dest;

    SetReg(dest, REG(stream));

    ofs = ReadRM(stream, source);
    if (B1(*stream))
        extraoperand.flags |= OMF_BYTE;
    else
        extraoperand.flags &= ~OMF_BYTE;

    ofs += Immediate(stream + 2, &extraoperand);
    return (ofs);
}

/* op32 - 386 special regs */
static uint OP32(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    uint id = *((uint*)(stream)) & 0xc005;
    if (id == 0xc000)
        id = OM_CRX;
    else if (id == 0xc001)
        id = OM_DRX;
    else if (id == 0xc004)
        id = OM_TRX;
    else
        id = OM_SUD;

    dest->flags &= ~OMF_BYTE;
    source->flags &= ~OMF_BYTE;
    source->flags |= OMF_OP32;
    dest->flags |= OMF_OP32;
    if (B1(*stream))
    {
        dest->code = id;
        dest->reg = REG(stream);
        SetReg(source, RM(stream));
    }
    else
    {
        SetReg(dest, RM(stream));
        source->code = id;
        source->reg = REG(stream);
    }
    return (0);
}

/* op33 - reg,rm,shiftcnt where B3 = reg source, b0 = shift cl */
static uint OP33(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    int ofs;
    dest->flags &= ~OMF_BYTE;
    source->flags &= ~OMF_BYTE;
    extraoperand = *dest;
    extraoperand.code |= OM_SHIFT;
    ofs = ReadRM(stream, dest);
    SetReg(source, REG(stream));
    if (B0(*stream))
        extraoperand.flags |= OMF_CL;
    else
        extraoperand.address = *(stream + 2 + ofs);

    return (ofs);
}

/* op 34 - push & pop word */
static uint OP34(BYTE* stream, OPERAND* dest)
{
    if ((segs & SG_TWOBYTEOP) || !(segs & SG_OPSIZ))
        strict = FALSE;
    return (ReadRM(stream, dest));
}

/* op 35 -floating RM */
static uint OP35(BYTE* stream, OPERAND* dest)
{
    strict = FALSE;
    if ((*((uint*)stream) & 0xd0de) == 0xd0de)
        MnemonicChar('p');
    if (MOD(stream) != 3)
        dest->flags |= OMF_FSTTAB | (B12(*stream) << OM_FTAB);
    else
        dest->flags |= OMF_FST;
    return (ReadRM(stream, dest));
}

/* op 36 - sized floating RM */
static uint OP36(BYTE* stream, OPERAND* dest)
{
    int size = SZ_QWORD;
    strict = FALSE;

    if ((*((uint*)stream) & 0x2807) != 0x2807)
        size = SZ_TBYTE;

    dest->flags |= OMF_FSTTAB | ((size) << OM_FTAB);
    return (ReadRM(stream, dest));
}

/* OP 37 - floating MATH */
static uint OP37(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    int ofs = 0, flop = 0;
    strict = FALSE;
    if ((*((uint*)stream) & 0xc0de) == 0xc0de)
        flop = 1;
    if (((REG(stream) & 5) ^ flop) == 5)
        MnemonicChar('r');

    if (MOD(stream) != 3)
    {
        ofs = ReadRM(stream, dest);
        dest->flags |= OMF_FSTTAB | (B12(*stream) << OM_FTAB);
    }
    else
    {
        if (*stream & 6)
            MnemonicChar('p');
        if (*stream & 4)
        {
            SetReg(dest, RM(stream));
            dest->flags |= OMF_FST;
            source->code = OM_FSTREG;
        }
        else
        {
            dest->code = OM_FSTREG;
            SetReg(source, RM(stream));
            source->flags |= OMF_FST;
        }
    }
    return (ofs);
}

//-------------------------------------------------------------------------

static uint OP38(BYTE* stream, OPERAND* dest)
{
    /* This is how we get a DWORD PTR for the far jump */
    strict = FALSE;
    dest->flags |= OMF_FSTTAB;
    return (ReadRM(stream, dest));
}

/* OP39 - word regrm with reg source */
static uint OP39(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    dest->flags &= ~OMF_BYTE;
    source->flags &= ~OMF_BYTE;
    return (RegRM(stream, source, dest));
}

/* op 40 regrm with reg source */
static uint OP40(BYTE* stream, OPERAND* dest, OPERAND* source) { return (RegRM(stream, source, dest)); }

/* op 41 reg, bitnum */
static uint OP41(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    int ofs;
    dest->flags &= ~OMF_BYTE;
    ofs = ReadRM(stream, dest);
    source->flags |= OMF_BYTE;
    Immediate(stream + 2 + ofs, source);
    return (ofs);
}

/* op 42 mixed regrm with reg dest & strictness enforced */
static uint OP42(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    int ofs = RegRM(stream, dest, source);
    dest->flags &= ~OMF_BYTE;
    source->flags &= ~OMF_OP32;
    strict = TRUE;
    return (ofs);
}

/* op43 - REGRM with b0 of opcode set reg is word else byte */
static uint OP43(BYTE* stream, OPERAND* dest, OPERAND* source)
{
    if (!B0(*stream))
    {
        dest->flags |= OMF_BYTE;
        source->flags |= OMF_BYTE;
    }
    return (RegRM(stream, dest, source)); /* Reg is dest */
}

//-------------------------------------------------------------------------

static FUNC optable[44] = {0,    OP1,  OP2,  OP3,  OP4,  OP5,  OP6,  OP7,  OP8,  OP9,  OP10, OP11, OP12, OP13, OP14,
                           OP15, OP16, OP17, OP18, OP19, OP20, OP21, OP22, OP23, OP24, OP25, OP26, OP27, OP28, OP29,
                           OP30, OP31, OP32, OP33, OP34, OP35, OP36, OP37, OP38, OP39, OP40, OP41, OP42, OP43};

BYTE* ReadOverrides(BYTE* stream, uint validator)
{
    segs = 0;
    total_prefixes = 0;

    for (;; stream++)
    {
        if (validator & VAL_VX)
        {
            if ((uint)(*stream - 0x64) < 2)
            {
                segs |= SG_REPNC << (*stream - 0x64);
                total_prefixes++;
                continue;
            }
        }
        if (validator & VAL_386)
        {
            if ((uint)(*stream - 0x64) < 4)
            {
                segs |= SG_FS << (*stream - 0x64);
                total_prefixes++;
                continue;
            }
        }
        if ((*stream & 0xe7) == 0x26)
        {
            segs |= 1 << ((*stream >> 3) & 3);
            total_prefixes++;
            continue;
        }
        if ((uint)(*stream - 0xf2) < 2)
        {
            segs |= SG_REPNZ << (*stream - 0xf2);
            total_prefixes++;
            continue;
        }
        break;
    }
    return (stream);
}

//-------------------------------------------------------------------------

BYTE* DispatchOperand(BYTE* stream, OPTABLE* opcode, BOOL adr32)
{
    BOOL op32 = adr32;
    strcpy(mnemonic, opcode->mnemonic);
    strict = TRUE;
    source.code = 0;
    dest.code = 0;

    if (*stream == 0x0f)
    {
        segs |= SG_TWOBYTEOP;
        total_prefixes++;
        stream++;
    }
    if (segs & SG_ADRSIZ)
        adr32 = !adr32;

    if (segs & SG_OPSIZ)
        op32 = !op32;

    if (*stream & 1)
    {
        source.flags = 0;
        dest.flags = 0;
    }
    else
    {
        source.flags = OMF_BYTE;
        dest.flags = OMF_BYTE;
    }
    extraoperand.code = 0;

    if (adr32)
    {
        source.flags |= OMF_ADR32;
        dest.flags |= OMF_ADR32;
    }
    if (op32)
    {
        source.flags |= OMF_OP32;
        dest.flags |= OMF_OP32;
    }
    if (opcode->operands)
        stream += (*optable[opcode->operands])(stream, &dest, &source);
    stream += opcode->length;
    return (stream);
}

//-------------------------------------------------------------------------

static char* DoStrict(char* buffer, OPERAND* record)
{
    if (strict)
        if (record->flags & OMF_BYTE)
            strcat(buffer, "byte ptr ");
        else if (record->flags & OMF_OP32)
            strcat(buffer, "dword ptr ");
        else
            strcat(buffer, "word ptr ");
    else if (record->flags & OMF_FSTTAB)
        sprintf(buffer, "%s ptr ", st[(record->flags >> OM_FTAB) & 7]);
    return (buffer + strlen(buffer));
}

//-------------------------------------------------------------------------

char* TabTo(char* buffer, int pos)
{
    int temp = pos - strlen(buffer);
    buffer += pos - temp;
    do
    {
        *buffer++ = ' ';
        *buffer = '\0';
    } while (--temp > 0);
    return (buffer);
}

//-------------------------------------------------------------------------

static char* GetST(char* buffer, OPERAND* record)
{
    sprintf(buffer, "st(%x)", record->reg);
    return (buffer + strlen(buffer));
}

//-------------------------------------------------------------------------

static char* GetStdReg(char* buffer, int reg, BOOL use32)
{
    char* pos;

    if (use32)
    {
        *buffer++ = 'e';
        reg |= 8;
    }
    pos = &"alcldlblahchdhbhaxcxdxbxspbpsidi"[reg * 2];
    *buffer++ = *pos++;
    *buffer++ = *pos;
    *buffer = '\0';
    return (buffer);
}

//-------------------------------------------------------------------------

static char* GetReg(char* buffer, OPERAND* record, int reg)
{
    BOOL use32 = ((record->flags & (OMF_OP32 | OMF_BYTE)) == OMF_OP32);
    if (!(record->flags & OMF_BYTE))
        reg |= 8;
    return (GetStdReg(buffer, reg, use32));
}

//-------------------------------------------------------------------------

static char* GetSpecial(char* buffer, OPERAND* record, char* name)
{
    *buffer++ = *name++;
    *buffer++ = *name++;
    *buffer++ = name[record->reg];
    *buffer = '\0';
    return (buffer);
}

//-------------------------------------------------------------------------

static char* GetSeg(char* buffer, int seg, BOOL override)
{
    char* pos = &"escsssdsfsgs"[seg * 2];
    *buffer++ = *pos++;
    *buffer++ = *pos++;
    if (override)
        *buffer++ = ':';
    *buffer = '\0';
    return (buffer);
}

//-------------------------------------------------------------------------

static char* SegOverride(char* buffer)
{
    if (segs & SG_ES)
        buffer = GetSeg(buffer, 0, TRUE);
    if (segs & SG_CS)
        buffer = GetSeg(buffer, 1, TRUE);
    if (segs & SG_SS)
        buffer = GetSeg(buffer, 2, TRUE);
    if (segs & SG_DS)
        buffer = GetSeg(buffer, 3, TRUE);
    if (segs & SG_FS)
        buffer = GetSeg(buffer, 4, TRUE);
    if (segs & SG_GS)
        buffer = GetSeg(buffer, 5, TRUE);
    segs = 0;
    return (buffer);
}

//-------------------------------------------------------------------------

static void Scaled(char* buffer, OPERAND* record, BOOL based)
{
    uint flags = record->flags;
    record->flags &= ~OMF_BYTE;
    record->flags |= OMF_OP32;
    if (based)
        buffer = GetStdReg(buffer, record->reg, TRUE);
    if (record->flags & OMF_SCALED)
    {
        char* pos = &" + +2*+4*+8*"[record->scale * 3];
        *buffer++ = *pos++;
        *buffer++ = *pos++;
        *buffer++ = *pos++;
        buffer = GetStdReg(buffer, record->scalereg, TRUE);
    }
    record->flags = flags;
}

//-------------------------------------------------------------------------

static void PutOperand(char* buffer, OPERAND* record)
{
    buffer += strlen(buffer);
    switch (record->code)
    {
        case OM_FSTREG:
            strcat(buffer, "st");
            break;
        case OM_CRX:
            GetSpecial(buffer, record, "CR0?23????");
            break;
        case OM_DRX:
            GetSpecial(buffer, record, "DR0123??67");
            break;
        case OM_TRX:
            GetSpecial(buffer, record, "TR??????67");
            break;
        case OM_SUD:
            GetSpecial(buffer, record, "?R????????");
            break;
        case OM_PORT:
            FormatValue(buffer, record, segs, SY_PORT);
            break;
        case OM_INT:
            FormatValue(buffer, record, segs, SY_INTR);
            break;
        case OM_SHIFT:
            if (record->flags & OMF_CL)
                strcpy(buffer, "cl");
            else if (record->address == 1)
                strcpy(buffer, "1");
            else
                FormatValue(buffer, record, segs, SY_SHIFT);
            break;
        case OM_RETURN:
            FormatValue(buffer, record, segs, SY_RETURN);
            break;
        case OM_SHORTBRANCH:
            FormatValue(buffer, record, segs, SY_SHORTBRANCH);
            break;
        case OM_LONGBRANCH:
            FormatValue(buffer, record, segs, SY_LONGBRANCH);
            break;
        case OM_FARBRANCH:
            FormatValue(buffer, record, segs, SY_SEGMENT);
            strcat(buffer, ":");
            while (*buffer)
                buffer++;
            FormatValue(buffer, record, segs, SY_ABSBRANCH);
            break;
        case OM_ABSOLUTE:
            buffer = DoStrict(buffer, record);
            buffer = SegOverride(buffer);
            *buffer++ = '[';
            *buffer = '\0';
            if (record->flags & OMF_SCALED)
            {
                FormatValue(buffer, record, segs, SY_WORDOFS);
                while (*buffer)
                    buffer++;
                Scaled(buffer, record, FALSE);
            }
            else
                FormatValue(buffer, record, segs, SY_ABSOLUTE);
            strcat(buffer, "]");
            break;
        case OM_IMMEDIATE:
            if (record->flags & OMF_BYTE)
                if (record->flags & OMF_SIGNED)
                    FormatValue(buffer, record, segs, SY_SIGNEDIMM);
                else
                    FormatValue(buffer, record, segs, SY_BYTEIMM);
            else
                FormatValue(buffer, record, segs, SY_WORDIMM);
            break;
        case OM_REG:
            if (record->flags & OMF_FST)
                buffer = GetST(buffer, record);
            else
                buffer = GetReg(buffer, record, record->reg);
            break;
        case OM_BASED:
            buffer = DoStrict(buffer, record);
            buffer = SegOverride(buffer);
            *buffer++ = '[';
            *buffer = '\0';
            if (record->flags & OMF_ADR32)
                Scaled(buffer, record, TRUE);
            else
                strcpy(buffer, based[record->reg]);
            while (*buffer)
                buffer++;
            if (record->flags & OMF_OFFSET)
            {
                if (record->flags & OMF_SIGNED_OFFSET)
                    FormatValue(buffer, record, segs, SY_SIGNEDOFS);
                else
                {
                    if (record->flags & OMF_WORD_OFFSET)
                        FormatValue(buffer, record, segs, SY_WORDOFS);
                    else
                        FormatValue(buffer, record, segs, SY_BYTEOFS);
                }
            }
            strcat(buffer, "]");
            break;
        case OM_SEGMENT:
            GetSeg(buffer, record->reg, FALSE);
            break;
    }
}

//-------------------------------------------------------------------------

void FormatDissassembly(char* buffer)
{
    char lbuf[256], *localbuffer = lbuf;
    localbuffer[0] = '\0';
#ifdef DEBUG
    if (segs & SG_ADRSIZ)
        strcat(localbuffer, "adrsiz: ");
    if (segs & SG_OPSIZ)
        strcat(localbuffer, "opsiz: ");
#endif /* DEBUG */
    if (segs & SG_REPZ)
        strcat(localbuffer, "repz ");
    if (segs & SG_REPNZ)
        strcat(localbuffer, "repnz ");
    if (segs & SG_REPNC)
        strcat(localbuffer, "repnc ");
    if (segs & SG_REPC)
        strcat(localbuffer, "repc ");
    strcat(localbuffer, mnemonic);
    localbuffer = TabTo(localbuffer, TAB_ARGPOS);

    PutOperand(localbuffer, &dest);
    if (source.code)
    {
        localbuffer += strlen(localbuffer);
        *localbuffer++ = ',';
        *localbuffer = '\0';
        PutOperand(localbuffer, &source);
    }
    if (extraoperand.code)
    {
        localbuffer += strlen(localbuffer);
        *localbuffer++ = ',';
        *localbuffer = '\0';
        PutOperand(localbuffer, &extraoperand);
    }
    buffer[0] = '\0';
    SegOverride(buffer);
    strcat(buffer, lbuf);
}

//-------------------------------------------------------------------------

static void RegisterSymbol(OPERAND* record)
{
    record->override = segs;
    switch (record->code)
    {
        case OM_FSTREG:
        case OM_CRX:
        case OM_DRX:
        case OM_TRX:
        case OM_SUD:
            break;
        case OM_PORT:
            AddSymbol(record, SY_PORT);
            break;
        case OM_INT:
            AddSymbol(record, SY_INTR);
            break;
        case OM_SHIFT:
            if (!(record->flags & OMF_CL) && (record->address != 1))
                AddSymbol(record, SY_SHIFT);
            break;
        case OM_RETURN:
            AddSymbol(record, SY_RETURN);
            break;
        case OM_SHORTBRANCH:
            AddSymbol(record, SY_SHORTBRANCH);
            break;
        case OM_LONGBRANCH:
            AddSymbol(record, SY_LONGBRANCH);
            break;
        case OM_FARBRANCH:
            AddSymbol(record, SY_SEGMENT);
            AddSymbol(record, SY_ABSBRANCH);
            break;
        case OM_ABSOLUTE:
            AddSymbol(record, SY_ABSOLUTE);
            break;
        case OM_IMMEDIATE:
            if (record->flags & OMF_BYTE)
                if (record->flags & OMF_SIGNED_OFFSET)
                    AddSymbol(record, SY_SIGNEDIMM);
                else
                    AddSymbol(record, SY_BYTEIMM);
            else
                AddSymbol(record, SY_WORDIMM);
            break;
        case OM_REG:
            break;
        case OM_BASED:
            if (record->flags & OMF_OFFSET)
            {
                if (record->flags & OMF_SIGNED_OFFSET)
                    AddSymbol(record, SY_SIGNEDOFS);
                else
                {
                    if (record->flags & OMF_WORD_OFFSET)
                        AddSymbol(record, SY_WORDOFS);
                    else
                        AddSymbol(record, SY_BYTEOFS);
                }
            }
            break;
        case OM_SEGMENT:
            break;
    }
}

//-------------------------------------------------------------------------

void RegisterSymbols(void)
{
    if (dest.code)
        RegisterSymbol(&dest);
    if (source.code)
        RegisterSymbol(&source);
    if (extraoperand.code)
        RegisterSymbol(&extraoperand);
}
