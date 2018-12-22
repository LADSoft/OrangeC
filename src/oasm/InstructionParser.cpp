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
#include <stdio.h>
#include <algorithm>
#include "Instruction.h"
#include "Fixup.h"
#include "UTF8.h"
#include <stdexcept>
#include <iostream>
#include "Token.h"


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
    std::transform(opcode.begin(), opcode.end(), opcode.begin(), ::tolower);

    return opcodeTable.end() != opcodeTable.find(opcode) || prefixTable.end() != prefixTable.find(opcode);
}
Instruction* InstructionParser::Parse(const std::string args, int PC)
{
    int errLine = Errors::GetErrorLine();
    std::string errName = Errors::GetFileName();
    line = args;
    std::string op;
    for (auto val : CleanupValues)
    {
        delete[] val;
    }
    for (auto val : operands)
    {
        delete val;
    }
    for (int i = 0; i < inputTokens.size(); ++i)
    {
        InputToken* t = inputTokens[i];
        delete t;
    }
    inputTokens.clear();
    // can't use clear in openwatcom, it is buggy

    CleanupValues.clear();
    operands.clear();
    prefixes.clear();
    numeric = nullptr;
    //	std::cout << line << std::endl;
    while (true)
    {
        int npos = line.find_first_of(" \t\v\r\n");
        if (npos != std::string::npos)
        {
            op = line.substr(0, npos);
            line = line.substr(npos);
            npos = line.find_first_not_of(" \t\v\r\n");
            if (npos == std::string::npos)
            {
                line = "";
            }
            else
            {
                line.replace(0, npos, "");
            }
        }
        else
        {
            op = line;
            line = "";
        }
        auto it = prefixTable.find(op);
        if (it != prefixTable.end())
        {
            prefixes.push_back(it->second);
        }
        else
        {
            break;
        }
    }
    std::transform(op.begin(), op.end(), op.begin(), ::tolower);

    if (op == "")
    {
        auto rv = DispatchOpcode(-1);
        if (rv == AERR_NONE)
        {
            unsigned char buf[32];
            bits.GetBytes(buf, 32);
            return new Instruction(buf, (bits.GetBits() + 7) / 8);
        }
    }
    else
    {
        auto it = opcodeTable.find(op);
        if (it != opcodeTable.end())
        {
            bits.Reset();
            if (!Tokenize(PC))
            {
                throw new std::runtime_error("Unknown token sequence");
            }
            eol = false;
            auto rv = DispatchOpcode(it->second);
            Instruction* s = nullptr;
            switch (rv)
            {
                case AERR_NONE:
                {
                    unsigned char buf[32];
                    bits.GetBytes(buf, 32);
    #ifdef XXXXX
                    std::cout << std::hex << bits.GetBits() << " ";
                    for (int i = 0; i<bits.GetBits()>> 3; i++)
                        std::cout << std::hex << (int)buf[i] << " ";
                    std::cout << std::endl;
    #endif
                    if (!eol)
                        throw new std::runtime_error("Extra characters at end of line");
                    s = new Instruction(buf, (bits.GetBits() + 7) / 8);
                    //			std::cout << bits.GetBits() << std::endl;
                    for (auto operand : operands)
                    {
                        if (operand->used && operand->size)
                        {
                            if (operand->node->GetType() != AsmExprNode::IVAL && operand->node->GetType() != AsmExprNode::FVAL)
                            {
                                if (s->Lost() && operand->pos)
                                    operand->pos -= 8;
                                int n = operand->relOfs;
                                if (n < 0)
                                    n = -n;
                                Fixup* f = new Fixup(operand->node, (operand->size + 7) / 8, operand->relOfs != 0, n, operand->relOfs > 0);
                                f->SetInsOffs((operand->pos + 7) / 8);
                                f->SetFileName(errName);
                                f->SetErrorLine(errLine);
                                s->Add(f);
                            }
                        }
                    }
                }
                    break;
                case AERR_SYNTAX:
                    throw new std::runtime_error("Syntax error while parsing instruction");
                case AERR_OPERAND:
                    throw new std::runtime_error("Unknown operand");
                case AERR_BADCOMBINATIONOFOPERANDS:
                    throw new std::runtime_error("Bad combination of operands");
                case AERR_UNKNOWNOPCODE:
                    throw new std::runtime_error("Unrecognized opcode");
                case AERR_INVALIDINSTRUCTIONUSE:
                    throw new std::runtime_error("Invalid use of instruction");
                default:
                    throw new std::runtime_error("unknown error");
            }
            return s;
        }
    }
    throw new std::runtime_error("Unrecognized opcode");
}
void InstructionParser::RenameRegisters(AsmExprNode* val)
{
    if (val->GetType() == AsmExprNode::LABEL)
    {
        auto test = val->label;
        std::transform(test.begin(), test.end(), test.begin(), ::tolower);
        auto it = tokenTable.find(test);
        if (it != tokenTable.end())
        {
            int n = it->second;
            if (n >= REGISTER_BASE)
            {
                val->SetType(AsmExprNode::REG);
                val->ival = n - REGISTER_BASE;
            }
        }
    }
    else
    {
        if (val->GetLeft())
            RenameRegisters(val->GetLeft());
        if (val->GetRight())
            RenameRegisters(val->GetRight());
    }
}
AsmExprNode* InstructionParser::ExtractReg(AsmExprNode** val)
{
    AsmExprNode* rv = nullptr;
    switch ((*val)->GetType())
    {
        case AsmExprNode::REG:
            rv = *val;
            *val = nullptr;
            break;
        case AsmExprNode::ADD:
            if ((*val)->GetLeft()->GetType() == AsmExprNode::REG)
            {
                rv = (*val)->GetLeft();
                AsmExprNode* todel = (*val);
                *val = (*val)->GetRight();
                todel->SetLeft(nullptr);
                todel->SetRight(nullptr);
                delete todel;
            }
            else if ((*val)->GetRight()->GetType() == AsmExprNode::REG)
            {
                rv = (*val)->GetRight();
                AsmExprNode* todel = (*val);
                *val = (*val)->GetLeft();
                todel->SetLeft(nullptr);
                todel->SetRight(nullptr);
                delete todel;
            }
            else
            {
                AsmExprNode* v = (*val)->GetLeft();
                if (!(rv = ExtractReg(&v)))
                {
                    v = (*val)->GetRight();
                    rv = ExtractReg(&v);
                    if (rv)
                    {
                        (*val)->SetRight(v);
                    }
                }
                else
                {
                    (*val)->SetLeft(v);
                }
            }
            break;
        case AsmExprNode::SUB:
            if ((*val)->GetLeft()->GetType() == AsmExprNode::REG)
            {
                rv = (*val)->GetLeft();
                (*val)->SetLeft((*val)->GetRight());
                (*val)->SetRight(nullptr);
                (*val)->SetType(AsmExprNode::NEG);
            }
            else
            {
                AsmExprNode* v = (*val)->GetLeft();
                rv = ExtractReg(&v);
                if (rv)
                {
                    (*val)->SetLeft(v);
                }
            }
            break;
        default:
            break;
    }
    return rv;
}
bool InstructionParser::MatchesTimes(AsmExprNode* val)
{
    bool rv = false;
    if (val->GetType() == AsmExprNode::MUL)
    {
        rv = val->GetLeft()->GetType() == AsmExprNode::REG && val->GetRight()->GetType() == AsmExprNode::IVAL;
        if (!rv)
            rv = val->GetRight()->GetType() == AsmExprNode::REG && val->GetLeft()->GetType() == AsmExprNode::IVAL;
    }
    return rv;
}
AsmExprNode* InstructionParser::ExtractTimes(AsmExprNode** val)
{
    AsmExprNode* rv = nullptr;
    switch ((*val)->GetType())
    {
        case AsmExprNode::MUL:
            if (MatchesTimes(*val))
            {
                rv = *val;
                *val = nullptr;
            }
            break;
        case AsmExprNode::ADD:
            if (MatchesTimes((*val)->GetLeft()))
            {
                rv = (*val)->GetLeft();
                AsmExprNode* todel = (*val);
                *val = (*val)->GetRight();
                todel->SetLeft(nullptr);
                todel->SetRight(nullptr);
                delete todel;
            }
            else if (MatchesTimes((*val)->GetRight()))
            {
                rv = (*val)->GetRight();
                AsmExprNode* todel = (*val);
                *val = (*val)->GetLeft();
                todel->SetLeft(nullptr);
                todel->SetRight(nullptr);
                delete todel;
            }
            else
            {
                AsmExprNode* v = (*val)->GetLeft();
                if (!(rv = ExtractTimes(&v)))
                {
                    v = (*val)->GetRight();
                    rv = ExtractTimes(&v);
                    if (rv)
                    {
                        (*val)->SetRight(v);
                    }
                }
                else
                {
                    (*val)->SetLeft(v);
                }
            }
            break;
        case AsmExprNode::SUB:
            if (MatchesTimes((*val)->GetLeft()))
            {
                rv = (*val)->GetLeft();
                (*val)->SetLeft((*val)->GetRight());
                (*val)->SetRight(nullptr);
                (*val)->SetType(AsmExprNode::NEG);
            }
            else
            {
                AsmExprNode* v = (*val)->GetLeft();
                rv = ExtractTimes(&v);
                if (rv)
                {
                    (*val)->SetLeft(v);
                }
            }
            break;
        default:
            break;
    }
    return rv;
}
bool InstructionParser::CheckRegs(AsmExprNode* val)
{
    bool rv = true;
    if (val->GetType() == AsmExprNode::REG)
    {
        rv = false;
    }
    else
    {
        if (val->GetLeft())
            rv &= CheckRegs(val->GetLeft());
        if (val->GetRight())
            rv &= CheckRegs(val->GetLeft());
    }
    return rv;
}
void InstructionParser::ParseNumeric(int PC)
{
    int plus = -1, times = -1;
    auto it = tokenTable.find("+");
    if (it != tokenTable.end())
        plus = it->second;
    it = tokenTable.find("*");
    if (it != tokenTable.end())
        times = it->second;
    RenameRegisters(val);
    if (val->GetType() == AsmExprNode::LABEL)
    {
        auto test = val->label;
        std::transform(test.begin(), test.end(), test.begin(), ::tolower);
        auto it = tokenTable.find(test);

        InputToken* next;
        if (it != tokenTable.end())
        {
            next = new InputToken;
            next->type = InputToken::TOKEN;
            next->val = val;
            val->SetType(AsmExprNode::IVAL);
            val->ival = it->second;
            inputTokens.push_back(next);
        }
        else
        {
            if (val->IsAbsolute())
                val = AsmExpr::Eval(val, PC);
            next = new InputToken;
            next->type = InputToken::NUMBER;
            next->val = val;
            inputTokens.push_back(next);
        }
    }
    else
    {
        AsmExprNode* cur = ExtractReg(&val);
        InputToken* next;
        bool found = false;
        if (cur)
        {
            found = true;
            next = new InputToken;
            next->type = InputToken::REGISTER;
            next->val = cur;
            cur->SetType(AsmExprNode::IVAL);
            inputTokens.push_back(next);
            if (plus != -1)
            {
                while (val && (cur = ExtractReg(&val)))
                {
                    next = new InputToken;
                    next->type = InputToken::TOKEN;
                    next->val = new AsmExprNode(plus);
                    inputTokens.push_back(next);
                    next = new InputToken;
                    next->type = InputToken::REGISTER;
                    next->val = cur;
                    cur->SetType(AsmExprNode::IVAL);
                    inputTokens.push_back(next);
                }
            }
        }
        if (times != -1)
        {
            while (val && (cur = ExtractTimes(&val)))
            {
                if (found)
                {
                    next = new InputToken;
                    next->type = InputToken::TOKEN;
                    next->val = new AsmExprNode(plus);
                    inputTokens.push_back(next);
                }
                found = true;
                AsmExprNode* reg;
                AsmExprNode* mul;
                if (cur->GetLeft()->GetType() == AsmExprNode::REG)
                {
                    reg = cur->GetLeft();
                    mul = cur->GetRight();
                }
                else
                {
                    reg = cur->GetRight();
                    mul = cur->GetLeft();
                }
                next = new InputToken;
                next->type = InputToken::REGISTER;
                next->val = reg;
                reg->SetType(AsmExprNode::IVAL);
                inputTokens.push_back(next);
                next = new InputToken;
                next->type = InputToken::TOKEN;
                next->val = new AsmExprNode(times);
                inputTokens.push_back(next);
                next = new InputToken;
                next->type = InputToken::NUMBER;
                next->val = mul;
                inputTokens.push_back(next);
            }
        }
        if (val)
        {
            if (val->IsAbsolute())
                val = AsmExpr::Eval(val, PC);
            if (found)
            {
                next = new InputToken;
                next->type = InputToken::TOKEN;
                next->val = new AsmExprNode(plus);
                inputTokens.push_back(next);
            }
            next = new InputToken;
            next->type = InputToken::NUMBER;
            next->val = val;
            inputTokens.push_back(next);
        }
    }
}
bool InstructionParser::Tokenize(int PC)
{
    while (line.size())
    {
        NextToken(PC);
        InputToken* next;
        switch (id)
        {
            case -1:
                return false;
            default:
                next = new InputToken;
                next->type = InputToken::TOKEN;
                next->val = new AsmExprNode(id);
                inputTokens.push_back(next);
                break;
            case TK_NUMERIC:
                ParseNumeric(PC);
                next = nullptr;
                if (val && !CheckRegs(val))
                {
                    return false;
                }
                break;
        }
    }
    return true;
}
bool InstructionParser::IsNumber()
{
    return line.size() &&
           (isdigit(line[0]) || line[0] == '+' || line[0] == '-' || line[0] == '~' || line[0] == '!' || line[0] == '(');
}
void InstructionParser::NextToken(int PC)
{
    id = -1;
    bool negate = false;
    int npos = line.find_first_not_of(" \t\r\n\v");
    if (npos == std::string::npos)
        line = "";
    else if (npos != 0)
        line = line.substr(npos);
    if (line.size())
    {
        int n = 0;
        std::string tk;
        if (line[0] == '"' || line[0] == '\'')
        {
            int accum = 0;
            int i;
            int quote = line[0];
            for (i = 1; i < line.size(); i++)
            {
                if (line[i] == quote)
                    break;
                if (i < 5)  // endianness & sizing
                    accum += line[i] << ((i - 1) * 8);
            }
            if (i == line.size())
                throw new std::runtime_error("Expected end quote");
            if (i == 1)
                throw new std::runtime_error("Quoted string is empty");
            line.erase(0, i + 1);
            id = TK_NUMERIC;
            val = new AsmExprNode(accum);
        }
        else if (Tokenizer::IsSymbolChar(line.c_str(), true) || IsNumber() || line[0] == '$')
        {
            val = asmexpr.Build(line);
            id = TK_NUMERIC;
        }
        else if (ispunct(line[0]))
        {
            token = line[0];
            line.erase(0, 1);
            auto it = tokenTable.find(token);
            if (it != tokenTable.end())
            {
                id = it->second;
            }
        }
    }
}
