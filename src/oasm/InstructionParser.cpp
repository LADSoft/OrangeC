/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#include "InstructionParser.h"
#include "Errors.h"
#include <ctype.h>
#include <fstream>
#include <stdio.h>
#include "Instruction.h"
#include "Fixup.h"
#include "UTF8.h"
#include <stdexcept>

extern bool IsSymbolStartChar(char ch);
extern bool IsSymbolChar(char ch);

static const unsigned mask[32] = { 
    0x1, 0x3,0x7,0xf, 0x1f,0x3f,0x7f,0xff,
    0x1ff, 0x3ff,0x7ff,0xfff, 0x1fff,0x3fff,0x7fff,0xffff,
    0x1ffff, 0x3ffff,0x7ffff,0xfffff, 0x1fffff,0x3fffff,0x7fffff,0xffffff,
    0x1ffffff, 0x3ffffff,0x7ffffff,0xfffffff, 0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff,
};
int CodingHelper::DoMath(int val)
{
    switch(math)
    {
        case '!':
            return -val;
        case '~':
            return ~val;
        case '+':
            return val + mathval;
        case '-':
            return val - mathval;
        case '>':
            return val >> mathval;
        case '<':
            return val << mathval;
        case '&':
            return val & mathval;
        case '|':
            return val | mathval;
        case '^':
            return val ^ mathval;
        default:
            return val;
    }
}
void BitStream::Add(int val, int cnt)
{
    val &= mask[cnt-1];
    int v = 8 - (bits &7);
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
                    if (bits == 8 && (val->ival & ~mask[bits-2]) != 0)
                        if ((val->ival & ~mask[bits-2]) != (~mask[bits-2]))
                            if ((val->ival & ~mask[bits-2]) != 0)
                                return false;
                }
                else
                {
                    if (bits == 8 && (val->ival & ~mask[bits-1]) != 0)
                        if ((val->ival & ~mask[bits-2]) != (~mask[bits-2]))
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
Instruction *InstructionParser::Parse(const std::string args, int PC)
{
    int errLine = Errors::GetErrorLine();
    std::string errName = Errors::GetFileName();
    line = args;
    std::string op;
    prefixes.clear();
    for (std::deque<Coding *>::iterator it = CleanupValues.begin(); it != CleanupValues.end(); ++ it)
    {
        Coding *aa = (*it);
        delete aa; 
    }
    for (std::deque<Numeric *>::iterator it = operands.begin(); it != operands.end(); ++it)
    {
        Numeric *aa = (*it);
        delete aa;
    }
    for (int i = 0; i < inputTokens.size(); ++i)
    {
        InputToken *t = inputTokens[i];
        delete t;
    }
    inputTokens.clear();
    CleanupValues.clear();
    operands.clear();
    numeric = NULL;
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
        std::map<std::string, int, lt>::iterator it = prefixTable.find(op);
        if (it != prefixTable.end())
        {
            prefixes.push_back(it->second);
        }
        else
        {
            break;
        }
    }
    if (op == "")
    {
        bool rv = DispatchOpcode(-1);
        if (rv)
        {
            unsigned char buf[32];
            bits.GetBytes(buf, 32);
            return new Instruction(buf, (bits.GetBits() + 7)/8);
        }
    }
    else
    {
        std::map<std::string, int, lt>::iterator it = opcodeTable.find(op);
        if (it != opcodeTable.end())
        {
            bits.Reset();
            if (!Tokenize(PC))
            {
                throw new std::runtime_error("Syntax error");
            }
            eol = false;
            bool rv = DispatchOpcode(it->second);
            Instruction *s = NULL;
            if (rv)
            {
                unsigned char buf[32];
                bits.GetBytes(buf, 32);
    #ifdef XXXXX
                std::cout << std::hex << bits.GetBits() << " ";
                for (int i=0; i < bits.GetBits() >> 3; i++)
                    std::cout << std::hex << (int)buf[i] << " ";
                std::cout << std::endl;
    #endif
                if (!eol)
                    throw new std::runtime_error("Extra characters at end of line");
                s = new Instruction(buf, (bits.GetBits() + 7)/8);
    //			std::cout << bits.GetBits() << std::endl;
                for (std::deque<Numeric *>::iterator it = operands.begin(); it != operands.end(); ++it)
                {
                    if ((*it)->used && (*it)->size)
                    {
                        int n = (*it)->relOfs;
                        if (n < 0)
                            n = - n;
                        Fixup *f = new Fixup((*it)->node, ((*it)->size + 7)/8, (*it)->relOfs != 0, n, (*it)->relOfs>0);
                        f->SetInsOffs(((*it)->pos + 7)/8);
                        f->SetFileName(errName);
                        f->SetErrorLine(errLine);
                        s->Add(f);
                    }
                }
            }
            else
            {
                throw new std::runtime_error("Syntax error");
            }
            return s;
        }
    }
    throw new std::runtime_error("Unrecognized opcode");
}
void InstructionParser::RenameRegisters(AsmExprNode *val)
{
    if (val->GetType() == AsmExprNode::LABEL)
    {
        std::map<std::string, int, lt>::iterator it = tokenTable.find(val->label);
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
AsmExprNode *InstructionParser::ExtractReg(AsmExprNode **val)
{
    AsmExprNode *rv = NULL;
    switch ((*val)->GetType())
    {
        case AsmExprNode::REG:
            rv = *val;
            *val = NULL;
            break;
        case AsmExprNode::ADD:
            if ((*val)->GetLeft()->GetType() == AsmExprNode::REG)
            {
                rv = (*val)->GetLeft();
                AsmExprNode *todel = (*val);
                *val = (*val)->GetRight();
                todel->SetLeft(NULL);
                todel->SetRight(NULL);
                delete todel;
            }
            else if ((*val)->GetRight()->GetType() == AsmExprNode::REG)
            {
                rv = (*val)->GetRight();
                AsmExprNode *todel = (*val);
                *val = (*val)->GetLeft();
                todel->SetLeft(NULL);
                todel->SetRight(NULL);
                delete todel;
            }
            else
            {
                AsmExprNode *v = (*val)->GetLeft();
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
                (*val)->SetRight(NULL);
                (*val)->SetType(AsmExprNode::NEG);
            }
            else
            {
                AsmExprNode *v = (*val)->GetLeft();
                rv = ExtractReg(&v);
                if (rv)
                {
                    (*val)->SetLeft(v);
                }
            }
            break;
    }
    return rv;
}
bool InstructionParser::MatchesTimes(AsmExprNode *val)
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
AsmExprNode *InstructionParser::ExtractTimes(AsmExprNode **val)
{
    AsmExprNode *rv = NULL;
    switch ((*val)->GetType())
    {
        case AsmExprNode::MUL:
            if (MatchesTimes(*val))
            {
                rv = *val;
                *val = NULL;
            }
            break;
        case AsmExprNode::ADD:
            if (MatchesTimes((*val)->GetLeft()))
            {
                rv = (*val)->GetLeft();
                AsmExprNode *todel = (*val);
                *val = (*val)->GetRight();
                todel->SetLeft(NULL);
                todel->SetRight(NULL);
                delete todel;
            }
            else if (MatchesTimes((*val)->GetRight()))
            {
                rv = (*val)->GetRight();
                AsmExprNode *todel = (*val);
                *val = (*val)->GetLeft();
                todel->SetLeft(NULL);
                todel->SetRight(NULL);
                delete todel;
            }
            else 
            {
                AsmExprNode *v = (*val)->GetLeft();
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
                (*val)->SetRight(NULL);
                (*val)->SetType(AsmExprNode::NEG);
            }
            else
            {
                AsmExprNode *v = (*val)->GetLeft();
                rv = ExtractTimes(&v);
                if (rv)
                {
                    (*val)->SetLeft(v);
                }
            }
            break;
    }
    return rv;
}
bool InstructionParser::CheckRegs(AsmExprNode *val)
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
    std::map<std::string, int, lt>::iterator it = tokenTable.find("+");
    if (it != tokenTable.end())
        plus = it->second;
    it = tokenTable.find("*");
    if (it != tokenTable.end())
        times = it->second;
    RenameRegisters(val);
    if (val->GetType() == AsmExprNode::LABEL)
    {
        std::map<std::string, int, lt>::iterator it = tokenTable.find(val->label);
        InputToken *next;
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
        AsmExprNode *cur = ExtractReg(&val);
        InputToken *next ;
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
                AsmExprNode *reg;
                AsmExprNode *mul;
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
        InputToken *next;
        switch(id)
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
                next = NULL;
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
    return line.size() && (isdigit(line[0]) || line[0] == '+' || line[0] == '-' ||
                         line[0] == '~' || line[0] == '!' || line[0] == '(');
}
void InstructionParser::NextToken(int PC)
{
    id = -1;
    bool negate = false;
    int npos = line.find_first_not_of(" \t\r\n\v");
    if (npos==std::string::npos)
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
            for (i=1; i < line.size(); i++)
            {
                if (line[i] == quote)
                    break;
                if (i < 5) // endianness & sizing
                    accum += line[i] << ((i-1) * 8);
            }
            if (i == line.size())
                throw new std::runtime_error("Expected end quote");
            if (i == 1)
                throw new std::runtime_error("Quoted string is empty");
            line.erase(0, i+1);
            id = TK_NUMERIC;
            val = new AsmExprNode(accum);
        }
        else if (IsSymbolStartChar(line.c_str()) || IsNumber() || line[0] == '$')
        {
            val = expr.Build(line);
            id = TK_NUMERIC;
        }
        else if (ispunct(line[0]))
        {
            token = line[0];
            line.erase(0, 1);
            std::map<std::string, int, lt>::iterator it = tokenTable.find(token);
            if (it != tokenTable.end())
            {
                id = it->second;
            }
        }
    }
}
