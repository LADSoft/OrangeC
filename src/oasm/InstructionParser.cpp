/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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
#include <cstdio>
#include <algorithm>
#include "Instruction.h"
#include "Fixup.h"
#include "ppInclude.h"
#include "UTF8.h"
#include <stdexcept>
#include <iostream>
#include "Token.h"

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
                    numeric = new Numeric(new AsmExprNode(newVal));
                    rv = true;
                }
            }
        }
    }
    return rv;
}

void InstructionParser::SetATT(bool att)
{
    attSyntax = att;
    if (attSyntax)
    {
        opcodeTable["cbtw"] = opcodeTable["cbw"];
        opcodeTable["cwtl"] = opcodeTable["cwde"];
        opcodeTable["cwtd"] = opcodeTable["cwd"];
        opcodeTable["cltd"] = opcodeTable["cdq"];
        opcodeTable["cltq"] = opcodeTable["cdqe"];
        opcodeTable["cqto"] = opcodeTable["cqo"];
        opcodeTable["lcall"] = 10000;
        opcodeTable["ljmp"] = 10001;
        ppInclude::SetCommentChar('#');
    }
    else
    {
        opcodeTable.erase("cbtw");
        opcodeTable.erase("cwtl");
        opcodeTable.erase("cwtd");
        opcodeTable.erase("cltd");
        opcodeTable.erase("cltq");
        opcodeTable.erase("cqto");
        ppInclude::SetCommentChar(';');
    }
}

