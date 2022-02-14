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

#include "ppExpr.h"
#include "ppDefine.h"
#include "Errors.h"
#include "ppInclude.h"
#include "ppkw.h"
#include <iostream>

KeywordHash ppExpr::hash = {
    {"(", kw::openpa},      {")", kw::closepa}, {"+", kw::plus},   {"-", kw::minus}, {"!", kw::lnot},
    {"~", kw::bcompl},      {"*", kw::star},    {"/", kw::divide}, {"%", kw::mod},   {"<<", kw::leftshift},
    {">>", kw::rightshift}, {">", kw::gt},      {"<", kw::lt},     {">=", kw::geq},  {"<=", kw::leq},
    {"==", kw::eq},         {"!=", kw::ne},     {"|", kw::bor},    {"&", kw::band},  {"^", kw::bxor},
    {"||", kw::lor},        {"&&", kw::land},   {"?", kw::hook},   {":", kw::colon}, {",", kw::comma},
};

ppExpr::CompilerExpression* ppExpr::expressionHandler;

ppInclude* ppExpr::include;

PPINT ppExpr::Eval(std::string& line, bool fromConditional)
{
    if (line.empty() || line.find_first_not_of(" \t\r\v\n") == std::string::npos)
    {
        return 0;
    }
    if (fromConditional && expressionHandler)
        return expressionHandler(line);
    floatWarned = false;
    tokenizer = std::make_unique<Tokenizer>(line, &hash);
    token = tokenizer->Next();
    if (!token)
        return 0;
    bool isunsigned = false;
    PPINT rv = comma_(line, isunsigned);
    if (!token->IsEnd())
        line = token->GetChars() + tokenizer->GetString();
    else
        line = "";
    tokenizer.release();
    return rv;
}

