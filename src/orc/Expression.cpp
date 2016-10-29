/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
#include "Errors.h"
#include "Expression.h"
#include <stdexcept>

int Expression::primary()
{
    int rv = 0;
    if (lexer.GetToken()->GetKeyword() == Lexer::openpa)
    {
        lexer.NextToken();
        if (lexer.GetToken() != NULL )
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
    if (lexer.GetToken() != NULL)
    {
        int kw = lexer.GetToken()->GetKeyword();
        if (kw == Lexer::plus || kw == Lexer::minus || kw == Lexer::lnot || kw == Lexer::bcompl)
        {
            lexer.NextToken();
            if (lexer.GetToken() != NULL)
            {
                int val1 = unary();
                switch(kw)
                {
                    case Lexer::minus:
                        val1 = - val1;
                        break;
                    case Lexer::plus:
                        val1 = + val1;
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
    return 0;
}
int Expression::multiply()
{
    int val1 = unary();
    int kw = lexer.GetToken()->GetKeyword();
    while (kw == Lexer::star || kw == Lexer::divide || kw == Lexer::mod)
    {
        lexer.NextToken();
        if (lexer.GetToken() != NULL)
        {
            int val2 = unary();
            switch(kw)
            {
                case Lexer::star:
                    val1 *= val2;
                    break;
                case Lexer::divide:
                    if (val2 == 0)
                    {
                        throw new std::runtime_error("Divide by zero in preprocessor constant");
                        val1 = 1;
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
                        val1 = 1;
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
        if (lexer.GetToken() != NULL)
        {
            int val2 = multiply();
            switch(kw)
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
        if (lexer.GetToken() != NULL)
        {
            int val2 = add();
            switch(kw)
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
        if (lexer.GetToken() != NULL)
        {
            int val2 = shift();
            switch(kw)
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
        if (lexer.GetToken() != NULL)
        {
            int val2 = shift();
            switch(kw)
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
    while (lexer.GetToken() != NULL && lexer.GetToken()->GetKeyword() == Lexer::band)
    {
        lexer.NextToken();
        if (lexer.GetToken() != NULL)
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
    while (lexer.GetToken() != NULL && lexer.GetToken()->GetKeyword() == Lexer::bxor)
    {
        lexer.NextToken();
        if (lexer.GetToken() != NULL)
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
    while (lexer.GetToken() != NULL && lexer.GetToken()->GetKeyword() == Lexer::bor)
    {
        lexer.NextToken();
        if (lexer.GetToken() != NULL)
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
    while (lexer.GetToken() != NULL && lexer.GetToken()->GetKeyword() == Lexer::land)
    {
        lexer.NextToken();
        if (lexer.GetToken() != NULL)
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
    while (lexer.GetToken() != NULL && lexer.GetToken()->GetKeyword() == Lexer::lor)
    {
        lexer.NextToken();
        if (lexer.GetToken() != NULL)
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
    if ((lexer.GetToken() != NULL) && lexer.GetToken()->GetKeyword() == Lexer::hook)
    {
        lexer.NextToken();
        if (lexer.GetToken() != NULL)
        {
            int val2 = logicalor();
            lexer.NextToken();
            if (lexer.GetToken() != NULL)
            {
                if (lexer.GetToken()->GetKeyword() == Lexer::colon)
                {
                    lexer.NextToken();
                    if (lexer.GetToken() != NULL)
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
                    val1 = 0;
                }
            }
        }
    }
    return val1;
}

