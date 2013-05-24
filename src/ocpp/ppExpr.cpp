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
#include "ppExpr.h"
#include "ppDefine.h"
#include "Errors.h"

KeywordHash ppExpr::hash;
bool ppExpr::initted;

void ppExpr::InitHash()
{
    if (!initted)
    {
        initted = true;
        hash["("] = openpa;
        hash[")"] = closepa;
        hash["+"] = plus;
        hash["-"] = minus;
        hash["!"] = not;
        hash["~"] = compl;
        hash["*"] = star;
        hash["/"] = divide;
        hash["%"] = mod;
        hash["<<"] = leftshift;
        hash[">>"] = rightshift;
        hash[">"] = gt;
        hash["<"] = lt;
        hash[">="] = geq;
        hash["<="] = leq;
        hash["=="] = eq;
        hash["!="] = ne;
        hash["|"] = or;
        hash["&"] = and;
        hash["^"] = xor;
        hash["||"] = lor;
        hash["&&"] = land;
        hash["?"] = hook;
        hash[":"] = colon;
        hash[","] = comma;
    }
}
PPINT ppExpr::Eval(std::string &line)
{
    tokenizer = new Tokenizer(line, &hash);
    token = tokenizer->Next();
    if (!token)
        return 0;
    PPINT rv = comma_(line);
    if (!token->IsEnd())
        line = token->GetChars() + tokenizer->GetString();
    else
        line = "";
    delete tokenizer;
    return rv;
}

