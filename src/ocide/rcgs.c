/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2012, David Lindauer, (LADSoft).
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
/* scanner
 */
/* Trigraphs implemented, won't work for token pasting though */
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <windows.h>
#include "rc.h"

#define MAX_STRLEN 257

/* #define DEMO */

extern int inSymFile;
extern char version[];
extern FILE *inputFile;
extern int ifskip, elsetaken;
extern IFSTRUCT *ifs;
extern char *errfile;
extern int errlineno;
extern int inSymFile;
extern char *rcIdFile;

char *infile;
extern short *lptr; /* shared with preproc */
extern FILE *inclfile[10]; /* shared with preproc */
extern char *inclfname[10]; /* shared with preproc */
extern IFSTRUCT *ifshold[10];
extern int inclline[10]; /* shared with preproc */
extern int incldepth; /* shared with preproc */
extern int inclhfile[10];
extern int inclInputLen[10];
extern char *inclInputBuffer[10];
extern char *inclibufPtr[10];
extern int inhfile;

char intstring[50];
int lineno;
int laststrlen;
short inputline[4096];
char *inputBuffer;
char *ibufPtr;
int inputLen;
int lastch;
enum e_sym lastst;
char lastid[256] = "";
char laststr[MAX_STRLEN + 1] = "";
long ival = 0;
long double rval = 0.0;
char *linstack[20]; /* stack for substitutions */
char chstack[20]; /* place to save lastch */
int lstackptr = 0; /* substitution stack pointer */
int cantnewline = FALSE;
int incconst = FALSE;

int backupchar =  - 1;

int demolines;
static LIST *cachedLineHead, **cachedLineTail;
static LIST *hCachedLineHead, **hCachedLineTail;
static int commentlevel;

BOOL iswhitespacechar(short x) 
{
    return ((x) == '\t' || isspace(x));
}
BOOL issymchar(short x) 
{
    return (isalnum(x) || (x) == '_');
}
BOOL isstartchar(short x) 
{
    return (isalpha(x) || (x) == '_');
}
void initsym(void)
{
    inputLen = 0;
    lptr = inputline;
    inputline[0] = 0;
    lineno = 0;
    errlineno = 0;
    lastid[0] = 0;
    laststr[0] = 0;
    ival = 0;
    rval = 0.0;
    cantnewline = FALSE;
    incconst = FALSE;
    backupchar =  - 1;
    inputBuffer = calloc(INPUT_BUFFER_LEN, 1);
    cachedLineTail = &cachedLineHead;
    cachedLineHead = 0;
    hCachedLineTail = &hCachedLineHead;
    hCachedLineHead = 0;
} 


/* Strips comments and also the newline char at the end of the line */
static void stripcomment(short *line)
{
    short *s = line,  *e = s, instr = FALSE;
    while (*e)
    {
        if (!instr &&  *e == '/' && !commentlevel)
        {
            if (*(e+1) == '*')
            {
                e++;
                commentlevel++;
            }
            else if (*(e+1) == '/' && !commentlevel)
            {
                *s++ = '\n';
                *s++ = 0;
                return ;
            }
            else if (!commentlevel)
                *s++ =  *e;
        }
        else if (!instr && commentlevel &&  *e == '*' && *(e+1) == '/')
        {
            commentlevel--;
            e++;
            if (commentlevel == 0)
            {
                *s++ = ' '; /* Comments are whitespace */
            }
        }
        else if (!commentlevel)
        {
            if (instr)
            {
                if (*e == instr)
                    instr = 0;
                else if (*e == '\\')
                    *s++ =  *e++;
            }
            else if (*e == '"')
                instr = '"';
            else if (*e == '\'')
                instr = '\'';
            *s++ =  *e;
        }
        e++;
    }
    *s = 0;
}


