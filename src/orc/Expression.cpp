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

#include "Errors.h"
#include "Expression.h"
#include <stdexcept>

int Expression::primary()
{
    int rv = 0;
    if (lexer.GetToken()->GetKeyword() == kw::openpa)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            rv = Eval();
            if (lexer.GetToken()->GetKeyword() == kw::closepa)
                lexer.NextToken();
            else
                throw std::runtime_error("Expected ')'");
        }
    }
    else if (lexer.GetToken()->IsNumeric())
    {
        rv = lexer.GetToken()->GetInteger();
        lexer.NextToken();
    }
    else if (lexer.GetToken()->IsCharacter())
    {
        rv = lexer.GetToken()->GetInteger();
        lexer.NextToken();
    }
    else
    {
        throw std::runtime_error("Constant value expected");
    }
    return rv;
}
int Expression::unary()
{
    if (lexer.GetToken() != nullptr)
    {
        kw keyWord = lexer.GetToken()->GetKeyword();
        if (keyWord == kw::plus || keyWord == kw::minus || keyWord == kw::lnot || keyWord == kw::bcompl)
        {
            lexer.NextToken();
            if (lexer.GetToken() != nullptr)
            {
                int val1 = unary();
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
            return primary();
        }
    }
    throw std::runtime_error("Syntax error in constant expression");
}
int Expression::multiply()
{
    int val1 = unary();
    kw keyWord = lexer.GetToken()->GetKeyword();
    while (keyWord == kw::star || keyWord == kw::divide || keyWord == kw::mod)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            int val2 = unary();
            switch (keyWord)
            {
                case kw::star:
                    val1 *= val2;
                    break;
                case kw::divide:
                    if (val2 == 0)
                    {
                        throw std::runtime_error("Divide by zero in preprocessor constant");
                    }
                    else
                    {
                        val1 /= val2;
                    }
                    break;
                case kw::mod:
                    if (val2 == 0)
                    {
                        throw std::runtime_error("Divide by zero in preprocessor constant");
                    }
                    else
                    {
                        val1 %= val2;
                    }
                    break;
            }
        }
        keyWord = lexer.GetToken()->GetKeyword();
    }
    return val1;
}
int Expression::add()
{
    int val1 = multiply();
    kw keyWord = lexer.GetToken()->GetKeyword();
    while (keyWord == kw::plus || keyWord == kw::minus)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            int val2 = multiply();
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
        keyWord = lexer.GetToken()->GetKeyword();
    }
    return val1;
}
int Expression::shift()
{
    int val1 = add();
    kw keyWord = lexer.GetToken()->GetKeyword();
    while (keyWord == kw::leftshift || keyWord == kw::rightshift)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            int val2 = add();
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
        keyWord = lexer.GetToken()->GetKeyword();
    }
    return val1;
}
int Expression::relation()
{
    int val1 = shift();
    kw keyWord = lexer.GetToken()->GetKeyword();
    while (keyWord == kw::gt || keyWord == kw::lt || keyWord == kw::geq || keyWord == kw::leq)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            int val2 = shift();
            switch (keyWord)
            {
                case kw::gt:
                    val1 = val1 > val2;
                    break;
                case kw::lt:
                    val1 = val1 < val2;
                    break;
                case kw::geq:
                    val1 = val1 >= val2;
                    break;
                case kw::leq:
                    val1 = val1 <= val2;
                    break;
            }
        }
        keyWord = lexer.GetToken()->GetKeyword();
    }
    return val1;
}
int Expression::equal()
{
    int val1 = relation();
    kw keyWord = lexer.GetToken()->GetKeyword();
    while (keyWord == kw::eq || keyWord == kw::ne)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            int val2 = shift();
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
        keyWord = lexer.GetToken()->GetKeyword();
    }
    return val1;
}
int Expression::and_()
{
    int val1 = equal();
    while (lexer.GetToken() != nullptr && lexer.GetToken()->GetKeyword() == kw::band)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            int val2 = equal();
            val1 &= val2;
        }
    }
    return val1;
}
int Expression::xor_()
{
    int val1 = and_();
    while (lexer.GetToken() != nullptr && lexer.GetToken()->GetKeyword() == kw::bxor)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            int val2 = and_();
            val1 ^= val2;
        }
    }
    return val1;
}
int Expression::or_()
{
    int val1 = xor_();
    while (lexer.GetToken() != nullptr && lexer.GetToken()->GetKeyword() == kw::bor)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            int val2 = xor_();
            val1 |= val2;
        }
    }
    return val1;
}
int Expression::logicaland()
{
    int val1 = or_();
    while (lexer.GetToken() != nullptr && lexer.GetToken()->GetKeyword() == kw::land)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            int val2 = or_();
            val1 = val1 && val2;
        }
    }
    return val1;
}
int Expression::logicalor()
{
    int val1 = logicaland();
    while (lexer.GetToken() != nullptr && lexer.GetToken()->GetKeyword() == kw::lor)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            int val2 = logicaland();
            val1 = val1 || val2;
        }
    }
    return val1;
}
int Expression::conditional()
{
    int val1 = logicalor();
    if ((lexer.GetToken() != nullptr) && lexer.GetToken()->GetKeyword() == kw::hook)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            int val2 = logicalor();
            lexer.NextToken();
            if (lexer.GetToken() != nullptr)
            {
                if (lexer.GetToken()->GetKeyword() == kw::colon)
                {
                    lexer.NextToken();
                    if (lexer.GetToken() != nullptr)
                    {
                        int val3 = conditional();
                        if (val1)
                            val1 = val2;
                        else
                            val2 = val3;
                    }
                }
                else
                {
                    throw std::runtime_error("Conditional needs ':'");
                }
            }
        }
    }
    return val1;
}
