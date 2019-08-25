/*
CC386 C Compiler
Copyright 1994-2011 David Lindauer.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

This program is derived from the cc68k complier by
Matthew Brandt (mailto::mattb@walkingdog.net)

You may contact the author of this derivative at:

mailto::camille@bluegrass.net
 */
/* scanner
 */
/*
   Trigraphs are translated before preprocessing.
   Digraphs are handled later in preprocessing.
   Both translations are enabled via the same option (/T, trigraph).
*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "utype.h"
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"
#include <io.h>

extern FILE * outputFile;
extern int prm_c99;
extern short *interjectptr;
extern int recordingTemplate, recordingClassFunc;
extern int prm_errfile;
extern char version[];
extern LIST *clist;
extern FILE *cppFile,  *listFile;
extern int inputFile;
extern int prm_cplusplus, prm_cmangle, prm_ansi;
extern int ifskip, elsetaken;
extern IFSTRUCT *ifs;
extern char *errfile;
extern int errlineno;
extern int prm_trigraph;
extern int sys_inc;

extern int currentfile;
extern char *infile;
extern short *lptr; /* shared with preproc */
extern int inclfile[]; /* shared with preproc */
extern char *inclfname[]; /* shared with preproc */
extern IFSTRUCT *ifshold[70];
extern int inclline[]; /* shared with preproc */
extern int inclcurrent[];
extern int inclpos[];
extern char *incldata[];
extern char *inclptr[];
extern int inclsysflags[];
extern int incldepth; /* shared with preproc */
extern int prm_listfile;

int sym_charindex;
int lineno;
int laststrlen;
short inputline[2*MACRO_REPLACE_SIZE+1], unalteredline[MACRO_REPLACE_SIZE+1];
int lastch;
enum e_sym lastst, lastlastst;
char lastid[101] = "", backupid[101];
char laststr[MAX_STRLEN *2+1] = "";
LLONG_TYPE ival = 0;
FPF rval ;
int cantnewline = FALSE;
int incconst = FALSE;

int backupchar =  - 1;

char inputbuffer[INPUT_BUFFER_SIZE];
int inputlen;
char *ibufPtr;

static int commentlevel;
static ULLONG_TYPE llminus1;
static int recordingOff;
static int floating_infinity = 0x7f800000;

int phiputcpp, phiputlist;
void initsym(void)
{
    lptr = inputline;
    phiputcpp = 0;
    phiputlist = 0;
    inputline[0] = 0;
    lineno = 0;
    errlineno = 1;
    lastid[0] = 0;
    laststr[0] = 0;
    ival = 0;
        memset(&rval, 0 , sizeof(rval));
        rval.type = IFPF_IS_ZERO;
    cantnewline = FALSE;
    incconst = FALSE;
    backupchar =  - 1;
    llminus1 = 0;
    llminus1--; // done at run-time so I don't have to compile twice
    recordingOff = FALSE;
    inputlen = 0;
}

void lineToCpp(void)
/*
 * line has been preprocessed, dump it to a file
 */
{
    if (cppFile)
    {
        short *p = inputline;
        if (!phiputcpp)
        {
            phiputcpp = TRUE;
            fprintf(cppFile, "/* LADsoft C compiler Version %s */\n\n", version)
                ;
        }
        while (*p)
        {
            if (*p != MACRO_PLACEHOLDER)
                fputc(*p++, cppFile);
            else
                p++;
        }
    }
}

/* Strips comments */
static int stripcomment(char *line, int begin)
{
    char *s = line,  *e = s, instr = 0;
    static int incmt = 0, incmtstr = 0;
    if (!begin)
        incmt = 0;
    e += incmt;
    s += incmt;
    while (*e)
    {
        if (!instr)
        {
            if (!commentlevel)
            {
                if (*e == '/')
                {
                    if (*(e+1) == '*')
                    {
                        incmt = s - line;
                        e += 2;
                        *s++ = ' ';
                        commentlevel = 1;
                        continue;
                    }
                    else if (*(e+1) == '/' && (!prm_ansi || prm_c99 || prm_cplusplus))
                    {
                        *s++ = '\n';
                        *s = 0;
                        return incmt;
                    }
                }
                else
                    if (*e == '"' ||  *e == '\'')
                        instr =  *e;
            }
            else
            {
                if (*e == '"' || *e == '\'')
                    incmtstr = *e;
                else if (*e == '/' && *(e+1) == '*' && !incmtstr)
                {
                    errlineno = lineno;
                    generror(ERR_NESTEDCOMMENT, 0, 0);
                }
                else if (*e == '*')
                {
                    if (*(e+1) == '/')
                    {
                        incmtstr = incmt = 0;
                        commentlevel = 0;
                        e++;
                    }
                }
                e++;
                continue;
            }
        }
        else
        if (!commentlevel &&  *e == instr)
        {
            int count = 0;
            while (s - count > line && *(s - count - 1) == '\\')
                count++;
            if (!(count &1))
                instr = 0;
        }
        *s++ =  *e++;
    }
    *s = 0;
    return incmt;
}