int getstring(char *s, int len, FILE *file)
{
    char *olds = s;
    while (TRUE)
    {
        while (inputLen--)
        {
            if (*ibufPtr == 0x1a)
            {
                *s = 0;
                inputLen = 0;
                return s == olds;
            }
            if (*ibufPtr != '\r')
            {
                if ((*s++ =  *ibufPtr++) == '\n' || !--len)
                {
                    *s = 0;
                    return 0;
                }
            }
            else
                ibufPtr++;
        }
        inputLen = fread(inputBuffer, 1, INPUT_BUFFER_LEN, file);
        ibufPtr = inputBuffer;
        if (inputLen <= 0)
        {
            *s = 0;
            inputLen = 0;
            return s == olds;
        }
    }
}
LIST *GetCachedLines(void)
{
    LIST *rv = NULL;
    if (inSymFile)
    {
        rv = hCachedLineHead;
        hCachedLineHead = 0;
        hCachedLineTail = &hCachedLineHead;
    }
    else if (incldepth == 0)
    {
        rv = cachedLineHead;
        cachedLineHead = 0;
        cachedLineTail = &cachedLineHead;
    }
    return rv;
}
static void CacheLine(WCHAR *lptr, char *xbuf)
{
    if (inSymFile)
    {
        char *p = strstr(xbuf, L"#define");
        if (p)
            return;
    }
    if (incldepth == 0 && (!*lptr || *lptr == '#') || inSymFile)
    {
            
        char *s = rcStrdup(xbuf); //FIXME ?
        LIST *x = rcAlloc(sizeof(LIST));
        x->data = s;
        if (inSymFile)
        {
            *hCachedLineTail = x;
            hCachedLineTail = &(*hCachedLineTail)->next;
        }
        else
        {
            *cachedLineTail = x;
            cachedLineTail = &(*cachedLineTail)->next;
        }
    }
}
//-------------------------------------------------------------------------

int getline(int listflag)
/*
 * Read in a line, preprocess it, and dump it to the list and preproc files
 * Also strip comments and alter trigraphs
 */
{
    int rv, rvc, prepping, temp;
    static int inpreprocess;
    char ibuf[4096], xbuf[4096],  *xptr;
    char *ptr = ibuf;
    if (cantnewline)
    {
        return (0);
    }
    repeatit: 
    do
    {
        rv = FALSE;
        prepping = FALSE;
        rvc = 0;
//        add: 
        while (rvc + 131 < 4096 && !rv)
        {
            ++lineno;
            ++errlineno;
            rv = getstring(ibuf + rvc, 4096 - rvc, inputFile);
            if (rv)
            {
                break;
            }
            rvc = strlen(ibuf);
            if (ibuf[rvc - 1] != '\n')
            {
                ibuf[rvc++] = '\n';
                ibuf[rvc] = 0;
            }
            rvc -= 2;
            while (ibuf[rvc] == ' ')
                rvc--;
            //					if (ibuf[rvc] != '\\')
            break;
        }
        if (rvc)
            rv = FALSE;
        if (rv)
        {
            if (ifs)
                generror(ERR_PREPROCMATCH, 0);
            if (commentlevel)
                generror(ERR_COMMENTMATCH, 0);
            if (incldepth > 0)
            {
                if (inSymFile && !strcmp(infile, rcIdFile))
                    inSymFile = FALSE;
                fclose(inputFile);
                inputFile = inclfile[--incldepth];
                lineno = inclline[incldepth];
                inhfile = inclhfile[incldepth];
                infile = inclfname[incldepth];
                free(inputBuffer);
                inputBuffer = inclInputBuffer[incldepth];
                inputLen = inclInputLen[incldepth];
                ibufPtr = inclibufPtr[incldepth];
                errlineno = lineno;
                errfile = infile;
                ifs = ifshold[incldepth];
                commentlevel = 0;
                popif();
                goto repeatit;
            }
        }
        if (rv)
            return 1;
        lptr = inputline;
        ptr = ibuf;
        xptr = xbuf;
        while ((temp = *ptr++) != 0)
        {
            *lptr++ = (unsigned char)temp;
            *xptr++ = (unsigned char)temp;
        }
        *lptr = 0;
        *xptr = 0;
        stripcomment(inputline);
        lptr = inputline;
        while (iswhitespacechar(*lptr))
            lptr++;
        CacheLine(lptr, xbuf);
        if (lptr[0] == '#')
        {
            inpreprocess++;
            listflag = preprocess();
            inpreprocess--;
            prepping = TRUE;
            lastst = eol;
        }
        if (incldepth)
            lastst = eol;
    }
    while (ifskip || prepping || inhfile && !inpreprocess)
        ;
    rvc = strlen(ibuf);
    /*
    if (defcheck(inputline) ==  - 10 && rvc + 131 < 4096)
    {
        if (ibuf[rvc - 1] == '\n')
            ibuf[rvc - 1] = ' ';
        goto add;
    }
    */
    return 0;
}

