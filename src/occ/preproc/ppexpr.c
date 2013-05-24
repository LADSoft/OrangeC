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

*/
#include "compiler.h"
#define ILP includes->lptr

extern INCLUDES *includes;
extern COMPILER_PARAMS cparams;

int getsch(int bytes, char **source) /* return an in-quote character */
{
    register int i=*(*source)++, j;
    if (**source == '\n')
        return INT_MIN;
    if (i != '\\')
    {
        return (char)i;
    }
    i = *(*source); /* get an escaped character */
    if (isdigit(i) && i < '8')
    {
        for (i = 0, j = 0; j < 3; ++j)
        {
            if (*(*source) <= '7' && *(*source) >= '0')
                i = (i << 3) + *(*source) - '0';
            else
                break;
            (*source)++;
        }
        return i;
    }
    (*source)++;
    switch (i)
    {
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 'f':
            return '\f';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 'v':
            return '\v';
        case 't':
            return '\t';
        case '\'':
            return '\'';
        case '\"':
            return '\"';
        case '\\':
            return '\\';
        case '?':
            return '?';
        case 'U':
            bytes = 4;
        case 'u':
            if (i == 'u')
                bytes = 2;
        case 'x':
            {
                int n = 0;
                while (isxdigit(*(*source)))
                {
                    if (*(*source) >= 0x60)
                        *(*source) &= 0xdf;
                    *(*source) -= 0x30;

                    if (*(*source) > 10)
                        *(*source) -= 7;
                    if (*(*source) > 15)
                        *(*source) -= 32;
                    n *= 16;
                    n += *(*source);
                    (*source)++;
                }
                /* hexadecimal escape sequences are only terminated by a non hex char */
                /* we sign extend or truncate */
                if (bytes == 1)
                    if (cparams.prm_charisunsigned)
                        n = (int)(BYTE)n;
                    else
                        n = (int)(char)n;
                if (bytes == 2 && i == 'x')
                        n = (int)(LCHAR)n;
                if (i != 'x')
                {
                    if (n <= 0x20 || n >= 0x7f && n <= 0x9f ||
                        n >=0xd800 && n<= 0xdfff)
                        pperror(ERR_INVCONST, 0);
                }
                return n;
            }
        default:
            return (char)i;
    }
}

static PPINT ieprimary(void)   
/*
 * PRimary integer
 *    id
 *    iconst
 *    (cast )intexpr
 *    (intexpr)
 */
{
       PPINT     temp;
        if (isdigit(*ILP))
        {
            PPINT rv = expectnum();
            skipspace();
            return rv;
        }
        else if(*ILP == '\'') {
            ILP++;
            temp = getsch(2,&ILP);
            if (*ILP != '\'')
                pperror(ERR_NEEDY, '\'');
            else
                ILP++;
            skipspace();
            return temp;
        }
        else if (*ILP == '(') {
            ILP++;
            skipspace();
            if (*ILP == ')')
            {
                ILP++;
                pperror(ERR_CONSTANT_VALUE_EXPECTED, 0);
            }
            else
            {
                temp = ppexpr();
                if (*ILP != ')')
                    pperror(ERR_NEEDY, ')');
                else
                    ILP++;
            }
            skipspace();
            return temp;
        }
        else if (isstartchar(*ILP))
        {
            char buf[256], *p = buf;
            while (issymchar(*ILP))
                *p++ = *ILP++;
            *p = 0;
            skipspace();
            return 0;
        }
        pperror(ERR_CONSTANT_VALUE_EXPECTED,0);
        return 0;
}
/*
 * Integer unary
 *   - unary
 *   ! unary
 *   ~unary
 *   primary
 */