/* strip digraphs */
void stripdigraph(short *buf)
{
    short *cp = buf;
    while (*cp)
    {
        if (*cp == '<' && *(cp + 1) == ':')
        {
            cp += 2;
            *buf++ = '[';
        }
        else if (*cp == ':' && *(cp + 1) == '>')
        {
            cp += 2;
            *buf++ = ']';
        }
        else if (*cp == '<' && *(cp + 1) == '%')
        {
            cp += 2;
            *buf++ = '{';
        }
        else if (*cp == '%' && *(cp + 1) == '>')
        {
            cp += 2;
            *buf++ = '}';
        }
        else if (*cp == '%' && *(cp + 1) == ':')
        {
            cp += 2;
            *buf++ = '#';
            if (*cp == '%' && *(cp + 1) == ':')
            {
                cp += 2;
                *buf++ = '#';
            }
        }
        else
            *buf++ = *cp++;
    }
    *buf = 0;
}

/* strip trigraphs */
void striptrigraph(char *buf)
{
    char *cp = buf;
    while (*cp)
    {
        if (*cp == '?' && *(cp + 1) == '?')
        {
            cp += 2;
            switch (*cp++)
            {
                case '=':
                    *buf++ = '#';
                    break;
                case '(':
                    *buf++ = '[';
                    break;
                case '/':
                    *buf++ = '\\';
                    break;
                case ')':
                    *buf++ = ']';
                    break;
                case '\'':
                    *buf++ = '^';
                    break;
                case '<':
                    *buf++ = '{';
                    break;
                case '!':
                    *buf++ = '|';
                    break;
                case '>':
                    *buf++ = '}';
                    break;
                case '-':
                    *buf++ = '~';
                    break;
                default:
                    cp -= 3;
                    *buf++ = *cp++;
                    break;
            }
        }
        else
             *buf++ =  *cp++;
    }
    *buf = 0;
}

//-------------------------------------------------------------------------