/*
 *      getch - basic get character routine.
 */
int getch(void)
{
    while ((lastch =  *lptr++) == '\0')
    {
        if (lstackptr > 0)
        {
            lptr = linstack[--lstackptr];
            lastch = chstack[lstackptr];
            return lastch;
        }
        if (cantnewline)
        {
            lptr--;
            lastch = ' ';
            break;
        }
        if (getline(incldepth == 0))
            return lastch =  - 1;
    }
    return lastch;
}

/*
 *      getid - get an identifier.
 *
 *      identifiers are any isidch conglomerate
 *      that doesn't start with a numeric character.
 *      this set INCLUDES keywords.
 */
void getid()
{
    register int i;
    i = 0;
     /* Mangling */
    if (lastch == 'L')
    {
        lastid[i++] = 'L';
        getch();
        if (lastch == '\"')
        {
            getch();
            i = 0;
            while (lastch != '\"' && lastch)
            {
                *(((short*)(laststr)) + i++) = lastch;
                getch();
            }
            if ((lastch &0x7f) != '\"')
                generror(ERR_NEEDCHAR, '\"');
            else
                getch();
            *(((short*)(laststr)) + i) = 0;
            laststrlen = i;
            lastst = lsconst;
            return ;
        }
    }
    while (issymchar(lastch))
    {
        if (i < sizeof(lastid)/sizeof(lastid[0]))
            lastid[i++] = lastch;
        getch();
    }
    lastid[i] = '\0';
    lastst = ident;
}

/*
 *      getsch - get a character in a quoted string.
 *
 *      this routine handles all of the escape mechanisms
 *      for characters in strings and character constants.
 */
int getsch(void) /* return an in-quote character */
{
    register int i, j;
    if (lastch == '\n')
        return  - 1;
    if (incconst || lastch != '\\')
    {
        i = lastch;
        getch();
        return i;
    }
    getch(); /* get an escaped character */
    if (isdigit(lastch))
    {
        for (i = 0, j = 0; j < 3; ++j)
        {
            if (lastch <= '7' && lastch >= '0')
                i = (i << 3) + lastch - '0';
            else
                break;
            getch();
        }
        return i;
    }
    i = lastch;
    getch();
    switch (i)
    {
        case '\n':
            getch();
            return getsch();
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
        case 't':
            return '\t';
        case '\'':
            return '\'';
        case '\"':
            return '\"';
        case '\\':
            return '\\';
        case 'x':
            {
                int n = 0, count = 0;
                while (isxdigit(lastch))
                {
                    count++;
                    lastch -= 0x30;
                    if (lastch > 10)
                        lastch -= 7;
                    if (lastch > 15)
                        lastch -= 32;
                    n *= 16;
                    n += lastch;
                    getch();
                }
                if (count > 2)
                    generror(ERR_CONSTTOOLARGE, 0);
                return n;
            }
        default:
            if (isdigit(i) && i < '8')
            {
                int n = 0;
                while (isdigit(i) && i < '8')
                {
                    n = n * 8+(lastch - '0');
                    getch();
                }
                return n;
            }
            return i;
    }
}

//-------------------------------------------------------------------------

int radix36(char c)
{
    if (isdigit(c))
        return c - '0';
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    return  - 1;
}

/*
 *      getbase - get an integer in any base.
 */
void getbase(int b, char **ptr)
{
    register long i, j;
    int errd = 0;
    i = 0;
    while (isalnum(**ptr))
    {
        if ((j = radix36(*(*ptr)++)) < b)
        {
            if (i > (ULONG_MAX - j) / b)
            if (!errd)
            {
                generror(ERR_CONSTTOOLARGE, 0);
                errd++;
            }
            i = i * b + j;
        }
        else
            break;
    }
    ival = i;
    lastst = iconst;
}