bool InstructionParser::MatchesOpcode(std::string opcode)
{
    std::transform(opcode.begin(), opcode.end(), opcode.begin(), ::tolower);

    opcode = ParsePreInstruction(opcode, false);
    if (opcodeTable.end() != opcodeTable.find(opcode) || prefixTable.end() != prefixTable.find(opcode))
        return true;
    if (attSyntax)
    {
        if (opcode.size() == 6)
        {
            if (opcode.substr(0, 4) == "movs" || opcode.substr(0, 4) == "movz")
                if (opcode[4] == 'b' || opcode[4] == 'w' || opcode[4] == 'l')
                    if (opcode[5] == 'w' || opcode[5] == 'l' || opcode[5] == 'q')
                        return true;
        }
        else if (opcode.size() > 0)
        {
            char ch = opcode[opcode.size() - 1];
            if (opcodeTable.end() != opcodeTable.find(opcode.substr(0, opcode.size() - 1)))
            {
                switch (ch)
                {
                    case 'b':
                    case 'w':
                    case 'l':
                    case 's':
                    case 't':
                        return true;
                }
            }
        }
    }
    return false;
}
std::unordered_map<std::string, int>::iterator InstructionParser::GetOpcode(const std::string& opcode, int& size1, int& size2)
{
    size1 = size2 = 0;
    auto it = opcodeTable.find(opcode);
    if (it == opcodeTable.end() && attSyntax)
    {
        if (opcode.size() == 6)
        {
            if (opcode.substr(0, 4) == "movs" || opcode.substr(0, 4) == "movz")
            {
                it = opcodeTable.find(opcode.substr(0, 4) + "x");
                switch (opcode[4])
                {
                    case 'b':
                        size1 = 1;
                        break;
                    case 'w':
                        size1 = 2;
                        break;
                    case 'l':
                        size1 = 4;
                        break;
                    default:
                        return opcodeTable.end();
                }
                switch (opcode[5])
                {
                    case 'w':
                        size2 = 2;
                        break;
                    case 'l':
                        size2 = 4;
                        break;
                    case 'q':
                        size2 = 8;
                        break;
                    default:
                        return opcodeTable.end();
                }
                if (size2 <= size1)
                    return opcodeTable.end();
            }
        }
        else
        {
            char ch = opcode[opcode.size() - 1];
            it = opcodeTable.find(opcode.substr(0, opcode.size() - 1));

            switch (ch)
            {
                case 'b':
                    size1 = 1;
                    break;
                case 'w':
                    size1 = 2;
                    break;
                case 'l':
                    size1 = 4;
                    break;
                case 'q':
                    size1 = 8;
                    break;
                case 's':
                    size1 = -4;
                    break;
                case 't':
                    size1 = -10;
                    break;
                default:
                    return opcodeTable.end();
            }
        }
    }
    return it;
}
Instruction* InstructionParser::Parse(const std::string& args, int PC)
{
    int errLine = Errors::GetErrorLine();
    std::string errName = Errors::GetFileName();
    line = ParsePreInstruction(args, true);
    std::string op;
    inputTokens.clear();

    for (auto c : CleanupValues)
        delete c;
    for (auto o : operands)
        delete o;
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
            unsigned char buf[64];
            bits.GetBytes(buf, (bits.GetBits() + 7) / 8);
            return new Instruction(buf, (bits.GetBits() + 7) / 8);
        }
    }
    else
    {
        int size1 = 0, size2 = 0;
        auto it = GetOpcode(op, size1, size2);
        if (it != opcodeTable.end())
        {
            bits.Reset();
            int opcode = it->second;
            if (!Tokenize(opcode, PC, size1, size2))
            {
                throw new std::runtime_error("Unknown token sequence");
            }
            eol = false;
            auto rv = DispatchOpcode(opcode);
            Instruction* s = nullptr;
            switch (rv)
            {
                case AERR_NONE: {
                    unsigned char buf[32];
                    bits.GetBytes(buf, 32);
#ifdef XXXXX
                    std::cout << std::hex << bits.GetBits() << " ";
                    for (int i = 0; i < bits.GetBits() >> 3; i++)
                        std::cout << std::hex << (int)buf[i] << " ";
                    std::cout << std::endl;
#endif
                    if (!eol)
                        throw new std::runtime_error("Extra characters at end of line");
                    s = new Instruction(buf, (bits.GetBits() + 7) / 8);
                    //			std::cout << bits.GetBits() << std::endl;
                    for (auto& operand : operands)
                    {
                        if (operand->used && operand->size)
                        {
                            if (((AsmExprNode*)operand->node)->GetType() != AsmExprNode::IVAL &&
                                ((AsmExprNode*)operand->node)->GetType() != AsmExprNode::FVAL)
                            {
                                if (s->Lost() && operand->pos)
                                    operand->pos -= 8;
                                int n = operand->relOfs;
                                if (n < 0)
                                    n = -n;
                                Fixup* f = new Fixup((AsmExprNode*)operand->node, (operand->size + 7) / 8, operand->relOfs != 0, n,
                                                     operand->relOfs > 0);
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
                std::unique_ptr<AsmExprNode> todel(*val);
                *val = (*val)->GetRight();
                todel->SetLeft(nullptr);
                todel->SetRight(nullptr);
            }
            else if ((*val)->GetRight()->GetType() == AsmExprNode::REG)
            {
                rv = (*val)->GetRight();
                std::unique_ptr<AsmExprNode> todel(*val);
                *val = (*val)->GetLeft();
                todel->SetLeft(nullptr);
                todel->SetRight(nullptr);
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
                std::unique_ptr<AsmExprNode> todel(*val);
                *val = (*val)->GetRight();
                todel->SetLeft(nullptr);
                todel->SetRight(nullptr);
            }
            else if (MatchesTimes((*val)->GetRight()))
            {
                rv = (*val)->GetRight();
                std::unique_ptr<AsmExprNode> todel(*val);
                *val = (*val)->GetLeft();
                todel->SetLeft(nullptr);
                todel->SetRight(nullptr);
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
            rv &= CheckRegs(val->GetRight());
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
            inputTokens.push_back(new InputToken);
            next = inputTokens.back();
            next->type = InputToken::TOKEN;
            next->val = val;
            val->SetType(AsmExprNode::IVAL);
            val->ival = it->second;
        }
        else
        {
            if (val->IsAbsolute())
                val = AsmExpr::Eval(val, PC);
            inputTokens.push_back(new InputToken);
            next = inputTokens.back();
            next->type = InputToken::NUMBER;
            next->val = val;
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
            inputTokens.push_back(new InputToken);
            next = inputTokens.back();
            next->type = InputToken::REGISTER;
            next->val = cur;
            cur->SetType(AsmExprNode::IVAL);
            if (plus != -1)
            {
                while (val && (cur = ExtractReg(&val)))
                {
                    inputTokens.push_back(new InputToken);
                    next = inputTokens.back();
                    next->type = InputToken::TOKEN;
                    next->val = new AsmExprNode(plus);
                    inputTokens.push_back(new InputToken);
                    next = inputTokens.back();
                    next->type = InputToken::REGISTER;
                    next->val = cur;
                    cur->SetType(AsmExprNode::IVAL);
                }
            }
        }
        if (times != -1)
        {
            while (val && (cur = ExtractTimes(&val)))
            {
                if (found)
                {
                    inputTokens.push_back(new InputToken);
                    next = inputTokens.back();
                    next->type = InputToken::TOKEN;
                    next->val = new AsmExprNode(plus);
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
                inputTokens.push_back(new InputToken);
                next = inputTokens.back();
                next->type = InputToken::REGISTER;
                next->val = reg;
                reg->SetType(AsmExprNode::IVAL);
                inputTokens.push_back(new InputToken);
                next = inputTokens.back();
                next->type = InputToken::TOKEN;
                next->val = new AsmExprNode(times);
                inputTokens.push_back(new InputToken);
                next = inputTokens.back();
                next->type = InputToken::NUMBER;
                next->val = mul;
            }
        }
        if (val)
        {
            if (val->IsAbsolute())
                val = AsmExpr::Eval(val, PC);
            if (found)
            {
                inputTokens.push_back(new InputToken);
                next = inputTokens.back();
                next->type = InputToken::TOKEN;
                next->val = new AsmExprNode(plus);
            }
            inputTokens.push_back(new InputToken);
            next = inputTokens.back();
            next->type = InputToken::NUMBER;
            next->val = val;
        }
    }
}
void InstructionParser::Split(const std::string& line, std::vector<std::string>& splt)
{
    int start = 0;
    int parens = 0;
    int i;
    for (i = 0; i < line.size(); i++)
    {
        if (line[i] == '(')
            parens++;
        else if (line[i] == ')' && parens)
            parens--;
        else if (line[i] == ',' && !parens)
        {
            splt.push_back(line.substr(start, i - start));
            start = i + 1;
        }
    }
    if (i > start)
        splt.push_back(line.substr(start, i - start));
    for (i = 0; i < splt.size(); i++)
    {
        int npos = splt[i].find_first_not_of("\t\v \r\n");
        if (npos && npos != std::string::npos)
            splt[i] = splt[i].substr(npos);
    }
}
bool InstructionParser::Tokenize(int& op, int PC, int& size1, int& size2)
{
    if (attSyntax)
        line = RewriteATT(op, line, size1, size2);
    while (line.size())
    {
        NextToken(PC);
        InputToken* next;
        switch (id)
        {
            case -1:
                return false;
            default:
                inputTokens.push_back(new InputToken);
                next = inputTokens.back();
                next->type = InputToken::TOKEN;
                next->val = new AsmExprNode(id);
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
