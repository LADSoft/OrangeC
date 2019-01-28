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

#include "ppExpr.h"
#include "ppDefine.h"
#include "Errors.h"
#include "ppInclude.h"

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
        hash["!"] = lnot;
        hash["~"] = bcompl;
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
        hash["|"] = bor;
        hash["&"] = band;
        hash["^"] = bxor;
        hash["||"] = lor;
        hash["&&"] = land;
        hash["?"] = hook;
        hash[":"] = colon;
        hash[","] = comma;
    }
}
PPINT ppExpr::Eval(std::string& line)
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

PPINT ppExpr::primary(std::string& line)
{
    PPINT rv = 0;
    if (token->GetKeyword() == openpa)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
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
        else if (token->GetId() == "__has_include")
        {
            token = tokenizer->Next();
            if (!token->IsEnd())
            {
                if (token->GetKeyword() == openpa)
                {
                    std::string line = tokenizer->GetString();
                    int n = line.find(")");
                    if (n == std::string::npos)
                    {
                        Errors::Error("Expected ')'");
                    }
                    else
                    {
                        std::string arg = line.substr(0, n);
                        define->Process(arg);

                        rv = ppInclude::has_include(arg);
                        tokenizer->SetString(line.substr(n + 1));
                        token = tokenizer->Next();
                    }
                }
                else
                {
                    Errors::Error("Expected '('");
                }
            }
        }
        else
        {
            rv = 0;
            token = tokenizer->Next();
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
PPINT ppExpr::unary(std::string& line)
{
    if (!token->IsEnd())
    {
        int kw = token->GetKeyword();
        if (kw == plus || kw == minus || kw == lnot || kw == bcompl)
        {
            token = tokenizer->Next();
            if (!token->IsEnd())
            {
                PPINT val1 = unary(line);
                switch (kw)
                {
                    case minus:
                        val1 = -val1;
                        break;
                    case plus:
                        val1 = +val1;
                        break;
                    case lnot:
                        val1 = !val1;
                        break;
                    case bcompl:
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
PPINT ppExpr::multiply(std::string& line)
{
    PPINT val1 = unary(line);
    int kw = token->GetKeyword();
    while (kw == star || kw == divide || kw == mod)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = unary(line);
            switch (kw)
            {
                case star:
                    val1 *= val2;
                    break;
                case divide:
                    if (val2 == 0)
                    {
                        val1 = 0;
                    }
                    else
                    {
                        val1 /= val2;
                    }
                    break;
                case mod:
                    if (val2 == 0)
                    {
                        val1 = 0;
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
PPINT ppExpr::add(std::string& line)
{
    PPINT val1 = multiply(line);
    int kw = token->GetKeyword();
    while (kw == plus || kw == minus)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = multiply(line);
            switch (kw)
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
PPINT ppExpr::shift(std::string& line)
{
    PPINT val1 = add(line);
    int kw = token->GetKeyword();
    while (kw == leftshift || kw == rightshift)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = add(line);
            switch (kw)
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
PPINT ppExpr::relation(std::string& line)
{
    PPINT val1 = shift(line);
    int kw = token->GetKeyword();
    while (kw == gt || kw == lt || kw == geq || kw == leq)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = shift(line);
            switch (kw)
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
PPINT ppExpr::equal(std::string& line)
{
    PPINT val1 = relation(line);
    int kw = token->GetKeyword();
    while (kw == eq || kw == ne)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = shift(line);
            switch (kw)
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
PPINT ppExpr::and_(std::string& line)
{
    PPINT val1 = equal(line);
    while (!token->IsEnd() && token->GetKeyword() == band)
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
PPINT ppExpr::xor_(std::string& line)
{
    PPINT val1 = and_(line);
    while (!token->IsEnd() && token->GetKeyword() == bxor)
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
PPINT ppExpr::or_(std::string& line)
{
    PPINT val1 = xor_(line);
    while (!token->IsEnd() && token->GetKeyword() == bor)
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
PPINT ppExpr::logicaland(std::string& line)
{
    PPINT val1 = or_(line);
    while (!token->IsEnd() && token->GetKeyword() == land)
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
PPINT ppExpr::logicalor(std::string& line)
{
    PPINT val1 = logicaland(line);
    while (!token->IsEnd() && token->GetKeyword() == lor)
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
PPINT ppExpr::conditional(std::string& line)
{
    PPINT val1 = logicalor(line);
    if ((!token->IsEnd()) && token->GetKeyword() == hook)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = comma_(line);
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
                            val1 = val3;
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
PPINT ppExpr::comma_(std::string& line)
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
    return (rv);
}