/*
 *      getfrac - get fraction part of a floating number.
 */
void getfrac(char **ptr)
{
    long double frmul;
    frmul = 0.1;
    while (isdigit(**ptr))
    {
        rval += frmul *(*(*ptr)++ - '0');
        frmul *= 0.1;
    }
}

/*
 *      getexp - get exponent part of floating number.
 *
 *      this algorithm is primative but usefull.  Floating
 *      exponents are limited to +/-255 but most hardware
 *      won't support more anyway.
 */
void getexp(char **ptr)
{
    long double expo, exmul;
    expo = 1.0;
    if (lastst != rconst)
        rval = ival;
    if (**ptr == '-')
    {
        exmul = 0.1;
        (*ptr)++;
    }
    else
    {
        exmul = 10.0;
        if (**ptr == '+')
            (*ptr)++;
    }
    getbase(10, ptr);
    if (ival > 255)
        generror(ERR_FPCON, 0);
    else
        while (ival--)
            expo *= exmul;
    rval *= expo;
    lastst = rconst;
}

/*
 *      getnum - get a number from input.
 *
 *      getnum handles all of the numeric input. it accepts
 *      decimal, octal, hexidecimal, and floating point numbers.
 */
void getnum(void)
{
    int isfloat = FALSE;
    char *ptr = intstring;

    while (isxdigit(lastch) || lastch == 'x' || lastch == 'X')
    {
        *ptr++ = lastch;
        getch();
    }
    if (lastch == '.')
    {
        isfloat = TRUE;
        *ptr++ = lastch;
        getch();
        while (isdigit(lastch))
        {
            *ptr++ = lastch;
            getch();
        }
    }
    if (lastch == 'e' || lastch == 'E')
    {
        isfloat = TRUE;
        *ptr++ = lastch;
        getch();
        if (lastch == '+' || lastch == '-')
        {
            *ptr++ = lastch;
            getch();
        }
        while (isdigit(lastch))
        {
            *ptr++ = lastch;
            getch();
        }
    }
    if (lastch == 'F')
    {
        isfloat = TRUE;
    }
    *ptr = 0;
    ptr = intstring;
    if (!isfloat)
    {
        if (*ptr == '0')
        {
            ptr++;
            if (*ptr == 'x' ||  *ptr == 'X')
            {
                ptr++;
                getbase(16, &ptr);
            }
            else
                getbase(8, &ptr);
        }
        else
            getbase(10, &ptr);
        if (lastch == 'U' || lastch == 'u')
        {
            lastst = iuconst;
            getch();
            if (lastch == 'L' || lastch == 'l')
            {
                lastst = luconst;
                getch();
            }
        }
        else if (lastch == 'L' || lastch == 'l')
        {
            lastst = lconst;
            getch();
            if (lastch == 'U' || lastch == 'u')
            {
                lastst = luconst;
                getch();
            }
        }
    }
    else
    {
        getbase(10, &ptr);
        if (*ptr == '.')
        {
            ptr++;
            rval = ival; /* float the integer part */
            getfrac(&ptr); /* add the fractional part */
            lastst = rconst;
        }
        if (*ptr == 'e' ||  *ptr == 'E')
        {
            ptr++;
            getexp(&ptr); /* get the exponent */
        }
        if (lastch == 'F' || lastch == 'f')
        {
            if (lastst != rconst)
            {
                rval = ival;
            }
            lastst = fconst;
            getch();
        }
        else if (lastch == 'L' || lastch == 'l')
        {
            if (lastst != rconst)
            {
                rval = ival;
            }
            lastst = lrconst;
            getch();
        }
    }
}

//-------------------------------------------------------------------------

