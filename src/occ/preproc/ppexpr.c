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

*/
#include "compiler.h"
#define ILP includes->lptr

extern INCLUDES *includes;
extern COMPILER_PARAMS cparams;

static PPINT iecommaop(BOOLEAN *uns);

int getsch(int bytes, unsigned char **source) /* return an in-quote character */
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
                {
                    if (cparams.prm_charisunsigned)
                        n = (int)(UBYTE)n;
                    else
                        n = (int)(char)n;
                }
                if (bytes == 2 && i == 'x')
                        n = (int)(LCHAR)n;
                if (i != 'x')
                {
                    if (n <= 0x20 || (n >= 0x7f && n <= 0x9f) ||
                        (n >=0xd800 && n<= 0xdfff))
                        pperror(ERR_INVCONST, 0);
                }
                return n;
            }
        default:
            return (char)i;
    }
}
static PPINT ieprimary(BOOLEAN *uns)   
/*
 * Primary integer
 *    id
 *    iconst
 *    (cast )intexpr
 *    (intexpr)
 */
{
       PPINT     temp;
       *uns = FALSE;
          
        if (isdigit(*ILP))
        {
            PPINT rv = expectnum(uns);
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
                skipspace();
                temp = iecommaop(uns);
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
static PPINT ieunary(BOOLEAN *uns)
{
   PPINT temp;
   skipspace();
    switch (*ILP) {
        case '-':
            ILP++;
            temp = -ieunary(uns);
            break;
        case '!':
            ILP++;
            temp = !ieunary(uns);
            break;
        case '~':
            ILP++;
            temp = ~ieunary(uns);
            break;
      case '+':
            ILP++;
            temp = ieunary(uns) ;
            break ;
        default:
                temp = ieprimary(uns);
                break;
    }
    skipspace();
    return(temp);
}
static PPINT iemultops(BOOLEAN *uns)
/* Multiply ops */
{
   PPINT val1 = ieunary(uns),val2;
    while (ILP[0] == '*' || ILP[0] == '/' || ILP[0] == '%') {
        char type = *ILP++;
        BOOLEAN uns1;
        val2 = ieunary(&uns1);
        *uns = *uns | uns1;
        switch(type) {
            case '*':
                if (*uns)
                    val1 = (PPUINT)val1 * (PPUINT)val2;
                else
                    val1 = val1 * val2;
                break;
            case '/':
                if (val2 == 0)
                {
                    val1 = 0;
                }
                else
                {
                    if (*uns)
                        val1 = (PPUINT)val1 / (PPUINT)val2;
                    else
                        val1 = val1 / val2;
                }
                break;
            case '%':
                if (*uns)
                    val1 = (PPUINT)val1 % (PPUINT)val2;
                else
                    val1 = val1 % val2;
                break;
        }
    }
    skipspace();
    return(val1);
}
static PPINT ieaddops(BOOLEAN *uns)
/* Add ops */
{
   PPINT val1 = iemultops(uns),val2;
    while ((ILP[0] == '+' && ILP[1] != '+') || (ILP[0] == '-' && ILP[1] != '-'))	{
        char type = ILP[0];
        BOOLEAN uns1;
        ILP++;
        val2 = iemultops(&uns1);
        *uns = *uns | uns1;
        if (type == '+') 
            val1 = val1 + val2;
        else
            val1 = val1 - val2;
    }
    skipspace();
    return(val1);
}
static PPINT ieshiftops(BOOLEAN *uns)
/* Shift ops */
{
   PPINT val1 = ieaddops(uns), val2;
    while ((ILP[0] == '<' && ILP[1] == '<') || (ILP[0] == '>' && ILP[1] == '>')) {
        int type = ILP[0];
        BOOLEAN uns1;
        ILP += 2;
        val2 = ieaddops(&uns1);
        if (type == '<')
            val1 <<= val2;
        else
            val1 >>= val2;
    }
    skipspace();
    return(val1);
}
static PPINT ierelation(BOOLEAN *uns)
/* non-eq relations */
{
   PPINT val1 = ieshiftops(uns), val2;
    while ((ILP[0] == '<' && ILP[1] != '<') || (ILP[0] == '>' && ILP[1] != '>')) {
        BOOLEAN eq = FALSE;
        BOOLEAN uns1;
        int type = ILP[0];
        ILP++;
        if (*ILP == '=')
        {
            eq = TRUE;
            ILP++;
        }
        val2 = ieshiftops(&uns1);
        *uns = *uns | uns1;
        if (*uns)
        {
            if (type == '<')
                if (eq)
                    val1 = (PPUINT)val1 <= (PPUINT)val2;
                else
                    val1 = (PPUINT)val1 < (PPUINT)val2;
            else
                if (eq)
                    val1 = (PPUINT)val1 >= (PPUINT)val2;
                else
                    val1 = (PPUINT)val1 > (PPUINT)val2;
        }
        else
        {
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
    }
    skipspace();
    return(val1);
}
static PPINT ieequalops(BOOLEAN *uns)
/* eq relations */
{
   PPINT val1 = ierelation(uns),val2;
    while ((ILP[0] == '=' || ILP[0] == '!') && ILP[1] == '=') {
        BOOLEAN uns1;
        char type =ILP[0];
        ILP += 2;
        val2 = ierelation(&uns1);
        *uns = *uns | uns1;
        if (type == '!')
            val1 = val1 != val2;
        else
            val1 = val1 == val2;
    }
    skipspace();
    return(val1);
}
static PPINT ieandop(BOOLEAN *uns)
/* and op */
{
   PPINT val1 = ieequalops(uns),val2;
    while (ILP[0] == '&' && ILP[1] != '&') {
        BOOLEAN uns1;
        ILP +=1;
        val2 = ieequalops(&uns1);
        *uns = *uns | uns1;
        val1 = val1 & val2;
    }
    skipspace();
    return(val1);
}
static PPINT iexorop(BOOLEAN *uns)
/* xor op */
{
   PPINT val1 = ieandop(uns),val2;
    while (ILP[0] == '^') {
        BOOLEAN uns1;
        ILP ++;
        val2 = ieandop(&uns1);
        *uns = *uns | uns1;
        val1 = val1 ^ val2;
    }         
    skipspace();
    return(val1);
}
static PPINT ieorop(BOOLEAN *uns)
/* or op */
{
   PPINT val1 = iexorop(uns),val2;
    while (ILP[0] == '|' && ILP[1] != '|') {
        BOOLEAN uns1;
        ILP ++;
        val2 = iexorop(&uns1);
        *uns = *uns | uns1;
        val1 = val1 | val2;
    }
    skipspace();
    return(val1);
}
static PPINT ielandop(BOOLEAN *uns)
/* logical and op */
{
   PPINT val1 = ieorop(uns),val2;
    while (ILP[0] == '&' && ILP[1] == '&') {
        ILP +=2;
        val2 = ieorop(uns);
        *uns = FALSE;
        val1 = val1 && val2;
    }
    skipspace();
    return(val1);
}
static PPINT ielorop(BOOLEAN *uns)
/* logical or op */
{
   PPINT val1 = ielandop(uns),val2;
    while (ILP[0] == '|' && ILP[1] == '|') {
        ILP += 2;
        val2 = ielandop(uns);
        *uns = FALSE;
        val1 = val1 || val2;
    }
    skipspace();
    return(val1);
}
static PPINT iecondop(BOOLEAN *uns)
/* Hook op */
{
   PPINT val1 = ielorop(uns),val2, val3;
        if (*ILP == '?') {
            BOOLEAN uns1, uns2;
            ILP ++;
            val2 = iecommaop(&uns1);
            if (*ILP != ':')
                pperror(ERR_NEEDY, ':');
            ILP++;
            val3 = iecondop(&uns2);
            *uns = uns1 || uns2;
            if (val1)
                val1 = val2;
            else
                val1 = val3;
        }
    skipspace();
    return(val1);
}
static PPINT iecommaop(BOOLEAN *uns)
/* Hook op */
{
   PPINT val1 = iecondop(uns);
        while (*ILP == ',') {
            BOOLEAN throwaway;
            ILP ++;
            iecondop(&throwaway);
        }
    skipspace();
    return(val1);
}
PPINT ppexpr()
/* Integer expressions */
{
    BOOLEAN uns = FALSE;
    PPINT val1 = iecommaop(&uns);
    skipspace();
    return val1;
}