PPINT ppExpr::primary(std::string& line, bool& isunsigned)
{
    PPINT rv = 0;
    if (token->GetKeyword() == kw::openpa)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            rv = comma_(line, isunsigned);
            if (token->GetKeyword() == kw::closepa)
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
                if (token->GetKeyword() == kw::openpa)
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
                            if (token->GetKeyword() != kw::closepa)
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
                if (token->GetKeyword() == kw::openpa)
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

                        rv = include->has_include(arg);
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
        else if (token->GetId() == "__has_include_next")
        {
            token = tokenizer->Next();
            if (!token->IsEnd())
            {
                if (token->GetKeyword() == kw::openpa)
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

                        rv = include->has_include_next(arg);
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
            int count = 0;
            token = tokenizer->Next();
            if (token->GetKeyword() == kw::openpa)
            {
                token = tokenizer->Next();
                count++;
                while (count && !token->IsEnd())
                {
                    if (token->GetKeyword() == kw::openpa)
                        count++;
                    if (token->GetKeyword() == kw::closepa)
                        count--;
                    token = tokenizer->Next();
                }
            }
        }
    }
    else if (token->IsNumeric())
    {
        if (token->IsFloating())
        {
            if (!floatWarned)
                Errors::Error("Invalid integer constant expression");
            floatWarned = true;
        }
        rv = token->GetInteger();
        Token::Type t = token->GetNumericType();
        switch (t)
        {
            case Token::t_unsignedint:
            case Token::t_unsignedlongint:
            case Token::t_unsignedlonglongint:
                isunsigned = true;
                break;
        }
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
PPINT ppExpr::unary(std::string& line, bool& isunsigned)
{
    if (!token->IsEnd())
    {
        kw keyWord = token->GetKeyword();
        if (keyWord == kw::plus || keyWord == kw::minus || keyWord == kw::lnot || keyWord == kw::bcompl)
        {
            token = tokenizer->Next();
            if (!token->IsEnd())
            {
                PPINT val1 = unary(line, isunsigned);
                switch (keyWord)
                {
                    case kw::minus:
                        val1 = -val1;
                        break;
                    case kw::plus:
                        val1 = +val1;
                        break;
                    case kw::lnot:
                        val1 = !val1;
                        break;
                    case kw::bcompl:
                        val1 = ~val1;
                        break;
                }
                return val1;
            }
        }
        else
        {
            return primary(line, isunsigned);
        }
    }
    Errors::Error("Syntax error in constant expression");
    return 0;
}
PPINT ppExpr::multiply(std::string& line, bool& isunsigned)
{
    PPINT val1 = unary(line, isunsigned);
    kw keyWord = token->GetKeyword();
    while (keyWord == kw::star || keyWord == kw::divide || keyWord == kw::mod)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = unary(line, isunsigned);
            switch (keyWord)
            {
                case kw::star:
                    if (isunsigned)
                        val1 = (PPUINT)val1 * val2;
                    else
                        val1 *= val2;
                    break;
                case kw::divide:
                    if (val2 == 0)
                    {
                        val1 = 0;
                    }
                    else
                    {
                        if (isunsigned)
                            val1 = (PPUINT)val1 / val2;
                        else
                            val1 /= val2;
                    }
                    break;
                case kw::mod:
                    if (val2 == 0)
                    {
                        val1 = 0;
                    }
                    else
                    {
                        if (isunsigned)
                            val1 = (PPUINT)val1 % val2;
                        else
                            val1 %= val2;
                    }
                    break;
            }
        }
        keyWord = token->GetKeyword();
    }
    return val1;
}
PPINT ppExpr::add(std::string& line, bool& isunsigned)
{
    PPINT val1 = multiply(line, isunsigned);
    kw keyWord = token->GetKeyword();
    while (keyWord == kw::plus || keyWord == kw::minus)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = multiply(line, isunsigned);
            switch (keyWord)
            {
                case kw::plus:
                    val1 += val2;
                    break;
                case kw::minus:
                    val1 -= val2;
                    break;
            }
        }
        keyWord = token->GetKeyword();
    }
    return val1;
}
PPINT ppExpr::shift(std::string& line, bool& isunsigned)
{
    PPINT val1 = add(line, isunsigned);
    kw keyWord = token->GetKeyword();
    while (keyWord == kw::leftshift || keyWord == kw::rightshift)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            bool test = false;
            PPINT val2 = add(line, test);
            switch (keyWord)
            {
                case kw::leftshift:
                    val1 <<= val2;
                    break;
                case kw::rightshift:
                    val1 >>= val2;
                    break;
            }
        }
        keyWord = token->GetKeyword();
    }
    return val1;
}
PPINT ppExpr::relation(std::string& line, bool& isunsigned)
{
    PPINT val1 = shift(line, isunsigned);
    kw keyWord = token->GetKeyword();
    while (keyWord == kw::gt || keyWord == kw::lt || keyWord == kw::geq || keyWord == kw::leq)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = shift(line, isunsigned);
            switch (keyWord)
            {
                case kw::gt:
                    if (isunsigned)
                        val1 = (PPUINT)val1 > val2;
                    else
                        val1 = val1 > val2;
                    break;
                case kw::lt:
                    if (isunsigned)
                        val1 = (PPUINT)val1 < val2;
                    else
                        val1 = val1 < val2;
                    break;
                case kw::geq:
                    if (isunsigned)
                        val1 = (PPUINT)val1 >= val2;
                    else
                        val1 = val1 >= val2;
                    break;
                case kw::leq:
                    if (isunsigned)
                        val1 = (PPUINT)val1 <= val2;
                    else
                        val1 = val1 <= val2;
                    break;
            }
        }
        keyWord = token->GetKeyword();
    }
    return val1;
}
PPINT ppExpr::equal(std::string& line, bool& isunsigned)
{
    PPINT val1 = relation(line, isunsigned);
    kw keyWord = token->GetKeyword();
    while (keyWord == kw::eq || keyWord == kw::ne)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = shift(line, isunsigned);
            switch (keyWord)
            {
                case kw::eq:
                    val1 = val1 == val2;
                    break;
                case kw::ne:
                    val1 = val1 != val2;
                    break;
            }
        }
        keyWord = token->GetKeyword();
    }
    return val1;
}
PPINT ppExpr::and_(std::string& line, bool& isunsigned)
{
    PPINT val1 = equal(line, isunsigned);
    while (!token->IsEnd() && token->GetKeyword() == kw::band)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = equal(line, isunsigned);
            val1 &= val2;
        }
    }
    return val1;
}
PPINT ppExpr::xor_(std::string& line, bool& isunsigned)
{
    PPINT val1 = and_(line, isunsigned);
    while (!token->IsEnd() && token->GetKeyword() == kw::bxor)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = and_(line, isunsigned);
            val1 ^= val2;
        }
    }
    return val1;
}
PPINT ppExpr::or_(std::string& line, bool& isunsigned)
{
    PPINT val1 = xor_(line, isunsigned);
    while (!token->IsEnd() && token->GetKeyword() == kw::bor)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = xor_(line, isunsigned);
            val1 |= val2;
        }
    }
    return val1;
}
PPINT ppExpr::logicaland(std::string& line, bool& isunsigned)
{
    PPINT val1 = or_(line, isunsigned);
    while (!token->IsEnd() && token->GetKeyword() == kw::land)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = or_(line, isunsigned);
            val1 = val1 && val2;
        }
    }
    return val1;
}
PPINT ppExpr::logicalor(std::string& line, bool& isunsigned)
{
    PPINT val1 = logicaland(line, isunsigned);
    while (!token->IsEnd() && token->GetKeyword() == kw::lor)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = logicaland(line, isunsigned);
            val1 = val1 || val2;
        }
    }
    return val1;
}
PPINT ppExpr::conditional(std::string& line, bool& isunsigned)
{
    bool test = false;
    PPINT val1 = logicalor(line, test);
    if ((!token->IsEnd()) && token->GetKeyword() == kw::hook)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            PPINT val2 = comma_(line, isunsigned);
            if (!token->IsEnd())
            {
                if (token->GetKeyword() == kw::colon)
                {
                    token = tokenizer->Next();
                    if (!token->IsEnd())
                    {
                        PPINT val3 = conditional(line, isunsigned);
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
    else
    {
        isunsigned |= test;
    }
    return val1;
}
PPINT ppExpr::comma_(std::string& line, bool& isunsigned)
{
    PPINT rv = 0;
    rv = conditional(line, isunsigned);
    while (!token->IsEnd() && token->GetKeyword() == kw::comma)
    {
        token = tokenizer->Next();
        if (token->IsEnd())
            break;
        (void)conditional(line, isunsigned);
    }
    return (rv);
}