int getsym2(void)
/*
 * translate character sequences to appropriate token names
 */
{
    register int i, j, k;
    int size;
//swlp: 
	switch (lastch)
    {
        case '+':
            getch();
            if (lastch == '+')
            {
                getch();
                lastst = autoinc;
            }
            else if (lastch == '=')
            {
                getch();
                lastst = asplus;
            }
            else
                lastst = plus;
            break;
        case '-':
            getch();
            if (lastch == '-')
            {
                getch();
                lastst = autodec;
            }
            else if (lastch == '=')
            {
                getch();
                lastst = asminus;
            }
            else if (lastch == '>')
            {
                getch();
                    lastst = pointsto;
            }
            else
                lastst = minus;
            break;
        case '*':
            getch();
            if (lastch == '=')
            {
                getch();
                lastst = astimes;
            }
            else
                lastst = star;
            break;
        case '/':
            getch();
            if (lastch == '=')
            {
                getch();
                lastst = asdivide;
            }
            else
                lastst = divide;
            break;
        case '^':
            getch();
            if (lastch == '=')
            {
                getch();
                lastst = asxor;
            }
            else
                lastst = uparrow;
            break;
        case ';':
            getch();
            lastst = semicolon;
            break;
        case ':':
            getch();
                lastst = colon;
            break;
        case '=':
            getch();
            if (lastch == '=')
            {
                getch();
                lastst = eq;
            }
            else
                lastst = assign;
            break;
        case '>':
            getch();
            if (lastch == '=')
            {
                getch();
                lastst = geq;
            }
            else if (lastch == '>')
            {
                getch();
                if (lastch == '=')
                {
                    getch();
                    lastst = asrshift;
                }
                else
                    lastst = rshift;
            }
            else
                lastst = gt;
            break;
        case '<':
            getch();
            if (incconst)
            {
                incconst = FALSE;
                for (i = 0; i < MAX_STRLEN; ++i)
                {
                    if (lastch == '>')
                        break;
                    if ((j = getsch()) ==  - 1)
                        break;
                    else
                        laststr[i] = j;
                }
                laststr[i] = 0;
                lastst = sconst;
                laststrlen = i;
                if (lastch != '>')
                    generror(ERR_NEEDCHAR, '>');
                else
                    getch();
            }
            else
            if (lastch == '=')
            {
                getch();
                lastst = leq;
            }
            else if (lastch == '<')
            {
                getch();
                if (lastch == '=')
                {
                    getch();
                    lastst = aslshift;
                }
                else
                    lastst = lshift;
            }
            else
                lastst = lt;
            break;
        case '\\':
            getch();
            lastst = backslash;
            break;
        case '\'':
            getch();
            j = 0;
            k = 0;
            ival = 0;
            for (i = 0, j = 0, k = 0; i < 4; i++, k += 8)
            {
                if (lastch == '\'')
                    break;
                j = getsch(); /* get a string char */
                ival += j << k;
            }
            if (i == 0)
            {
                generror(ERR_CHAR4CHAR, 0);
                getch();
            }
            else if (lastch != '\'')
            {
                while (*lptr &&  *lptr != '\'')
                    lptr++;
                if (*lptr)
                    lptr++;
                generror(ERR_CHAR4CHAR, 0);
            }
            else
                getch();
            lastst = cconst;
            break;
        case '\"':
            size = 0;
            while (lastch == '\"')
            {
                getch();
                for (i = size; i < MAX_STRLEN; ++i)
                {
                    if (lastch == '\"')
                        break;
                    if ((j = getsch()) ==  - 1)
                        break;
                    else
                        laststr[i] = j;
                }
                laststr[i] = 0;
                laststrlen = size = i;
                lastst = sconst;
                if (lastch != '\"')
                    generror(ERR_NEEDCHAR, '\"');
                else
                {
                    getch();
                    while (iswhitespacechar(lastch) && lastch >= 32)
                        getch();
                }

            }
            break;
        case '!':
            getch();
            if (lastch == '=')
            {
                getch();
                lastst = neq;
            }
            else
                lastst = not;
            break;
        case '%':
            getch();
            if (lastch == '=')
            {
                getch();
                lastst = asmodop;
            }
            else
                lastst = modop;
            break;
        case '~':
            getch();
            lastst = compl;
            break;
        case '.':
            if (isdigit(*lptr))
                getnum();
            else
            {
                getch();
                if (lastch == '.')
                {
                    getch();
                    if (lastch == '.')
                    {
                        getch();
                        lastst = ellipse;
                        break;
                    }
                    else
                    {
                        generror(ERR_ILLCHAR, lastch);
                    }
                }
                else
                    lastst = dot;
            }
            break;
        case ',':
            getch();
            lastst = comma;
            break;
        case '&':
            getch();
            if (lastch == '&')
            {
                lastst = land;
                getch();
            }
            else if (lastch == '=')
            {
                lastst = asand;
                getch();
            }
            else
                lastst = and;
            break;
        case '|':
            getch();
            if (lastch == '|')
            {
                lastst = lor;
                getch();
            }
            else if (lastch == '=')
            {
                lastst = asor;
                getch();
            }
            else
                lastst = or;
            break;
        case '(':
            getch();
            lastst = openpa;
            break;
        case ')':
            getch();
            lastst = closepa;
            break;
        case '[':
            getch();
            lastst = openbr;
            break;
        case ']':
            getch();
            lastst = closebr;
            break;
        case '{':
            getch();
            lastst = begin;
            break;
        case '}':
            getch();
            lastst = end;
            break;
        case '?':
            getch();
            lastst = hook;
            break;
        default:
            generror(ERR_ILLCHAR, lastch);
            getch();
            return 1;
    }
    return 0;
}

