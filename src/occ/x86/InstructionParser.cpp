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

#include "InstructionParser.h"
#include "Errors.h"
#include <ctype.h>
#include <fstream>
#include <cassert>
#include <stdio.h>
#include "Instruction.h"
#include "Fixup.h"
#include <stdexcept>
#include <iostream>
#include "Token.h"
#include "be.h"
extern TYPE stdint;
extern bool assembling;

static const unsigned mask[32] = {
    0x1,      0x3,      0x7,       0xf,       0x1f,      0x3f,      0x7f,       0xff,       0x1ff,      0x3ff,      0x7ff,
    0xfff,    0x1fff,   0x3fff,    0x7fff,    0xffff,    0x1ffff,   0x3ffff,    0x7ffff,    0xfffff,    0x1fffff,   0x3fffff,
    0x7fffff, 0xffffff, 0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff,
};
void BitStream::Add(int val, int cnt)
{
    val &= mask[cnt - 1];
    int v = 8 - (bits & 7);
    //	std::cout << val << ";" << cnt << std::endl;
    if (cnt > v)
    {
        if (v != 8)
        {
            // assumes won't cross byte boundary
            bytes[bits >> 3] |= (val >> (cnt - v));
            cnt -= v;
            bits += v;
        }
        while (cnt > 0)
        {
            // endianness assumed...
            bytes[bits >> 3] = (val & 0xff);
            val >>= 8;
            bits += 8;
            cnt -= 8;
        }
    }
    else
    {
        bytes[bits >> 3] |= val << (v - cnt);
        bits += (cnt);
    }
}
bool InstructionParser::ParseNumber(int relOfs, int sign, int bits, int needConstant, int tokenPos)
{

    if (inputTokens[tokenPos]->type == InputToken::NUMBER)
    {
        val = inputTokens[tokenPos]->val;
        bool isConst = val->IsAbsolute();
        if (isConst || !needConstant)
        {
            if (isConst)
            {
                if (val->GetType() != AsmExprNode::IVAL)
                    return false;
                if (sign)
                {
                    if (bits == 8 && (val->ival & ~mask[bits - 2]) != 0)
                        if ((val->ival & ~mask[bits - 2]) != (~mask[bits - 2]))
                            if ((val->ival & ~mask[bits - 2]) != 0)
                                return false;
                }
                else
                {
                    if (bits == 8 && (val->ival & ~mask[bits - 1]) != 0)
                        if ((val->ival & ~mask[bits - 2]) != (~mask[bits - 2]))
                            return false;
                }
            }
            numeric = new Numeric;
            numeric->node = val;
            numeric->pos = 0;
            numeric->relOfs = relOfs;
            numeric->size = bits;
            numeric->used = false;
            return true;
        }
    }
    return false;
}
bool InstructionParser::SetNumber(int tokenPos, int oldVal, int newVal)
{
    bool rv = false;
    if (tokenPos < inputTokens.size() && inputTokens[tokenPos]->type == InputToken::NUMBER)
    {
        val = inputTokens[tokenPos]->val;
        bool isConst = val->IsAbsolute();
        if (isConst)
        {
            if (val->GetType() == AsmExprNode::IVAL)
            {
                if (val->ival == oldVal)
                {
                    numeric = new Numeric;
                    memset(numeric, 0, sizeof(*numeric));
                    numeric->node = new AsmExprNode(newVal);
                    rv = true;
                }
            }
        }
    }
    return rv;
}
bool InstructionParser::MatchesOpcode(std::string opcode)
{
    return opcodeTable.end() != opcodeTable.find(opcode) || prefixTable.end() != prefixTable.find(opcode);
}

std::string InstructionParser::FormatInstruction(ocode* ins)
{
    std::string rv = ::opcodeTable[ins->opcode];
    rv += " ";
    for (auto t : inputTokens)
    {
        switch (t->type)
        {
            case InputToken::LABEL:
                rv += t->val->label;
                break;
            case InputToken::NUMBER:
            {
                if (t->val->GetType() == AsmExprNode::ADD)
                {
                    rv += t->val->GetLeft()->label + "+";
                    char buf[256];
                    sprintf(buf, "%d", t->val->GetRight()->ival);
                    rv += buf;
                }
                else if (t->val->GetType() == AsmExprNode::LABEL)
                {
                    rv += t->val->label;
                }
                else
                {
                    char buf[256];
                    sprintf(buf, "%d", t->val->ival);
                    rv += buf;
                }
                break;
            }
            case InputToken::REGISTER:
                rv += tokenNames[(e_tk)(t->val->ival + 1000)];
                break;
            case InputToken::TOKEN:
                rv += tokenNames[(e_tk)t->val->ival];
                break;
            default:
                rv += "unknown";
                break;
        }
    }
    return rv;
}

