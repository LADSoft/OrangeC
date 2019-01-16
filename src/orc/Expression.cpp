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

#include "Errors.h"
#include "Expression.h"
#include <stdexcept>

int Expression::primary()
{
    int rv = 0;
    if (lexer.GetToken()->GetKeyword() == Lexer::openpa)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            rv = Eval();
            if (lexer.GetToken()->GetKeyword() == Lexer::closepa)
                lexer.NextToken();
            else
                throw new std::runtime_error("Expected ')'");
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
        throw new std::runtime_error("Constant value expected");
    }
    return rv;
}
int Expression::unary()
{
    if (lexer.GetToken() != nullptr)
    {
        int kw = lexer.GetToken()->GetKeyword();
        if (kw == Lexer::plus || kw == Lexer::minus || kw == Lexer::lnot || kw == Lexer::bcompl)
        {
            lexer.NextToken();
            if (lexer.GetToken() != nullptr)
            {
                int val1 = unary();
                switch (kw)
                {
                    case Lexer::minus:
                        val1 = -val1;
                        break;
                    case Lexer::plus:
                        val1 = +val1;
                        break;
                    case Lexer::lnot:
                        val1 = !val1;
                        break;
                    case Lexer::bcompl:
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
    throw new std::runtime_error("Syntax error in constant expression");
}
int Expression::multiply()
{
    int val1 = unary();
    int kw = lexer.GetToken()->GetKeyword();
    while (kw == Lexer::star || kw == Lexer::divide || kw == Lexer::mod)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            int val2 = unary();
            switch (kw)
            {
                case Lexer::star:
                    val1 *= val2;
                    break;
                case Lexer::divide:
                    if (val2 == 0)
                    {
                        throw new std::runtime_error("Divide by zero in preprocessor constant");
                    }
                    else
                    {
                        val1 /= val2;
                    }
                    break;
                case Lexer::mod:
                    if (val2 == 0)
                    {
                        throw new std::runtime_error("Divide by zero in preprocessor constant");
                    }
                    else
                    {
                        val1 %= val2;
                    }
                    break;
            }
        }
        kw = lexer.GetToken()->GetKeyword();
    }
    return val1;
}
int Expression::add()
{
    int val1 = multiply();
    int kw = lexer.GetToken()->GetKeyword();
    while (kw == Lexer::plus || kw == Lexer::minus)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            int val2 = multiply();
            switch (kw)
            {
                case Lexer::plus:
                    val1 += val2;
                    break;
                case Lexer::minus:
                    val1 -= val2;
                    break;
            }
        }
        kw = lexer.GetToken()->GetKeyword();
    }
    return val1;
}
int Expression::shift()
{
    int val1 = add();
    int kw = lexer.GetToken()->GetKeyword();
    while (kw == Lexer::leftshift || kw == Lexer::rightshift)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            int val2 = add();
            switch (kw)
            {
                case Lexer::leftshift:
                    val1 <<= val2;
                    break;
                case Lexer::rightshift:
                    val1 >>= val2;
                    break;
            }
        }
        kw = lexer.GetToken()->GetKeyword();
    }
    return val1;
}
int Expression::relation()
{
    int val1 = shift();
    int kw = lexer.GetToken()->GetKeyword();
    while (kw == Lexer::gt || kw == Lexer::lt || kw == Lexer::geq || kw == Lexer::leq)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            int val2 = shift();
            switch (kw)
            {
                case Lexer::gt:
                    val1 = val1 > val2;
                    break;
                case Lexer::lt:
                    val1 = val1 < val2;
                    break;
                case Lexer::geq:
                    val1 = val1 >= val2;
                    break;
                case Lexer::leq:
                    val1 = val1 <= val2;
                    break;
            }
        }
        kw = lexer.GetToken()->GetKeyword();
    }
    return val1;
}
int Expression::equal()
{
    int val1 = relation();
    int kw = lexer.GetToken()->GetKeyword();
    while (kw == Lexer::eq || kw == Lexer::ne)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            int val2 = shift();
            switch (kw)
            {
                case Lexer::eq:
                    val1 = val1 == val2;
                    break;
                case Lexer::ne:
                    val1 = val1 != val2;
                    break;
            }
        }
        kw = lexer.GetToken()->GetKeyword();
    }
    return val1;
}
int Expression::and_()
{
    int val1 = equal();
    while (lexer.GetToken() != nullptr && lexer.GetToken()->GetKeyword() == Lexer::band)
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
    while (lexer.GetToken() != nullptr && lexer.GetToken()->GetKeyword() == Lexer::bxor)
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
    while (lexer.GetToken() != nullptr && lexer.GetToken()->GetKeyword() == Lexer::bor)
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
    while (lexer.GetToken() != nullptr && lexer.GetToken()->GetKeyword() == Lexer::land)
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
    while (lexer.GetToken() != nullptr && lexer.GetToken()->GetKeyword() == Lexer::lor)
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
    if ((lexer.GetToken() != nullptr) && lexer.GetToken()->GetKeyword() == Lexer::hook)
    {
        lexer.NextToken();
        if (lexer.GetToken() != nullptr)
        {
            int val2 = logicalor();
            lexer.NextToken();
            if (lexer.GetToken() != nullptr)
            {
                if (lexer.GetToken()->GetKeyword() == Lexer::colon)
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
                    throw new std::runtime_error("Conditional needs ':'");
                }
            }
        }
    }
    return val1;
}
