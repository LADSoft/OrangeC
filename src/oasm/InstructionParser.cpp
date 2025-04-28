/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
extern bool IsSymbolCharRoutine(const char*, bool);

bool InstructionParser::ParseNumber(int relOfs, int sign, int bits, int needConstant, int tokenPos)
{
    if (inputTokens[tokenPos]->type == InputToken::NUMBER)
    {
        val = std::make_shared<AsmExprNode>(*(AsmExprNode*)inputTokens[tokenPos]->val);
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
            numeric->node = new AsmExprNode(*val.get());
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
        val = std::make_shared<AsmExprNode>(*(AsmExprNode*)inputTokens[tokenPos]->val);
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
std::shared_ptr<Instruction> InstructionParser::Parse(const std::string& args, int PC)
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
            return std::make_shared<Instruction>(buf, (bits.GetBits() + 7) / 8);
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
            std::shared_ptr<Instruction> s = nullptr;
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
                    s = std::make_shared<Instruction>(buf, (bits.GetBits() + 7) / 8);
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
                                auto temp = std::make_shared<AsmExprNode>(*(AsmExprNode*)operand->node);
                                auto f = std::make_shared<Fixup>(temp, (operand->size + 7) / 8, operand->relOfs != 0, n,
                                                                 operand->relOfs > 0);
                                f->SetInsOffs((operand->pos + 7) / 8);
                                f->SetFileName(errName);
                                f->SetErrorLine(errLine);
                                s->Add(std::move(f));
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
    auto plus = tokenTable.find("+");

    int npos = line.find_first_not_of(" \t\r\n\v");
    if (npos == std::string::npos)
        line = "";
    else if (npos != 0)
        line = line.substr(npos);
    if (attSyntax)
        line = RewriteATT(op, line, size1, size2);
    size_t n = std::string::npos;
    size_t m = std::string::npos;
    char quotechar = 0, lastchar = 0;
    int i = 0;
    for (auto c : line)
    {
        if (quotechar)
        {
            if (c == quotechar)
                quotechar = 0;
        }
        else
        {
            if (c == '[')
                if (n == std::string::npos)
                    n = i;
            if (c == ':' && n != std::string::npos && m == std::string::npos)
                n = i;
            if (c == ']' && m == std::string::npos)
                m = i;
            if (c == '\'' || c == '"')
                quotechar = c;
        }
        i++;
        lastchar = c;
    }

    std::deque<std::string> regs;
    if (n != std::string::npos)
    {
        n++;
        if (m == std::string::npos)
            throw new std::runtime_error("missing ']");
        m = n;
        while (m < line.size() && line[m] != ']')
        {
            while (m < line.size() && isspace(line[m]))
                m++;
            if (IsSymbolCharRoutine(line.c_str() + m, true))
            {
                // registter candidate
                size_t q = m + 1;
                while (q < line.size() && IsSymbolCharRoutine(line.c_str() + q, false))
                    q++;

                std::string potentialreg = line.substr(m, q - m);
                std::transform(potentialreg.begin(), potentialreg.end(), potentialreg.begin(), ::tolower);
                auto it = tokenTable.find(potentialreg);
                if (it != tokenTable.end() && it->second >= REGISTER_BASE)
                {
                    // is registera
                    regs.push_back(std::move(potentialreg));
                    while (q < line.size() && isspace(line[q]))
                        q++;
                    if (q < line.size() && line[q] == '*')
                    {
                        // scaled register, standard format
                        size_t r = q + 1;
                        while (r < line.size() && isspace(line[r]))
                            r++;
                        if (r >= line.size() || !isdigit(line[r]))
                            throw new std::runtime_error("scale specifier required");
                        while (r < line.size() && isdigit(line[r]))
                            r++;
                        regs.back() += line.substr(q, r - q);
                        line.erase(m, r - m);
                        r = m - 1;
                        while (r > n && isspace(line[r]))
                            r--;
                        if (r >= n)
                        {
                            if (line[r] != '+' && !isspace(line[r]))
                                throw new std::runtime_error("register math not allowed");
                            if (line[r] == '+')
                                ;
                            {
                                line.erase(r, 1);
                                m--;
                            }
                        }
                    }
                    else
                    {
                        size_t r = m - 1;
                        while (r >= n && isspace(line[r]))
                            r--;
                        if (r >= n && line[r] == '*')
                        {
                            // scaled register, reverse format
                            r--;
                            regs.back() += "*";
                            while (r >= n && isspace(line[r]))
                                r--;
                            size_t s = r;
                            if (q == std::string::npos || !isdigit(line[s]))
                                throw new std::runtime_error("scale specifier required");

                            while (s > 0 && isdigit(line[s - 1]))
                                s--;
                            regs.back() += line.substr(s, r - s + 1);
                            line.erase(r, q - r);
                            m = r;

                            r--;
                            while (r > n && isspace(line[r]))
                                r--;
                            if (r >= n)
                            {
                                if (line[r] != '+')
                                    throw new std::runtime_error("register math not allowed");
                                if (line[r] == '+')
                                    ;
                                {
                                    line.erase(r, 1);
                                    m--;
                                }
                            }
                        }
                        else
                        {
                            // just a register
                            line.erase(m, q - m);
                            r = m - 1;
                            while (r > n && isspace(line[r]))
                                r--;
                            if (r >= n)
                            {
                                if (line[r] != '+' && !isspace(line[r]))
                                    throw new std::runtime_error("register math not allowed");
                                if (line[r] == '+')
                                    ;
                                {
                                    line.erase(r, 1);
                                    m--;
                                }
                            }
                        }
                    }
                }
                else
                {
                    // a label name
                    m = q;
                }
            }
            else
            {
                if (isdigit(line[m]))
                    while (isalnum(line[m]))
                        m++;
                else
                    m++;
                // any other character
            }
        }
        if (regs.size() > 2)
            throw new std::runtime_error("too many index registers");
        if (regs.size() == 2)
        {
            if (regs.begin()->find_first_of('*') != std::string::npos || regs.back() == "esp")
            {
                // put the scaled index register last or an esp first
                regs.push_back(regs.front());
                regs.pop_front();
            }
        }
    }
    if (n == std::string::npos)
    {
        InsertTokens(std::move(line), PC, false);
    }
    else
    {
        InsertTokens(std::move(line.substr(0, n)), PC);
        int first = true;
        for (auto r : regs)
        {
            if (!first)
            {
                inputTokens.push_back(new InputToken);
                auto next = inputTokens.back();
                next->type = InputToken::TOKEN;
                next->val = new AsmExprNode(plus->second);
            }
            first = false;
            InsertTokens(std::move(r), PC);
        }
        size_t m = line.find_first_of(']', n);
        if (m != n)
        {
            size_t q = n;
            while (isspace(line[q]))
                q++;
            if (regs.size() && (line[q] != ']' && line[q] != '+' && line[q] != '-'))
            {
                line.insert(n, "+");
                m++;
            }
            InsertTokens(std::move(line.substr(n, m - n)), PC);
        }
        InsertTokens(std::move(line.substr(m)), PC);
    }

    return true;
}
void InstructionParser::InsertTokens(std::string&& line, int PC, bool hasBrackets)
{
    char lastChar = 0;

    while (line.size())
    {
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
                inputTokens.push_back(new InputToken);
                auto next = inputTokens.back();
                next->type = InputToken::NUMBER;
                next->val = new AsmExprNode(*std::make_shared<AsmExprNode>(accum).get());
            }
            else if (IsSymbolCharRoutine(line.c_str(), true))
            {
                while (n < line.size() && IsSymbolCharRoutine(line.c_str() + n, false))
                    n++;
                std::string id = line.substr(0, n);
                std::transform(id.begin(), id.end(), id.begin(), ::tolower);
                auto it = tokenTable.find(id);
                if (it != tokenTable.end())
                {
                    if (it->second >= 1000)
                    {
                        inputTokens.push_back(new InputToken);
                        auto next = inputTokens.back();
                        next->type = InputToken::REGISTER;
                        next->val = new AsmExprNode(it->second - REGISTER_BASE);
                    }
                    else
                    {
                        inputTokens.push_back(new InputToken);
                        auto next = inputTokens.back();
                        next->type = InputToken::TOKEN;
                        next->val = new AsmExprNode(it->second);
                    }
                    line.erase(0, n);
                }
                else
                {
                    inputTokens.push_back(new InputToken);
                    auto next = inputTokens.back();
                    next->type = InputToken::NUMBER;
                    auto temp = asmexpr.Build(line);
                    next->val = new AsmExprNode(*temp.get());
                    if (temp->IsAbsolute())
                        next->val = new AsmExprNode(*asmexpr.Eval(std::move(temp), PC).get());
                }
            }
            else if (isdigit(line[0]) || line[0] == '+' || line[0] == '-' || line[0] == '(' || line[0] == '$' || line[0] == '~')
            {
                if ((lastChar != 0 || hasBrackets) && lastChar != ',' && lastChar != '[' && (line[0] == '+' || line[0] == '-'))
                {
                    inputTokens.push_back(new InputToken);
                    auto next = inputTokens.back();
                    next->type = InputToken::TOKEN;
                    next->val = new AsmExprNode(tokenTable.find("+")->second);
                }
                inputTokens.push_back(new InputToken);
                auto next = inputTokens.back();
                next->type = InputToken::NUMBER;
                auto temp = asmexpr.Build(line);
                if (temp->IsAbsolute())
                    next->val = new AsmExprNode(*asmexpr.Eval(std::move(temp), PC).get());
                else
                    next->val = new AsmExprNode(*temp.get());
            }
            else if (ispunct(line[0]))
            {
                lastChar = line[0];
                std::string token;
                token += line[0];
                line.erase(0, 1);
                auto it = tokenTable.find(token);
                if (it != tokenTable.end())
                {
                    inputTokens.push_back(new InputToken);
                    auto next = inputTokens.back();
                    next->type = InputToken::TOKEN;
                    next->val = new AsmExprNode(it->second);
                }
                else
                {
                    throw new std::runtime_error(std::string("Unexpected token: ") + std::string(std::move(token)));
                }
            }
            else
            {
                throw new std::runtime_error(std::string("Unexpected character: ") + line[0]);
            }
        }
    }
}
