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

#include "InstructionParser.h"
#include "Errors.h"
#include <cctype>
#include <fstream>
#include <cassert>
#include <cstdio>
#include "Instruction.h"
#include "Fixup.h"
#include <stdexcept>
#include <iostream>
#include "Token.h"
#include "be.h"
#include "symfuncs.h"
#include "config.h"
#include "ildata.h"
#include "occ.h"

extern const unsigned BitMasks[32];

bool InstructionParser::ParseNumber(int relOfs, int sign, int bits, int needConstant, int tokenPos)
{

    if (inputTokens[tokenPos]->type == InputToken::NUMBER)
    {
        val = (AsmExprNode*)inputTokens[tokenPos]->val;
        bool isConst = val->IsAbsolute();
        if (isConst || !needConstant)
        {
            if (isConst)
            {
                if (val->GetType() != AsmExprNode::IVAL)
                    return false;
                if (sign)
                {
                    if (bits == 8 && (val->ival & ~BitMasks[bits - 2]) != 0)
                        if ((val->ival & ~BitMasks[bits - 2]) != (~BitMasks[bits - 2]))
                            if ((val->ival & ~BitMasks[bits - 2]) != 0)
                                return false;
                }
                else
                {
                    if (bits == 8 && (val->ival & ~BitMasks[bits - 1]) != 0)
                        if ((val->ival & ~BitMasks[bits - 2]) != (~BitMasks[bits - 2]))
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
        val = (AsmExprNode*)inputTokens[tokenPos]->val;
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
        AsmExprNode* val = (AsmExprNode*)t->val;
        switch (t->type)
        {
            case InputToken::LABEL:
                rv += val->label;
                break;
            case InputToken::NUMBER: {
                if (val->GetType() == AsmExprNode::ADD)
                {
                    rv += val->GetLeft()->label + "+";
                    char buf[256];
                    sprintf(buf, "%d", (int)val->GetRight()->ival);
                    rv += buf;
                }
                else if (val->GetType() == AsmExprNode::LABEL)
                {
                    rv += val->label;
                }
                else
                {
                    char buf[256];
                    sprintf(buf, "%d", (int)val->ival);
                    rv += buf;
                }
                break;
            }
            case InputToken::REGISTER:
                rv += tokenNames[(e_tk)(val->ival + 1000)];
                break;
            case InputToken::TOKEN:
                rv += tokenNames[(e_tk)val->ival];
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
    for (auto v : CleanupValues)
        delete v;
    for (auto v : operands)
        delete v;
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
        default: {
            switch (ins->opcode)
            {
                case op_ret:
                    if (ins->oper1)
                        ins->oper1->length = 0;
                    break;
                case op_lea:
                    ins->oper2->length = 0;
                    break;
                case op_push: {
                    AMODE* aps = ins->oper1;
                    if (!aps->length)
                        aps->length = ISZ_UINT;
                    if (aps->mode == am_immed && isintconst(aps->offset) && aps->offset->i >= CHAR_MIN &&
                        aps->offset->i <= CHAR_MAX)
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
                case op_idiv: {
                    AMODE* aps = ins->oper1;
                    AMODE* apd = ins->oper2;
                    if (apd)
                    {
                        if (apd->mode == am_immed && isintconst(apd->offset) && apd->offset->i >= CHAR_MIN &&
                            apd->offset->i <= CHAR_MAX)
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
                                ins->oper2->offset->i &= 0xff;
                            else if (ins->oper1->length == ISZ_USHORT || ins->oper1->length == ISZ_U16 ||
                                     ins->oper1->length == ISZ_WCHAR)
                                ins->oper2->offset->i &= 0xffff;
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
                case op_test:
                    if (ins->oper2 && ins->oper2->mode == am_immed)
                        ins->oper2->length = 0;
                    break;
                case op_shrd:
                case op_shld:
                case op_shufpd:
                case op_shufps:
                    if (ins->oper3 && ins->oper3->mode == am_immed)
                        ins->oper3->length = 0;
                    break;
                default:
                    if (ins->opcode >= op_ja && ins->opcode <= op_jz)
                        if (ins->opcode != op_jmp || ins->oper1->mode == am_immed)
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
                unsigned char buf[64];
                bits.GetBytes(buf, (bits.GetBits() + 7) / 8);
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
    static InputToken* segs[] = {&Tokencs, &Tokencs, &Tokends, &Tokenes, &Tokenfs, &Tokengs, &Tokenss};
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
int resolveoffset(Optimizer::SimpleExpression* n, int* resolved)
{
#ifdef ISPARSER
    return 0;
#else
    int rv = 0;
    if (n)
    {
        switch (n->type)
        {
            case Optimizer::se_sub:
                rv += resolveoffset(n->left, resolved);
                rv -= resolveoffset(n->right, resolved);
                break;
            case Optimizer::se_add:
                // case Optimizer::se_addstruc:
                rv += resolveoffset(n->left, resolved);
                rv += resolveoffset(n->right, resolved);
                break;
            case Optimizer::se_i:
            case Optimizer::se_ui:
                rv += n->i;
                break;
            case Optimizer::se_auto: {
                int m = n->sp->offset;

                if (!usingEsp && m > 0)
                    m += 4;

                if (n->sp->storage_class == Optimizer::scc_parameter && Optimizer::fastcallAlias)
                {
                    if ((currentFunction->tp->btp->type != Optimizer::st_struct &&
                         currentFunction->tp->btp->type != Optimizer::st_union) ||
                        n->sp->offset != Optimizer::chosenAssembler->arch->retblocksize)
                    {

                        m -= Optimizer::fastcallAlias * Optimizer::chosenAssembler->arch->parmwidth;
                        if (m >= Optimizer::chosenAssembler->arch->retblocksize)
                        {
                            rv += m;
                        }
                    }
                    else
                    {
                        rv += m;
                    }
                }
                else
                {
                    rv += m;
                }
            }
            break;
            case Optimizer::se_labcon:
            case Optimizer::se_global:
            case Optimizer::se_pc:
            case Optimizer::se_threadlocal:
                *resolved = 0;
                break;
            default:
                diag("Unexpected node type in resolveoffset");
                break;
        }
    }
    return rv;
#endif
}
AsmExprNode* MakeFixup(Optimizer::SimpleExpression* offset)
{
    int resolved = 1;
    int n = resolveoffset(offset, &resolved);
    if (!resolved)
    {
        AsmExprNode* rv;
        if (offset->type == Optimizer::se_sub && offset->left->type == Optimizer::se_threadlocal)
        {
            Optimizer::SimpleExpression* node = GetSymRef(offset->left);
            Optimizer::SimpleExpression* node1 = GetSymRef(offset->right);
            std::string name = node->sp->outputName;
            AsmExprNode* left = new AsmExprNode(name);
            name = node1->sp->outputName;
            AsmExprNode* right = new AsmExprNode(name);
            rv = new AsmExprNode(AsmExprNode::SUB, left, right);
        }
        else
        {
            Optimizer::SimpleExpression* node = GetSymRef(offset);
            std::string name;
            if (node->type == Optimizer::se_labcon)
            {
                char buf[256];
                sprintf(buf, "L_%d", (int)node->i);
                name = buf;
            }
            else
            {
                name = node->sp->outputName;
            }
            rv = new AsmExprNode(name);
        }
        if (n)
        {
            rv = new AsmExprNode(AsmExprNode::ADD, rv, new AsmExprNode(n));
        }
        return rv;
    }
    else
    {
        return new AsmExprNode(n);
    }
}

void InstructionParser::SetNumberToken(int val)
{
    InputToken* next = new InputToken;
    next->type = InputToken::NUMBER;
    next->val = new AsmExprNode(val);
    inputTokens.push_back(next);
}
AsmExprNode* MakeFixup(Optimizer::SimpleExpression* oper);

bool InstructionParser::SetNumberToken(Optimizer::SimpleExpression* offset, int& n)
{
    int resolved = 1;
    n = resolveoffset(offset, &resolved);
    resolved |= Optimizer::assembling;
    if (resolved)
        SetNumberToken(n);
    return !!resolved;
}
void InstructionParser::SetExpressionToken(Optimizer::SimpleExpression* offset)
{
    int n;
    if (!SetNumberToken(offset, n))
    {
        Optimizer::SimpleExpression* exp = GetSymRef(offset);
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
            case ISZ_FARPTR:
                inputTokens.push_back(&Tokenfar);
                break;
            default:
                diag("SetSize: unknown enum");
                break;
        }
    }
}
void InstructionParser::SetBracketSequence(bool open, int sz, int seg)
{
    static InputToken* segs[] = {&Tokencs, &Tokencs, &Tokends, &Tokenes, &Tokenfs, &Tokengs, &Tokenss};
    if (open)
    {
        SetSize(sz);
        inputTokens.push_back(&Tokenopenbr);
        if (seg)
        {
            inputTokens.push_back(segs[seg]);
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
            SetRegToken(operand->seg, 100);
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
            if (operand->offset && ((operand->offset->type != Optimizer::se_ui && operand->offset->type != Optimizer::se_i) ||
                                    operand->offset->i != 0))
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
            if (operand->offset && ((operand->offset->type != Optimizer::se_ui && operand->offset->type != Optimizer::se_i) ||
                                    operand->offset->i != 0))
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
Instruction* InstructionParser::Parse(const std::string& args, int PC) { return nullptr; }