PPINT ppExpr::primary(std::string &line)
{
    PPINT rv = 0;
    if (token->GetKeyword() == openpa)
    {
        token = tokenizer->Next();
        if (!token->IsEnd() )
        {
            rv = comma_(line);
            if (token->GetKeyword() == closepa)
                token = tokenizer->Next();
            else
                Errors::Error("Expected ')'");
        }
    }
    else if (token->IsIdentifier())
    {
        if (token->GetId() == "defined")
        {
            token = tokenizer->Next();
            if (!token->IsEnd())
            {
                bool open = false;
                if (token->GetKeyword() == openpa)
                {
                    open = true;
                    token = tokenizer->Next();
                }
                if (!token->IsEnd())
                {
                    if (token->IsIdentifier())
                    {
                        rv = !!define->Lookup(token->GetId());
                    }
                    else
                    {
                        Errors::Error("Expected identifier in defined statement");
                    }
                    if (open)
                    {
                        token = tokenizer->Next();
                        if (!token->IsEnd())
                        {
                            if (token->GetKeyword() != closepa)
                            {
                                Errors::Error("Expected ')'");
                            }
                            else
                            {
                                token = tokenizer->Next();
                            }
                        }
                    }
                }					
            }
        }
    }
    else if (token->IsNumeric())
    {
        rv = token->GetInteger();
        token = tokenizer->Next();
    }
    else if (token->IsCharacter())
    {
        rv = token->GetInteger();
        token = tokenizer->Next();
    }
    else
    {
        Errors::Error("Constant value expected");
    }
    return rv;
}
PPINT ppExpr::unary(std::string &line)
{
    if (!token->IsEnd())
    {
        int kw = token->GetKeyword();
        if (kw == plus || kw == minus || kw == not || kw == compl)
        {
            token = tokenizer->Next();
            if (!token->IsEnd())
            {
                PPINT val1 = unary(line);
                switch(kw)
                {
                    case minus:
                        val1 = - val1;
                        break;
                    case plus:
                        val1 = + val1;
                        break;
                    case not:
                        val1 = !val1;
                        break;
                    case compl:
                        val1 = ~val1;
                        break;
                }
                return val1;
            }
        }
        else
        {
            return primary(line);
        }
    }
    Errors::Error("Syntax error in constant expression");
    return 0;
}
PPINT ppExpr::multiply(std::string &line)
{
    PPINT val1 = unary(line);
    int kw = token->GetKeyword();
    while (kw == star || kw == divide || kw == mod)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = unary(line);
            switch(kw)
            {
                case star:
                    val1 *= val2;
                    break;
                case divide:
                    if (val2 == 0)
                    {
                        Errors::Error("Divide by zero in preprocessor constant");
                        val1 = 1;
                    }
                    else
                    {
                        val1 /= val2;
                    }
                    break;
                case mod:
                    if (val2 == 0)
                    {
                        Errors::Error("Divide by zero in preprocessor constant");
                        val1 = 1;
                    }
                    else
                    {
                        val1 %= val2;
                    }
                    break;
            }
        }
        kw = token->GetKeyword();
    }
    return val1;
}
PPINT ppExpr::add(std::string &line)
{
    PPINT val1 = multiply(line);
    int kw = token->GetKeyword();
    while (kw == plus || kw == minus)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = multiply(line);
            switch(kw)
            {
                case plus:
                    val1 += val2;
                    break;
                case minus:
                    val1 -= val2;
                    break;
            }
        }
        kw = token->GetKeyword();
    }
    return val1;
}
PPINT ppExpr::shift(std::string &line)
{
    PPINT val1 = add(line);
    int kw = token->GetKeyword();
    while (kw == leftshift || kw == rightshift)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = add(line);
            switch(kw)
            {
                case leftshift:
                    val1 <<= val2;
                    break;
                case rightshift:
                    val1 >>= val2;
                    break;
            }
        }
        kw = token->GetKeyword();
    }
    return val1;
}
PPINT ppExpr::relation(std::string &line)	
{
    PPINT val1 = shift(line);
    int kw = token->GetKeyword();
    while (kw == gt || kw == lt || kw == geq || kw == leq)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = shift(line);
            switch(kw)
            {
                case gt:
                    val1 = val1 > val2;
                    break;
                case lt:
                    val1 = val1 < val2;
                    break;
                case geq:
                    val1 = val1 >= val2;
                    break;
                case leq:
                    val1 = val1 <= val2;
                    break;
            }
        }
        kw = token->GetKeyword();
    }
    return val1;
}
PPINT ppExpr::equal(std::string &line)
{
    PPINT val1 = relation(line);
    int kw = token->GetKeyword();
    while (kw == eq || kw == ne)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = shift(line);
            switch(kw)
            {
                case eq:
                    val1 = val1 == val2;
                    break;
                case ne:
                    val1 = val1 != val2;
                    break;
            }
        }
        kw = token->GetKeyword();
    }
    return val1;
}
PPINT ppExpr::and_(std::string &line)
{
    PPINT val1 = equal(line);
    while (!token->IsEnd() && token->GetKeyword()== and)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = equal(line);
            val1 &= val2;
        }
    }
    return val1;
}
PPINT ppExpr::xor_(std::string &line)
{
    PPINT val1 = and_(line);
    while (!token->IsEnd() && token->GetKeyword()== xor)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = and_(line);
            val1 ^= val2;
        }
    }
    return val1;
}
PPINT ppExpr::or_(std::string &line)
{
    PPINT val1 = xor_(line);
    while (!token->IsEnd() && token->GetKeyword()== or)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = xor_(line);
            val1 |= val2;
        }
    }
    return val1;
}
PPINT ppExpr::logicaland(std::string &line)
{
    PPINT val1 = or_(line);
    while (!token->IsEnd() && token->GetKeyword()== land)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = or_(line);
            val1 = val1 && val2;
        }
    }
    return val1;
}
PPINT ppExpr::logicalor(std::string &line)
{
    PPINT val1 = logicaland(line);
    while (!token->IsEnd() && token->GetKeyword()== lor)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = logicaland(line);
            val1 = val1 || val2;
        }
    }
    return val1;
}
PPINT ppExpr::conditional(std::string &line)
{
    PPINT val1 = logicalor(line);
    if ((!token->IsEnd()) && token->GetKeyword() == hook)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = logicalor(line);
            token = tokenizer->Next();
            if (!token->IsEnd())
            {
                if (token->GetKeyword() == colon)
                {
                    token = tokenizer->Next();
                    if (!token->IsEnd())
                    {
                        PPINT val3 = conditional(line);
                        if (val1)
                            val1 = val2;
                        else
                            val2 = val3;
                    }
                }
                else
                {
                    Errors::Error("Conditional needs ':'");
                    val1 = 0;
                }
            }
        }
    }
    return val1;
}
PPINT ppExpr::comma_(std::string &line)
{
    PPINT rv = 0;
    rv = conditional(line);
    while (!token->IsEnd() && token->GetKeyword() == comma)
    {
        token = tokenizer->Next();
        if (token->IsEnd())
            break;
        (void)conditional(line);
    }
    return(rv);
}