asmError InstructionParser::GetInstruction(OCODE* ins, Instruction*& newIns, std::list<Numeric*>& operands)
{
    inputTokens.clear();
    // can't use clear in openwatcom, it is buggy

    CleanupValues.clear();
    this->operands.clear();
    prefixes.clear();
    numeric = nullptr;
    switch (ins->opcode)
    {
        case op_repz:
        case op_repe:
        case op_rep:
            newIns = new Instruction((unsigned char*)"\xf3", 1, true);
            break;
        case op_repnz:
        case op_repne:
            newIns = new Instruction((unsigned char*)"\xf2", 1, true);
            break;
        case op_lock:
            newIns = new Instruction((unsigned char*)"\xf0", 1, true);
            break;
        default:
        {
            switch (ins->opcode)
            {
                case op_ret:
                    if (ins->oper1)
                        ins->oper1->length = 0;
                    break;
                case op_lea:
                    ins->oper2->length = 0;
                    break;
                case op_push:
                {
                    AMODE* aps = ins->oper1;
                    if (!aps->length)
                        aps->length = ISZ_UINT;
                    if (aps->mode == am_immed && isintconst(aps->offset) && aps->offset->v.i >= CHAR_MIN &&
                        aps->offset->v.i <= CHAR_MAX)
                        aps->length = ISZ_UCHAR;
                    break;
                }
                case op_add:
                case op_sub:
                case op_adc:
                case op_sbb:
                case op_imul:
                    /* yes you can size an imul constant !!!! */
                case op_cmp:
                case op_and:
                case op_or:
                case op_xor:
                case op_idiv:
                {
                    AMODE* aps = ins->oper1;
                    AMODE* apd = ins->oper2;
                    if (apd)
                    {
                        if (apd->mode == am_immed && isintconst(apd->offset) && apd->offset->v.i >= CHAR_MIN &&
                            apd->offset->v.i <= CHAR_MAX)
                            apd->length = ISZ_UCHAR;
                    }
                    else
                    {
                        if (!aps->length)
                            aps->length = ISZ_UINT;
                    }
                }
                break;
                case op_mov:
                    if (ins->oper2 && ins->oper2->mode == am_immed)
                    {
                        ins->oper2->length = 0;
                        if (isintconst(ins->oper2->offset))
                        {
                            if (ins->oper1->length == ISZ_UCHAR)
                                ins->oper2->offset->v.i &= 0xff;
                            else if (ins->oper1->length == ISZ_USHORT || ins->oper1->length == ISZ_U16)
                                ins->oper2->offset->v.i &= 0xffff;
                        }
                    }
                    break;
                case op_btr:
                case op_bts:
                case op_bt:
                case op_shl:
                case op_shr:
                case op_sal:
                case op_sar:
                case op_rol:
                case op_ror:
                case op_rcl:
                case op_rcr:
                    if (ins->oper2 && ins->oper2->mode == am_immed)
                        ins->oper2->length = 0;
                    break;
                case op_shrd:
                case op_shld:
                    if (ins->oper3)
                        ins->oper3->length = 0;
                    break;
                default:
                    if (ins->opcode >= op_ja && ins->opcode <= op_jz)
                        ins->oper1->length = 0;
                    if (ins->opcode == op_ret && ins->oper1)
                        ins->oper1->length = 0;

                    break;
            }
            SetTokens(ins);
            bits.Reset();
            asmError rv = DispatchOpcode(ins->opcode);
            if (rv == AERR_NONE)
            {
                unsigned char buf[32];
                bits.GetBytes(buf, 32);
                newIns = new Instruction(buf, (bits.GetBits() + 7) / 8);
                operands = this->operands;
            }
            return rv;
        }
    }
    return AERR_NONE;
}
void InstructionParser::SetRegToken(int reg, int sz)
{
    static InputToken* segs[] = {&Tokencs, &Tokends, &Tokenes, &Tokenfs, &Tokengs, &Tokenss};
    static InputToken* dword[] = {&Tokeneax, &Tokenecx, &Tokenedx, &Tokenebx, &Tokenesp, &Tokenebp, &Tokenesi, &Tokenedi};
    static InputToken* word[] = {&Tokenax, &Tokencx, &Tokendx, &Tokenbx, &Tokensp, &Tokenbp, &Tokensi, &Tokendi};
    static InputToken* byte[] = {&Tokenal, &Tokencl, &Tokendl, &Tokenbl, &Tokenah, &Tokench, &Tokendh, &Tokenbh};
    static InputToken* floats[] = {&Tokenst0, &Tokenst1, &Tokenst2, &Tokenst3, &Tokenst4, &Tokenst5, &Tokenst6, &Tokenst7};
    static InputToken* creg[] = {&Tokencr0, &Tokencr1, &Tokencr2, &Tokencr3, &Tokencr4, &Tokencr5, &Tokencr6, &Tokencr7};
    static InputToken* dreg[] = {&Tokendr0, &Tokendr1, &Tokendr2, &Tokendr3, &Tokendr4, &Tokendr5, &Tokendr6, &Tokendr7};
    static InputToken* treg[] = {&Tokentr0, &Tokentr1, &Tokentr2, &Tokentr3, &Tokentr4, &Tokentr5, &Tokentr6, &Tokentr7};
    static InputToken* mmreg[] = {&Tokenmm0, &Tokenmm1, &Tokenmm2, &Tokenmm3, &Tokenmm4, &Tokenmm5, &Tokenmm6, &Tokenmm7};
    static InputToken* xmmreg[] = {&Tokenxmm0, &Tokenxmm1, &Tokenxmm2, &Tokenxmm3, &Tokenxmm4, &Tokenxmm5, &Tokenxmm6, &Tokenxmm7};
    static InputToken** check;

    if (sz < 0)
        sz = -sz;
    if (sz == 0)
        sz = 4;
    switch (sz)
    {
        case ISZ_UCHAR:
        case ISZ_BOOLEAN:
            check = byte;
            break;
        case ISZ_USHORT:
        case ISZ_U16:
        case ISZ_WCHAR:
            check = word;
            break;
        case ISZ_UINT:
        case ISZ_ULONG:
        case ISZ_ADDR:
        case ISZ_U32:
            check = dword;
            break;
        case ISZ_FLOAT:
        case ISZ_IFLOAT:
        case ISZ_DOUBLE:
        case ISZ_IDOUBLE:
        case ISZ_LDOUBLE:
        case ISZ_ILDOUBLE:
            check = floats;
            break;
        case 100:
            check = segs;
            break;
        case 200:
            check = creg;
            break;
        case 201:
            check = dreg;
            break;
        case 202:
            check = treg;
            break;
        case 300:
            check = mmreg;
            break;
        case 301:
            check = xmmreg;
            break;
        default:
            diag("SetRegToken: unknown enum");
            break;
    }
    inputTokens.push_back(check[reg]);
}
void InstructionParser::SetNumberToken(int val)
{
    InputToken* next = new InputToken;
    next->type = InputToken::NUMBER;
    next->val = new AsmExprNode(val);
    inputTokens.push_back(next);
}
int resolveoffset(EXPRESSION* n, int* resolved);
AsmExprNode* MakeFixup(EXPRESSION* oper);