int getstring(char *s, int len, int file)
{
    char *olds = s;
    while (TRUE)
    {
        while (inputlen--)
        {
            if (*ibufPtr == 0x1a)
            {
                *s = 0;
                inputlen = 0;
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
        inputlen = read(file, inputbuffer, sizeof(inputbuffer));
        ibufPtr = inputbuffer;
        if (inputlen <= 0)
        {
            *s = 0;
            inputlen = 0;
            return s == olds;
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
    int rv, rvc, i, prepping, temp = 0;

    static char ibuf[MACRO_REPLACE_SIZE], xbuf[MACRO_REPLACE_SIZE] ;
    char  *xptr;
    char *ptr = ibuf, dcrv;
    short *uptr;
    if (cantnewline)
    {
        return (0);
    }
    repeatit:
    do
    {
        rv = FALSE;
        prepping = FALSE;
        rvc = temp;
        lferror();
        add:

        while (rvc + 131 < MACRO_REPLACE_SIZE && !rv)
        {
            ++lineno;
            rv = getstring(ibuf + rvc, MACRO_REPLACE_SIZE-132-rvc, inputFile);
            if (rv)
                break;
            if (prm_listfile && !sys_inc)
            {
                fprintf(listFile, "%5d: %s", lineno, ibuf + rvc);
            }
            temp = rvc;
            if (prm_trigraph)
                striptrigraph(ibuf);
            rvc = strlen(ibuf);
            while (rvc && isspace(ibuf[rvc - 1]))
                rvc--;

            if (!rvc || ibuf[rvc - 1] != '\\')
                break;
            rvc--;
        }
        if (rvc)
            ibuf[rvc++] = '\n';
        ibuf[rvc] = 0;
        if (temp = stripcomment(ibuf, temp))
            rv = FALSE;
        rvc = strlen(ibuf);
        if (rvc)
            rv = FALSE;
        if (rv)
        {
            if (ifs)
                generror(ERR_PREPROCMATCH, 0, 0);
            if (commentlevel)
                generror(ERR_COMMENTMATCH, 0, 0);
            if (incldepth > 0)
            {
                close(inputFile);
                inputFile = inclfile[--incldepth];
                lineno = inclline[incldepth];
                infile = inclfname[incldepth];
                currentfile = inclcurrent[incldepth];
                inputlen = inclpos[incldepth];
                ibufPtr = inclptr[incldepth];
                sys_inc = inclsysflags[incldepth];
                memcpy(inputbuffer, incldata[incldepth], sizeof(inputbuffer));
                errlineno = lineno;
                errfile = infile;
                ifs = ifshold[incldepth];
                commentlevel = 0;
                popif();
                browse_startfile(infile);
                if (prm_listfile)
                    fprintf(listFile,"\n");
                goto repeatit;
            }
            currentfile = 0;
        }
        if (rv)
            return 1;
        lptr = inputline;
        ptr = ibuf;
        xptr = xbuf;
        uptr = unalteredline;
        while (*ptr)
        {
            *lptr++ =  *ptr;
            *xptr++ =  *ptr;
            *uptr++ =  *ptr++;
        }
        *lptr = 0;
        *xptr = 0;
        *uptr = 0;
        lptr = inputline;

        while (*lptr != '\n' && isspace(*lptr))
            lptr++;
        if (lptr[0] == '#' || (prm_trigraph && lptr[0] == '%' && lptr[1] == ':'))
        {
                        int v = lastlastst;
            if (prm_trigraph)
                stripdigraph(lptr);
            recordingOff = TRUE;
            errlineno = lineno;
            if (!commentlevel)
                listflag = preprocess();
            prepping = TRUE;
            recordingOff = FALSE;
                        lastlastst = v;
        }
    }
    while (ifskip || prepping)
        ;
    rvc = strlen(ibuf);
    if (defcheck(inputline) == INT_MIN+1 && rvc + 131 < MACRO_REPLACE_SIZE)
    {
        if (ibuf[rvc - 1] == '\n')
            ibuf[rvc - 1] = ' ';
        goto add;
    }
    //      for (i=0; i < pstrlen(inputline); i++)
    //         if (inputline[i] == '/' && inputline[i+1] == '/') {
    //            inputline[i] = ' ' ;
    //            inputline[i+1] = 0 ;
    //            break ;
    //         }
    lineToCpp();
    return 0;
}

/*
 *      getch - basic get character routine.
 */
int getch(void)

{
    if (interjectptr)
    {
        if (lastch == '\n')
        {
            short *p = interjectptr;
            short *q = unalteredline;
            while (*p &&  *p != '\n')
                *q++ =  *p++;
            *q++ = '\n';
            *q++ = 0;
        }
        if ((lastch =  *interjectptr++) == 0)
        {
            interjectptr--;
            lastch =  - 1;
        }
    }
    else
    while ((lastch =  *lptr++) == '\0')
    {
        if (cantnewline)
        {
            lptr--;
            lastch = ' ';
            break;
        }
        if (getline(incldepth == 0))
            return lastch =  - 1;
    }
    if ((recordingTemplate || recordingClassFunc) && !recordingOff && !ifskip)
        insertTemplateChar(lastch);
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
    if (prm_cmangle)
        lastid[i++] = '_';
     /* Mangling */
    if (lastch == 'L')
    {
        lastid[i++] = 'L';
        getch();
        if (lastch == '\"')
        {
            getch();
            i = 0;
            lastst = lsconst;
            while (lastch != '\"' && lastch)
            {
                if (i >= MAX_STRLEN/2) {
                    generror(ERR_STRINGTOOBIG,0,0);
                    return ;
                }
                *(((short*)(laststr)) + i++) = getsch(2);
            }
            if ((lastch &0x7f) != '\"')
                generror(ERR_NEEDCHAR, '\"', 0);
            else
                getch();
            *(((short*)(laststr)) + i) = 0;
            laststrlen = i;
            return ;
        }
        else if (lastch == '\'')
        {
            getch();
            ival = getsch(2);
            lastst = iconst;
            while (lastch != '\'' && lastch != '\n')
                getsch(2);
            if (lastch != '\'')
                generror(ERR_NEEDCHAR, '\'', 0);
            else
                getch();
            return ;
        }
    }
    while (issymchar(lastch))
    {
        if (i < 100)
            lastid[i++] = lastch;
        getch();
    }
    if ((lastid[i - 1] &0xf0) == 0x90)
        lastid[i - 1] = 0x90;
    lastid[i] = '\0';
    lastst = id;
}

/*
 *      getsch - get a character in a quoted string.
 *
 *      this routine handles all of the escape mechanisms
 *      for characters in strings and character constants.
 */
int getsch(int bytes) /* return an in-quote character */
{
    register int i, j;
    if (lastch == '\n')
        return INT_MIN;
    if (lastch == MACRO_PLACEHOLDER)
    {
        getch();
        return getsch(bytes);
    }
    if (incconst || lastch != '\\')
    {
        i = lastch;
        getch();
        return i;
    }
    do
    {
        getch(); /* get an escaped character */
    } while (lastch == MACRO_PLACEHOLDER);

    if (isdigit(lastch) && lastch < '8')
    {
        for (i = 0, j = 0; j < 3; ++j)
        {
            if (lastch <= '7' && lastch >= '0')
                i = (i << 3) + lastch - '0';
            else
                break;
            do
            {
                getch(); /* get an escaped character */
            } while (lastch == MACRO_PLACEHOLDER);
        }
        return i;
    }
    i = lastch;
    do
    {
        getch(); /* get an escaped character */
    } while (lastch == MACRO_PLACEHOLDER);
    switch (i)
    {
        case '\n':
            getch();
            return getsch(bytes);
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
        case 'x':
            {
                int n = 0, count = 0;
                while (isxdigit(lastch))
                {
                    count++;
                    if (lastch >= 0x60)
                        lastch &= 0xdf;
                    lastch -= 0x30;

                    if (lastch > 10)
                        lastch -= 7;
                    if (lastch > 15)
                        lastch -= 32;
                    n *= 16;
                    n += lastch;
                    do
                    {
                        getch(); /* get an escaped character */
                    } while (lastch == MACRO_PLACEHOLDER);
                }
//                if (count > bytes *2)
//                    generror(ERR_CONSTTOOLARGE, 0, 0);
                if (bytes == 1)// && n > UCHAR_MAX)
                    n = (int)(char)n;
                else if (bytes == 2)
                    n = (int)(short)n;
                return n;
            }
        default:
            return i;
    }
}

//-------------------------------------------------------------------------

int radix36(int c)
{
    if (isdigit(c))
        return c - '0';
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    return INT_MAX;
}

/*
 *      getbase - get an integer in any base.
 */
LLONG_TYPE getbase(int b, char **ptr)
{
    LLONG_TYPE i;
    int j;
    int errd = 0;
    i = 0;
    while ((j = radix36(**ptr)) < b)
    {
        (*ptr)++;
        if (i > (llminus1 - j) / b)
        if (!errd)
        {
            generror(ERR_CONSTTOOLARGE, 0, 0);
            errd++;
        }
        i = i * b + j;
    }
    lastst = iconst;
        return i;
}

/*
 *      getfrac - get fraction part of a floating number.
 */
static void getfrac(int radix, char **ptr, FPF *rval)
{
    ULLONG_TYPE i = 0;
    int pow = 0, j, k = 0;
    FPF temp, temp1;
    while ((j = radix36(**ptr)) < radix)
    {
        i = radix * i + j;
        pow--;
        if (++k == sizeof(i) * 2)
        {
            UnsignedLongLongToFPF(&temp, i);
            if (radix == 10)
                FPFMultiplyPowTen(&temp, pow);
            else
                temp.exp += 4 * pow;
            AddSubFPF(0,rval,&temp,&temp1);
            *rval = temp1;
            k = 0;
            i = 0;
        }
        (*ptr)++;
    }
    UnsignedLongLongToFPF(&temp, i);
    if (radix == 10)
        FPFMultiplyPowTen(&temp, pow);
    else
        temp.exp += 4 * pow;
    AddSubFPF(0,rval,&temp,&temp1);
    *rval = temp1;
}

/*
 *      getexp - get exponent part of floating number.
 *
 *      this algorithm is primative but usefull.  Floating
 *      exponents are limited to +/-255 but most hardware
 *      won't support more anyway.
 */
static int getexp(int radix, char **ptr)
{
    BOOL neg = FALSE;
        int iv;
    if (**ptr == '-')
    {
        neg = TRUE;
        (*ptr)++;
    }
    else
    {
        if (**ptr == '+')
            (*ptr)++;
    }
    iv = getbase(10, ptr);
    if (neg)
        iv =  - iv;
        return iv;
}

/*
 *      getnum - get a number from input.
 *
 *      getnum handles all of the numeric input. it accepts
 *      decimal, octal, hexidecimal, and floating point numbers.
 */
void getnum(void)
{
    char buf[200],  *ptr = buf;
    int hasdot = FALSE;
    int radix = 10;
    int floatradix = 0;

    if (lastch == '0')
    {
        getch();
        if (lastch == 'x' || lastch == 'X')
        {
            getch();
            radix = 16;
        }
                else if (!prm_ansi && (lastch == 'b' || lastch == 'B'))
                {
                        getch();
                        radix = 2;
                }
        else
            radix = 8;
    }
    while (radix36(lastch) < radix)
    {
        *ptr++ = lastch;
        getch();
    }
    if (lastch == '.')
    {
        if (radix == 8)
            radix = 10;
        *ptr++ = lastch;
        getch();
        while (radix36(lastch) < radix)
        {
            *ptr++ = lastch;
            getch();
        }
    }
    if ((lastch == 'e' || lastch == 'E') && radix != 16)
        radix = floatradix = 10;
    else if ((lastch == 'p' || lastch == 'P') && radix == 16)
    {
        floatradix = 2;
    }

    if (floatradix)
    {
        *ptr++ = lastch;
        getch();
        if (lastch == '-' || lastch == '+')
        {
            *ptr++ = lastch;
            getch();
        }
        while (radix36(lastch) < 10)
        {
            *ptr++ = lastch;
            getch();
        }
    }

    *ptr = 0;
    ptr = buf;
    // at this point the next char is any qualifier after the number
    if (radix36(*ptr) < radix)
        ival = getbase(radix, &ptr);
    else
    {
        ival = 0;
        lastst = iconst;
    }
        if (ival == 0 && radix == 8)
                radix = 10;
    if (*ptr == '.')
    {
        ptr++;
        UnsignedLongLongToFPF(&rval, ival);
        ival = 0;
        getfrac(radix, &ptr, &rval);
        lastst = rconst;
    }
    if (*ptr == 'e' ||  *ptr == 'E' ||  *ptr == 'p' ||  *ptr == 'P')
    {
        if (lastst != rconst)
        {
            UnsignedLongLongToFPF(&rval, ival);
        }
        ptr++;
        ival = getexp(floatradix, &ptr);
        lastst = rconst;
    }
    if (lastst != rconst)
    {
        if (lastch == 'i' || lastch == 'I')
        {
            if (!prm_ansi &&  *lptr == '6' && *(lptr + 1) == '4')
            {
                getch();
                getch();
                getch();
                lastst = llconst;
            }
        }
        else if (lastch == 'U' || lastch == 'u')
        {
            getch();
                if (lastch == 'i' || lastch == 'I')
                {
                    if (!prm_ansi &&  *lptr == '6' && *(lptr + 1) == '4')
                    {
                        getch();
                        getch();
                        getch();
                        lastst = lluconst;
                    }
                }
            else if (lastch == 'L' || lastch == 'l')
            {
                getch();
                if ((lastch == 'L' || lastch == 'l'))
                {
                                        if (!prm_c99)
                                                gensymerror(ERR_TYPE_C99, "unsigned long long");
                    lastst = lluconst;
                    getch();
                }
                else
                {
                    if (ival > ULONG_MAX)
                                        {
                                                if (!prm_c99)
                                                        gensymerror(ERR_TYPE_C99, "unsigned long long");
                        lastst = lluconst;
                                        }
                    else
                                        {
                        lastst = luconst;
                                        }
                }
            }
            else
            {
                if (ival > ULONG_MAX)
                                {
                                        if (!prm_c99)
                                                gensymerror(ERR_TYPE_C99, "unsigned long long");
                    lastst = lluconst;
                                }
                else
                                {
                    lastst = iuconst;
                                }
            }
        }
        else if (lastch == 'L' || lastch == 'l')
        {
            getch();
            if (lastch == 'L' || lastch == 'l')
            {
                getch();
                if (lastch == 'U' || lastch == 'u')
                {
                                        if (!prm_c99)
                                                gensymerror(ERR_TYPE_C99, "unsigned long long");
                    getch();
                    lastst = lluconst;
                }
                else
                                {
                                        if (!prm_c99)
                                                gensymerror(ERR_TYPE_C99, "long long");
                    lastst = llconst;
                                }
            }

            else if (lastch == 'U' || lastch == 'u')
            {
                if (prm_c99 && ival > ULONG_MAX)
                                {
                    lastst = lluconst;
                                }
                else
                    lastst = luconst;
                getch();
            }
            else
            {
                if (prm_c99) {
                  if (radix == 10) {
                    if (ival > LONG_MAX || ival < LONG_MIN)
                      lastst = llconst;
                    else lastst = lconst;
                  }
                  else {
//                    if (ival > LLONG_MAX || ival < LLONG_MIN)
//                      lastst = lluconst;
//                    else
                                        if (ival > ULONG_MAX)
                      lastst = llconst;
                    else if (ival > LONG_MAX || ival < LONG_MIN)
                      lastst = luconst;
                    else lastst = lconst;
                  }
                }
                else
                    lastst = lconst;
            }
        }
        else
        {
            if (prm_c99) {
              if (radix == 10) {
                if (ival > LONG_MAX || ival < LONG_MIN)
                  lastst = llconst;
                else lastst = lconst;
              }
              else {
//                if (ival > LLONG_MAX || ival < LLONG_MIN)
//                  lastst = lluconst;
//                else
                                if (ival > ULONG_MAX)
                                {
                  lastst = llconst;
                                }
                else if (ival > LONG_MAX || ival < LONG_MIN)
                                {
                  lastst = luconst;
                                }
                else lastst = lconst;
              }
            }
        }
    }
    else
    {
        long double l;  /* convert to optimize float const, e.g. .12e2 == 12.0 */
        if (floatradix == 2)
        {
           rval.exp += ival;
        }
        else
                {
           FPFMultiplyPowTen(&rval,ival);
        }
        if (lastch == 'F' || lastch == 'f')
        {
            float f;
            lastst = fconst;
                        CastToFloat(ISZ_FLOAT, &rval);
            getch();
            FPFToFloat(&f, &rval);
            l = f;
        }
        else if (lastch == 'L' || lastch == 'l')
        {
            lastst = lrconst;
                        CastToFloat(ISZ_LDOUBLE, &rval);
            getch();
            FPFToLongDouble(&l, &rval);
        } else
        {
            double d;
                        CastToFloat(ISZ_DOUBLE, &rval);
            FPFToDouble(&d, &rval);
            l = d;
        }
        LongDoubleToFPF(&rval, &l);
    }
    if (isstartchar(lastch) || isdigit(lastch))
    {
        generror(ERR_INVCONST, 0, 0);
        while ((isstartchar(lastch) || isdigit(lastch)) && lastch != eof)
            getch();
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
    swlp: switch (lastch)
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
                    if (prm_cplusplus && lastch == '*')
                    {
                        getch();
                        lastst = pointstar;
                    }
                    else
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
                if (prm_cplusplus && lastch == ':')
                {
                    getch();
                    if (lastch == '*')
                    {
                        getch();
                        lastst = classselstar;
                    }
                    else
                        lastst = classsel;
                }
                else
            if (lastch == '>')
            {
                getch();
                lastst = closebr;
            }
            else
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
                sys_inc = TRUE;
                for (i = 0; i < MAX_STRLEN; ++i)
                {
                    if (lastch == '>')
                        break;
                    if ((j = getsch(1)) == INT_MIN)
                        break;
                    else
                        laststr[i] = j;
                }
                laststr[i] = 0;
                lastst = sconst;
                laststrlen = i;
                if (lastch != '>')
                    generror(ERR_NEEDCHAR, '>', 0);
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
            else if (lastch == ':')
            {
                getch();
                lastst = openbr;
            }
            else if (lastch == '%')
            {
                getch();
                lastst = begin;
            }
            else
                lastst = lt;
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
                if (i == 1)
                    ival &= 0xff;
                // get rid of sign if this is multibyte
                j = getsch(1); /* get a string char */
                ival += j << k;
            }
            if (i == 0)
                generror(ERR_CHAR4CHAR, 0, 0);
            else
                while (lastch != '\'' && lastch != '\n')
                    getsch(1);
            if (lastch != '\'')
                generror(ERR_NEEDCHAR, '\'', 0);
            else
                getch();
            if (prm_cplusplus)
                lastst = cconst;
            else
                lastst = iconst;
            break;
        case 0x2d4:
            getch();
            i = 0;
            lastst = sconst;
            while (lastch != '\"' && lastch) {
                if (i >= MAX_STRLEN) {
                    generror(ERR_STRINGTOOBIG,0,0);
                    break;
                }
                laststr[i++] = lastch;
            }
            laststr[i] = 0;
            if ((lastch &0x7f) != '\"')
                generror(ERR_NEEDCHAR, '\"', 0);
            else
                getch();
            laststrlen = i;
            size = i;
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
                    if ((j = getsch(1)) == INT_MIN)
                        break;
                    else
                        laststr[i] = j;
                }
                laststr[i] = 0;
                laststrlen = size = i;
                lastst = sconst;
                if (lastch != '\"')
                    generror(ERR_NEEDCHAR, '\"', 0);
                else
                    getch();
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
            else if (lastch == '>')
            {
                getch();
                lastst = end;
            }
            else if (lastch == ':')
            {
                getch();
                lastst = hash;
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
                    if (prm_cplusplus && lastch == '*')
                    {
                        getch();
                        lastst = dotstar;
                    }
                    else
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
                        generror(ERR_ILLCHAR, lastch, 0);
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
        case '#':
            getch();
            lastst = hash;
            break;
            //                case SYM_ASSIGN :
            //                       getch() ;
            //                        lastst = assign ;
            //                        break ;
            //                case SYM_SHIFTASSIGN :
            //                        getch() ;
            //                        lastst = lassign ;
            //                        break ;
        case '@':
             /* @= is same as above */
            if (!prm_ansi)
            {
                getch();
                if (lastch == '=')
                {
                    getch();
                    lastst = lassign;
                    break;
                }
                else
                    lastch =  *--lptr;
            }
            // FALL THROUGH
        default:
            #ifdef XXXXX
                if (iscommentchar(lastch))
                {
                    do
                    {
                        getch();
                    }
                    while (!iscommentchar(lastch) && lastch != '\n');
                }
                else
            #endif
                generror(ERR_ILLCHAR, lastch, 0);
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
    int done;
    lastlastst = lastst ;
    do
    {
        done = TRUE;
        sym_charindex = lptr - inputline;
        errlineno = lineno;
        if (backupchar !=  - 1)
        {
            lastst = backupchar;
            if (lastst == id)
                strcpy(lastid, backupid);
            backupchar =  - 1;
            return ;
        }
        if (cantnewline && ! *lptr)
        {
            lastst = eol;
            return ;
        }
        while (isspace(lastch) || lastch == MACRO_PLACEHOLDER)
        {
            getch();
            if (cantnewline && ! *lptr)
            {
                lastst = eol;
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
            if (lastst == kw__Pragma)
            {
                Compile_Pragma();
                done = FALSE;
            }
        }
        else
            getsym2();
    }
    while (!done)
        ;
}

/*
 * when we need specific punctuation, call one of these routines
 */
int needpunc(enum e_sym p, int *skimlist)
{
    if (lastst == p)
    {
        getsym();
        return (TRUE);
    }
    else
        expecttoken(p, skimlist);
    return (FALSE);
}

//-------------------------------------------------------------------------

int needpuncexp(enum e_sym p, int *skimlist)
{
    if (lastst == p)
    {
        getsym();
        return (TRUE);
    }
    else
        expecttokenexp(p, skimlist);
    return (FALSE);
}

/*
 * having to back up a character is rare, but sometimes...
 */
void backup(int st)
{
    if (lastst == id)
        strcpy(backupid, lastid);
    backupchar = lastst;
    lastst = st;
}