/*
 *      getsym - get next symbol from input stream.
 *
 *      getsym is the basic lexical analyzer.  It builds
 *      basic tokens out of the characters on the input
 *      stream and sets the following global variables:
 *
 *      lastch:         A look behind buffer.
 *      lastst:         type of last symbol read.
 *      laststr:        last string constant read.
 *      lastid:         last identifier read.
 *      ival:           last integer constant read.
 *      rval:           last real constant read.
 *
 *      getsym should be called for all your input needs...
 */
void getsym(void)
{
    if (backupchar !=  - 1)
    {
        lastst = backupchar;
        backupchar =  - 1;
        return ;
    }
    if (! *lptr)
    {
        if (lastst == eol && lastch ==  - 1 || lastst == eof)
            lastst = eof;
        else
            lastst = eol;
        getch();
        return ;
    }
    while (iswhitespacechar(lastch))
    {
        getch();
        if (! *lptr)
        {
            lastst = eol;
            getch();
            return ;
        }
    }
    if (lastch ==  - 1)
        lastst = eof;
    else if (isdigit(lastch))
        getnum();
    else if (isstartchar(lastch))
    {
        getid();
        searchkw();
    }
    else
        getsym2();
}

//-------------------------------------------------------------------------

void skip_comma(void)
{
    if (lastst == comma)
    {
        getsym();
        if (lastst == eol)
            getsym();
    }
}

//-------------------------------------------------------------------------

void need_eol(void)
{
    if (lastst != eol)
        generror(ERR_EXTRA_DATA_ON_LINE, 0);
    getsym();
}

//-------------------------------------------------------------------------

void need_begin(void)
{
    if (lastst != begin && lastst != kw_begin)
        generror(ERR_BEGIN_EXPECTED, 0);
    getsym();
    need_eol();
}

//-------------------------------------------------------------------------

void need_end(void)
{
    if (lastst != end && lastst != kw_end)
        generror(ERR_END_EXPECTED, 0);
    getsym();
    need_eol();
}

/*
 * when we need specific punctuation, call one of these routines
 */
int needpunc(enum e_sym p)
{
    if (lastst == p)
    {
        getsym();
        return (TRUE);
    } 
    else
        expecttoken(p);
    return (FALSE);
}

//-------------------------------------------------------------------------

int needpuncexp(enum e_sym p)
{
    if (lastst == p)
    {
        getsym();
        return (TRUE);
    } 
    else
        expecttokenexp(p);
    return (FALSE);
}

//-------------------------------------------------------------------------

int is_number(void)
{
    switch (lastst)
    {
        case iconst:
        case iuconst:
        case lconst:
        case luconst:
            return TRUE;
        case openpa:
        case minus:
            return TRUE;
        case ident:
            return TRUE;
        default:
            return FALSE;
    }
}

/*
 * having to back up a character is rare, but sometimes...
 */
void backup(int st)
{
    backupchar = lastst;
    lastst = st;
}