static PPINT ieunary(void)
{
   PPINT temp;
   skipspace();
    switch (*ILP) {
        case '-':
            ILP++;
            temp = -ieunary();
            break;
        case '!':
            ILP++;
            temp = !ieunary();
            break;
        case '~':
            ILP++;
            temp = ~ieunary();
            break;
      case '+':
            ILP++;
            temp = ieunary() ;
            break ;
        default:
                temp = ieprimary();
                break;
    }
    skipspace();
    return(temp);
}
static PPINT iemultops(void)
/* Multiply ops */
{
   PPINT val1 = ieunary(),val2;
    while (ILP[0] == '*' || ILP[0] == '/' || ILP[0] == '%') {
        char type = *ILP++;
        val2 = ieunary();
        switch(type) {
            case '*':
                val1 = val1 * val2;
                break;
            case '/':
                val1 = val1 / val2;
                break;
            case '%':
                val1 = val1 % val2;
                break;
        }
    }
    skipspace();
    return(val1);
}
static PPINT ieaddops(void)
/* Add ops */
{
   PPINT val1 = iemultops(),val2;
    while (ILP[0] == '+' && ILP[1] != '+' || ILP[0] == '-' && ILP[1] != '-')	{
        char type = ILP[0];
        ILP++;
        val2 = iemultops();
        if (type == '+') 
            val1 = val1 + val2;
        else
            val1 = val1 - val2;
    }
    skipspace();
    return(val1);
}
static PPINT ieshiftops(void)
/* Shift ops */
{
   PPINT val1 = ieaddops(), val2;
    while (ILP[0] == '<' && ILP[1] == '<' || ILP[0] == '>' && ILP[1] == '>') {
        int type = ILP[0];
        ILP += 2;
        val2 = ieaddops();
        if (type == '<')
            val1 <<= val2;
        else
            val1 >>= val2;
    }
    skipspace();
    return(val1);
}
static PPINT ierelation(void)
/* non-eq relations */
{
   PPINT val1 = ieshiftops(), val2;
    while (ILP[0] == '<' && ILP[1] != '<' || ILP[0] == '>' && ILP[1] != '>') {
        BOOL eq = FALSE;
        int type = ILP[0];
        ILP++;
        if (*ILP == '=')
        {
            eq = TRUE;
            ILP++;
        }
        val2 = ieshiftops();
        if (type == '<')
            if (eq)
                val1 = val1 <= val2;
            else
                val1 = val1 < val2;
        else
            if (eq)
                val1 = val1 >= val2;
            else
                val1 = val1 > val2;
    }
    skipspace();
    return(val1);
}
static PPINT ieequalops(void)
/* eq relations */
{
   PPINT val1 = ierelation(),val2;
    while ((ILP[0] == '=' || ILP[0] == '!') && ILP[1] == '=') {
        char type =ILP[0];
        ILP += 2;
        val2 = ierelation();
        if (type == '!')
            val1 = val1 != val2;
        else
            val1 = val1 == val2;
    }
    skipspace();
    return(val1);
}
static PPINT ieandop(void)
/* and op */
{
   PPINT val1 = ieequalops(),val2;
    while (ILP[0] == '&' && ILP[1] != '&') {
        ILP +=1;
        val2 = ieequalops();
        val1 = val1 & val2;
    }
    skipspace();
    return(val1);
}
static PPINT iexorop(void)
/* xor op */
{
   PPINT val1 = ieandop(),val2;
    while (ILP[0] == '^') {
        ILP ++;
        val2 = ieandop();
        val1 = val1 ^ val2;
    }         
    skipspace();
    return(val1);
}
static PPINT ieorop(void)
/* or op */
{
   PPINT val1 = iexorop(),val2;
    while (ILP[0] == '|' && ILP[1] != '|') {
        ILP ++;
        val2 = iexorop();
        val1 = val1 | val2;
    }
    skipspace();
    return(val1);
}
static PPINT ielandop(void)
/* logical and op */
{
   PPINT val1 = ieorop(),val2;
    while (ILP[0] == '&' && ILP[1] == '&') {
        ILP +=2;
        val2 = ieorop();
        val1 = val1 && val2;
    }
    skipspace();
    return(val1);
}
static PPINT ielorop(void)
/* logical or op */
{
   PPINT val1 = ielandop(),val2;
    while (ILP[0] == '|' && ILP[1] == '|') {
        ILP += 2;
        val2 = ielandop();
        val1 = val1 || val2;
    }
    skipspace();
    return(val1);
}
static PPINT iecondop(void)
/* Hook op */
{
   PPINT val1 = ielorop(),val2, val3;
        if (*ILP == '?') {
            ILP ++;
            val2 = ielorop();
            if (*ILP != ':')
                pperror(ERR_NEEDY, ':');
            ILP++;
            val3 = iecondop();
            if (val1)
                val1 = val2;
            else
                val1 = val3;
        }
    skipspace();
    return(val1);
}
static PPINT iecommaop(void)
/* Hook op */
{
   PPINT val1 = iecondop();
        while (*ILP == ',') {
            ILP ++;
            iecondop();
        }
    skipspace();
    return(val1);
}
PPINT ppexpr(void)
/* Integer expressions */
{
    skipspace();
    return iecommaop();
}