bool InstructionParser::SetNumberToken(EXPRESSION* offset, int& n)
{
    int resolved = 1;
    n = resolveoffset(offset, &resolved);
    resolved |= assembling;
    if (resolved)
        SetNumberToken(n);
    return !!resolved;
}
void InstructionParser::SetExpressionToken(EXPRESSION* offset)
{
    int n;
    if (!SetNumberToken(offset, n))
    {
        EXPRESSION* exp = GetSymRef(offset);
        AsmExprNode* expr = MakeFixup(offset);
        InputToken* next = new InputToken;
        next->type = InputToken::NUMBER;
        next->val = expr;
        inputTokens.push_back(next);
    }
}
void InstructionParser::SetSize(int sz)
{
    if (sz)
    {
        if (sz < 0)
            sz = -sz;
        switch (sz)
        {
            case ISZ_UCHAR:
            case ISZ_BOOLEAN:
                inputTokens.push_back(&Tokenbyte);
                break;
            case ISZ_WCHAR:
            case ISZ_USHORT:
            case ISZ_U16:
                inputTokens.push_back(&Tokenword);
                break;
            case ISZ_UINT:
            case ISZ_ULONG:
            case ISZ_ADDR:
            case ISZ_FLOAT:
            case ISZ_IFLOAT:
            case ISZ_U32:
                inputTokens.push_back(&Tokendword);
                break;
            case ISZ_ULONGLONG:
            case ISZ_DOUBLE:
            case ISZ_IDOUBLE:
            case ISZ_LDOUBLE:
            case ISZ_ILDOUBLE:
                inputTokens.push_back(&Tokenqword);
                break;
            default:
                diag("SetSize: unknown enum");
                break;
        }
    }
}
void InstructionParser::SetBracketSequence(bool open, int sz, int seg)
{
    if (open)
    {
        SetSize(sz);
        inputTokens.push_back(&Tokenopenbr);
        if (seg)
        {
            inputTokens.push_back(seg == e_fs ? &Tokenfs : &Tokengs);
            inputTokens.push_back(&Tokencolon);
        }
    }
    else
    {
        inputTokens.push_back(&Tokenclosebr);
    }
}
void InstructionParser::SetOperandTokens(amode* operand)
{
    switch (operand->mode)
    {
        case am_dreg:
            SetRegToken(operand->preg, operand->length);
            break;
        case am_freg:
            SetRegToken(operand->preg, ISZ_LDOUBLE);
            break;
        case am_screg:
            SetRegToken(operand->preg, 200);
            break;
        case am_sdreg:
            SetRegToken(operand->preg, 201);
            break;
        case am_streg:
            SetRegToken(operand->preg, 202);
            break;
        case am_seg:
            SetRegToken(operand->preg, 100);
            break;
        case am_mmreg:
            SetRegToken(operand->preg, 300);
            break;
        case am_xmmreg:
            SetRegToken(operand->preg, 301);
            break;
        case am_indisp:
            SetBracketSequence(true, operand->length, operand->seg);
            SetRegToken(operand->preg, ISZ_UINT);
            if (operand->offset && !isconstzero(&stdint, operand->offset))
            {
                inputTokens.push_back(&Tokenplus);
                SetExpressionToken(operand->offset);
            }
            SetBracketSequence(false, 0, 0);
            break;
        case am_indispscale:
            SetBracketSequence(true, operand->length, operand->seg);
            if (operand->preg != -1)
            {
                SetRegToken(operand->preg, ISZ_UINT);
                inputTokens.push_back(&Tokenplus);
            }
            SetRegToken(operand->sreg, ISZ_UINT);
            if (operand->scale != 0)
            {
                inputTokens.push_back(&Tokenstar);
                SetNumberToken(1 << operand->scale);
            }
            if (operand->offset && !isconstzero(&stdint, operand->offset))
            {
                inputTokens.push_back(&Tokenplus);
                SetExpressionToken(operand->offset);
            }
            SetBracketSequence(false, 0, 0);
            break;
        case am_direct:
            SetBracketSequence(true, operand->length, operand->seg);
            SetExpressionToken(operand->offset);
            SetBracketSequence(false, 0, 0);
            break;
        case am_immed:
            SetSize(operand->length);
            SetExpressionToken(operand->offset);
            break;
        default:
            assert(0);
            break;
    }
}
void InstructionParser::SetTokens(ocode* ins)
{
    if (ins->oper1)
    {
        SetOperandTokens(ins->oper1);
    }
    if (ins->oper2)
    {
        inputTokens.push_back(&Tokencomma);
        SetOperandTokens(ins->oper2);
    }
    if (ins->oper3)
    {
        inputTokens.push_back(&Tokencomma);
        SetOperandTokens(ins->oper3);
    }
}
Instruction* InstructionParser::Parse(const std::string args, int PC) { return nullptr; }
