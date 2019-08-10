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

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <math.h>

#include "header.h"
#include "dbgtype.h"
#include <ctype.h>

#define powl(x, y) pow((x), (y))
#define PUSH(xxx) varinfo_list[varinfo_count++] = xxx
#define POP() varinfo_list[--varinfo_count]

static VARINFO* varinfo_list[200];
static int varinfo_count;

extern THREAD* activeThread;
extern unsigned bitmask[];

#define NVAL 0
#define IVAL 1
#define IUVAL 2
#define RVAL 4

static LLONG_TYPE ival;
static long double rval;
static int lastst;

static VARINFO* iecondop(char** text, DEBUG_INFO** dbg, SCOPE* sc, int towarn);
static void freeall(void)
{
    int i;
    for (i = 0; i < varinfo_count; i++)
        FreeVarInfo(varinfo_list[i]);
    varinfo_count = 0;
}
static void skipspace(char** text)
{
    while (**text && isspace(**text))
        (*text)++;
}
static VARINFO* ieerr(char** text, VARINFO* v1, VARINFO* v2, char* msg, int towarn)
{
    if (v1)
        FreeVarInfo(v1);
    if (v2)
        FreeVarInfo(v2);
    freeall();
    if (towarn)
    {
        if (text)
            (*text) += strlen(*text);
        ExtendedMessageBox("Invalid Expression", MB_SETFOREGROUND | MB_SYSTEMMODAL, "%s", msg);
    }
    return 0;
}
static int radix36(char c)
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
static void getbase(int b, char** ptr, int towarn)
{
    LLONG_TYPE i;
    int j;
    int errd = 0;
    i = 0;
    while ((j = radix36(**ptr)) < b)
    {
        (*ptr)++;
        if (i > (unsigned LLONG_TYPE)(((LLONG_TYPE)-1) - j) / b)
            if (!errd)
            {
                ieerr(0, 0, 0, "Constant too large", towarn);
                errd++;
            }
        i = i * b + j;
    }
    ival = i;
    lastst = IVAL;
}

/*
 *      getfrac - get fraction part of a floating number.
 */
static void getfrac(int radix, char** ptr)
{
    long double frmul;
    frmul = 1.0 / radix;
    while (radix36(**ptr) < radix)
    {
        rval += frmul * radix36(*(*ptr)++);
        frmul /= radix;
    }
    lastst = RVAL;
}

/*
 *      getexp - get exponent part of floating number.
 *
 *      this algorithm is primative but usefull.  Floating
 *      exponents are limited to +/-255 but most hardware
 *      won't support more anyway.
 */
static void getexp(int radix, char** ptr, int towarn)
{
    int neg = FALSE;
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
    getbase(10, ptr, towarn);
    if (ival > LDBL_MAX_10_EXP)
    {
        ieerr(0, 0, 0, "Exponent too large", towarn);
        ival = 0;
    };
    if (neg)
        ival = -ival;
    if (radix == 10)
        rval *= powl(10.0, (long double)ival);
    else
    {
        rval *= powl(2.0, (long double)ival);
    }
    lastst = RVAL;
}

/*
 *      inputNum - get a number from input.
 *
 *      inputNum handles all of the numeric input. it accepts
 *      decimal, octal, hexidecimal, and floating point numbers.
 */
static void inputNum(char** text, int towarn)
{
    char buf[200], *ptr = buf;
    int hasdot = FALSE;
    int radix = 10;
    int floatradix = 0;

    lastst = NVAL;
    if (**text == '0')
    {
        (*text)++;
        if (**text == 'x' || **text == 'X')
        {
            (*text)++;
            radix = 16;
        }
        else
            radix = 8;
    }
    else
    {
        char* t = *text;
        while (isxdigit(*t))
            t++;
        if (*t == 'H' || *t == 'h')
            radix = 16;
    }
    while (radix36(**text) < radix)
    {
        *ptr++ = **text;
        (*text)++;
    }
    if (radix == 16 && (**text == 'H' || **text == 'h'))
        (*text)++;
    if (**text == '.')
    {
        if (radix == 8)
            radix = 10;
        *ptr++ = **text;
        (*text)++;
        while (radix36(**text) < radix)
        {
            *ptr++ = **text;
            (*text)++;
        }
    }
    if ((**text == 'e' || **text == 'E') && radix != 16)
        radix = floatradix = 10;
    else if ((**text == 'p' || **text == 'P') && radix == 16)
        floatradix = 2;

    if (floatradix)
    {
        *ptr++ = **text;
        (*text)++;
        if (**text == '-' || **text == '+')
        {
            *ptr++ = **text;
            (*text)++;
        }
        while (radix36(**text) < 10)
        {
            *ptr++ = **text;
            (*text)++;
        }
    }

    *ptr = 0;
    ptr = buf;
    // at this point the next char is any qualifier after the number

    if (radix36(*ptr) < radix)
        getbase(radix, &ptr, towarn);
    else
    {
        ival = 0;
        lastst = IVAL;
    }
    if (*ptr == '.')
    {
        ptr++;
        rval = ival;
        getfrac(radix, &ptr);
    }
    if (*ptr == 'e' || *ptr == 'E' || *ptr == 'p' || *ptr == 'P')
    {
        if (lastst != RVAL)
        {
            rval = ival;
        }
        ptr++;
        getexp(floatradix, &ptr, towarn);
    }
    if (lastst != RVAL)
    {
        if (**text == 'i')
        {
            if (**text == '6' && *(*text + 1) == '4')
            {
                (*text)++;
                (*text)++;
                (*text)++;
            }
        }
        else if (**text == 'U' || **text == 'u')
        {
            lastst = IUVAL;
            (*text)++;
            if (**text == 'L' || **text == 'l')
            {
                (*text)++;
                if (**text == 'L' || **text == 'l')
                {
                    (*text)++;
                }
            }
        }
        else if (**text == 'L' || **text == 'l')
        {
            (*text)++;
            if (**text == 'L' || **text == 'l')
            {
                (*text)++;
                if (**text == 'U' || **text == 'u')
                {
                    (*text)++;
                    lastst = IUVAL;
                }
            }

            else if (**text == 'U' || **text == 'u')
            {
                lastst = IUVAL;
                (*text)++;
            }
        }
    }
    else
    {
        if (**text == 'F' || **text == 'f')
        {
            if (lastst != RVAL)
            {
                rval = ival;
            }
            lastst = RVAL;
            (*text)++;
        }
        else if (**text == 'L' || **text == 'l')
        {
            if (lastst != RVAL)
            {
                rval = ival;
            }
            lastst = RVAL;
            (*text)++;
        }
    }
    if (isalnum(**text) || (**text) == '_')
    {
        lastst = NVAL;
    }
}
static int pointedsize(DEBUG_INFO** dbg, VARINFO* var)
{
    VARINFO* p = var->subtype;
    int rv = 0;
    var->subtype = NULL;
    if (var->type <= eULongLong || (var->type >= eFloat && var->type <= eComplexLongDouble))
    {
        rv = basictypesize(var->type);
    }
    else
    {
        ExpandPointerInfo(*dbg, var);
        if (var->subtype->type <= eULongLong || (var->subtype->type >= eFloat && var->subtype->type <= eComplexLongDouble))
        {
            rv = basictypesize(var->subtype->type);
        }
        else if (var->pointer)
            rv = 4;
        else if (var->structure || var->unionx)
            rv = var->arraysize;
        else if (var->array)
            rv = var->arraysize;
        else
            rv = 1; /* should never get here */
        FreeVarInfo(var->subtype);
    }
    var->subtype = p;
    return rv;
}
static VARINFO* constnode(char** text, int towarn)
{
    inputNum(text, towarn);
    if (lastst == NVAL)
        return ieerr(text, 0, 0, "Invalid constant", towarn);
    if (lastst == RVAL)
    {
        VARINFO* v = calloc(sizeof(VARINFO), 1);
        if (!v)
            return 0;
        v->constant = 1;
        v->fval = rval;
        v->type = eLongDouble;
        return v;
    }
    else if (lastst == IVAL || lastst == IUVAL)
    {
        VARINFO* v = calloc(sizeof(VARINFO), 1);
        if (!v)
            return 0;
        v->constant = 1;
        v->ival = ival;
        v->type = lastst == IVAL ? eInt : eUInt;
        return v;
    }
    return 0;
}
static void EvalBasicType(VARINFO* var, int* signedtype, SCOPE* scope)
{
    char data[12];
    int type = HintBasicValue(var, signedtype, data);
    var->constant = TRUE;
    switch (type)
    {
        case eChar:
            var->ival = *(LLONG_TYPE*)data;
            break;
        default:
            var->ival = 0;
            var->fval = 0;
            var->fvali = 0;
            break;
        case eInt:
            if (signedtype)
                var->ival = *(int*)data;
            else
                var->ival = *(unsigned*)data;
            break;
        case eBool:
            var->ival = data[0];
            break;
        case eFloat:
        case eImaginary:
            var->fval = *(float*)data;
            break;
        case eDouble:
        case eImaginaryDouble:
            var->fval = *(long double*)data;
            break;
        case eLongDouble:
        case eImaginaryLongDouble:
            var->fval = *(double*)data;
            break;
        case eComplex:
            var->fval = *(float*)data;
            var->fvali = *(float*)(data + 4);
            break;
        case eComplexDouble:
            var->fval = *(double*)data;
            var->fvali = *(double*)(data + 8);
            break;
        case eComplexLongDouble:
            var->fval = *(long double*)data;
            var->fvali = *(long double*)(data + 10);
            break;
    }
}
static VARINFO* makeconst(VARINFO* var, SCOPE* scope)
{
    int signedtype;
    if (var->constant)
        return var;
    if ((var->type <= eULongLong || (var->type >= eFloat && var->type <= eComplexLongDouble)) && !var->array)
    {
        EvalBasicType(var, &signedtype, scope);
        if (var->bitfield)
        {
            if (signedtype)
            {
                var->ival <<= 32 - var->bitstart - var->bitlength;
                var->ival >>= 32 - var->bitlength;
            }
            else
            {
                var->ival >>= var->bitstart;
                var->ival &= bitmask[var->bitlength - 1];
            }
        }
    }
    else
    {
        var->constant = TRUE;
        var->type = eUInt;
        if (var->pointer)
        {
            var->ival = 0;
            if (var->derefaddress != -1)
                var->ival = var->derefaddress;
            else
                ReadValue(var->ival, &var->address, 4, var);
            var->pointer = FALSE;
        }
        else
            var->ival = var->address;
        FreeVarInfo(var->subtype);
        var->subtype = 0;
    }
    return var;
}
static void truncateconst(VARINFO* var1, VARINFO* var2)
{
    // nop
}
static VARINFO* lookupsym(char** text, DEBUG_INFO** dbg, SCOPE* sc, int towarn)
{
    char buf[2048], *p = buf;
    VARINFO* var;
    SCOPE *next, *sc1;
    int offset = 0;
    int type = 0;
    int address = 0;
    BOOL global = FALSE;
    if (sc)
        next = sc->next;
    while (isspace(**text))
        (*text)++;
    if (**text == ':')
    {
        if ((*text)[1] != ':')
        {
            return NULL;
        }
        *text += 2;
        global = TRUE;
    }
    GetQualifiedName(buf, text, FALSE, towarn);
    if (!strrchr(buf + 1, '@') && !strrchr(buf + 1, '#'))
    {
        p = buf + 1;
        // lookup for member variables within the scope of the this ptr
        if (!global && sc && strcmp(buf + 1, "this"))
        {
            DEBUG_INFO* dbg1;
            SCOPE *next = sc->next, *sc1;
            char buf1[4096];
            sc->next = NULL;
            // but we have to prefer parameters and vars defined in the function
            if ((sc1 = FindSymbol(dbg, sc, (char*)p, &address, &type)))
            {
                var = GetVarInfo(*dbg, (char*)p, address, type, sc1, activeThread);
                sc->next = next;
                return var;
            }
            sc->next = next;
            sprintf(buf1, "this->%s", p);
            var = EvalExpr(dbg, sc, buf1, FALSE);
            if (var)
            {
                return var;
            }
        }
    }
    else
    {
        p = buf;
    }
    if (sc && !strcmp(p, "this"))
        sc->next = NULL;  // only search for 'this' in the local scope
    if ((sc1 = FindSymbol(dbg, sc, (char*)p, &address, &type)))
    {
        var = GetVarInfo(*dbg, (char*)p, address, type, sc1, activeThread);
        //            if (var->udt)
        //                return ieerr(text, var, 0, "Can't use type here", towarn);
        sc->next = next;
        return var;
    }
    if (sc)
    {
        char fn[2048], *q = fn + 1;
        sc->next = next;
        // one final search for function scoped statics
        // these will only be found if not obscured by some other name,
        // regardless of scope.
        if (FindFunctionName(q, sc->address, NULL, NULL))
        {
            if (q[0] != '@')
                *(--q) = '@';
            strcat(q, buf);
            if ((sc1 = FindSymbol(dbg, sc, (char*)q, &address, &type)))
            {
                var = GetVarInfo(*dbg, (char*)q, address, type, sc1, activeThread);
                sc->next = next;
                return var;
            }
        }
    }
    return NULL;
}
typedef struct
{
    char* str;
    int type;
    char* mstr;
} CASTTYPE;

static CASTTYPE regs[] = {
    {"al", DB_REG_AL},   {"ah", DB_REG_AH},   {"bl", DB_REG_BL},   {"bh", DB_REG_BH},   {"cl", DB_REG_CL},   {"ch", DB_REG_CH},
    {"dl", DB_REG_DL},   {"dh", DB_REG_DH},   {"ax", DB_REG_AX},   {"bx", DB_REG_BX},   {"cx", DB_REG_CX},   {"dx", DB_REG_DX},
    {"sp", DB_REG_SP},   {"bp", DB_REG_BP},   {"si", DB_REG_SI},   {"di", DB_REG_DI},   {"eax", DB_REG_EAX}, {"ebx", DB_REG_EBX},
    {"ecx", DB_REG_ECX}, {"edx", DB_REG_EDX}, {"esp", DB_REG_ESP}, {"ebp", DB_REG_EBP}, {"esi", DB_REG_ESI}, {"edi", DB_REG_EDI},
};
static VARINFO* regnode(char** text)
{
    int i, l;
    VARINFO* var1;
    char* p = *text;
    skipspace(&p);
    for (i = 0; i < sizeof(regs) / sizeof(regs[0]); i++)
        if (!strncmp(p, regs[i].str, l = strlen(regs[i].str)) && !isalnum(p[l]) && p[l] != '_')
            break;
    if (i >= sizeof(regs) / sizeof(regs[0]))
        return 0;
    *text += strlen(regs[i].str);
    var1 = calloc(sizeof(VARINFO), 1);
    if (!var1)
        return 0;
    var1->type = eInt;
    var1->address = regs[i].type;
    var1->explicitreg = 1;
    return var1;
}
static CASTTYPE casts[] = {{"int", eInt, "i"},
                           {"long", eInt, "l"},
                           {"long long", eLongLong, "L"},
                           {"unsigned int", eUInt, "ui"},
                           {"unsigned", eUInt, "ui"},
                           {"unsigned long", eUInt, "ul"},
                           {"unsigned long long", eULongLong, "uL"},
                           {"short", eShort, "s"},
                           {"unsigned short", eUShort, "us"},
                           {"char", eChar, "c"},
                           {"unsigned char", eUChar, "uc"},
                           {"bool", eBool, "4bool"},
                           {"float", eFloat, "f"},
                           {"double", eDouble, "d"},
                           {"long double", eLongDouble, "g"},
                           {"float imaginary", eImaginary, ""},
                           {"double imaginary", eImaginaryDouble, ""},
                           {"long double imaginary", eImaginaryLongDouble, ""}};
int GetConstVol(char** srcin)
{
    int flags = 0;
    char* src = *srcin;
    while (1)
    {
        while (isspace(*src))
            src++;
        if (!strncmp(src, "const", 5) && !isalnum(src[5]) && src[5] != '_')
        {
            src += 5;
            flags |= 1;
        }
        else if (!strncmp(src, "volatile", 5) && !isalnum(src[8]) && src[8] != '_')
        {
            src += 8;
            flags |= 2;
        }
        else
            break;
    }
    *srcin = src;
    return flags;
}
char* GetQualifiers(char* dest, char** srcin, int flags)
{
    char* src = *srcin;
    flags |= GetConstVol(&src);
    while (isspace(*src))
        src++;
    if (*src == '&' || *src == '*')
    {
        char c = *src++ == '&' ? 'r' : 'p';
        dest = GetQualifiers(dest, &src, 0);
        dest += strlen(dest);
        *dest++ = c;
        *dest = 0;
    }
    if (flags & 1)
        *dest++ = 'x';
    else if (flags & 2)
        *dest++ = 'y';
    *dest = 0;
    *srcin = src;
    return dest;
}

void SimplifyNames(char* dest)
{
    // this may be a little off in terms of the chosen indexes
    int count = 0, i;
    char names[10][512];
    char* p = dest;
    while (*p && count < 10)
    {
        if (isdigit(*p))
        {
            int n = atoi(p);
            while (isdigit(*p))
                p++;
            if (isalpha(*p) || *p == '_')
            {
                for (i = 0; i < count; i++)
                    if (strlen(names[i]) == n && !strncmp(names[i], p, n))
                        break;
                if (i == count)
                {
                    strncpy(names[count], p, n);
                    names[count++][n] = 0;
                }
            }
        }
        else
            p++;
    }
    if (count)
    {
        int found[10];
        memset(found, 0, sizeof(found));
        p = dest;
        while (*p)
        {
            if (isdigit(*p))
            {
                int n = atoi(p);
                char* q = p;
                while (isdigit(*q))
                    q++;
                if (isalpha(*q) || *q == '_')
                {
                    for (i = 0; i < count; i++)
                        if (strlen(names[i]) == n && !strncmp(names[i], q, n))
                            break;
                    if (i != count && found[i]++)
                    {
                        char bf[20];
                        int digits = p - q;
                        sprintf(bf, "n%d", i);
                        strncpy(p, bf, strlen(bf));
                        p += strlen(bf);
                        memcpy(p, p + n - digits - 2, strlen(p) - (n - digits - 2) + 1);
                    }
                    else
                    {
                        p = q;
                    }
                }
                else
                    p = q;
            }
            else
                p++;
        }
    }
}
void GetQualifiedName(char* dest, char** src, BOOL type, BOOL towarn)
{
    // not handling const/volatile yet
    int i = 0;
    char* p = dest;
    int flags = 0;
    if (!(*src))
        return;
    if (!type)
    {
        *dest = 0;
        while (isspace(**src))
            (*src)++;
    }
    if (type)
    {
        int l;
        flags = GetConstVol(src);
        for (i = 0; i < sizeof(casts) / sizeof(casts[0]); i++)
            if (!strncmp((*src), casts[i].str, l = strlen(casts[i].str)) && !isalnum((*src)[l]) && (*src)[l] != '_')
                break;
        if (i < sizeof(casts) / sizeof(casts[0]))
        {
            char* q;
            *src += strlen(casts[i].str);
            q = GetQualifiers(p, src, flags);
            strcpy(q, casts[i].mstr);
            return;
        }
        else if (!strcmp((*src), "struct "))
            (*src) += 7;
        else if (!strcmp((*src), "union "))
            (*src) += 6;
        else if (!strcmp((*src), "enum "))
            (*src) += 5;
    }
    *p++ = '@';
    while (*(*src))
    {
        if (*(*src) == ':')
        {
            if ((*src)[1] != ':')
            {
                *p = 0;
                return;
            }
            if (!(*src)[2])
            {
                break;
            }
            (*src) += 2;
            while (isspace(*(*src)))
                (*src)++;
            *p++ = '@';
            *p = 0;
        }
        else if (*(*src) == '<')
        {
            // this isn't perfect, it won't create template names with default arguments
            char buf1[2048], *q = buf1, *r = p;

            while (isalnum(r[-1]) || r[-1] == '_')
                r--;
            *q++ = '#';
            strcpy(q, r);
            q += strlen(q);
            *q++ = '$';
            (*src)++;
            while (1)
            {
                while (isspace(*(*src)))
                    (*src)++;
                GetQualifiedName(q, src, TRUE, towarn);
                q = q + strlen(q);
                while (isspace(*(*src)))
                    (*src)++;
                if (*(*src) == ',')
                    (*src)++;
                else
                    break;
            }
            *q++ = '~';
            *q++ = 0;
            if (**src != '>')
                return;
            (*src)++;
            strcpy(r, buf1);
            p = r + strlen(r);
        }
        else if (isspace(*(*src)))
        {
            while (isspace(*(*src)))
                (*src)++;
            if (*(*src) != '<' && *(*src) != ':')
                break;
        }
        else if (!type && !strncmp((*src), "operator", 8) && !isalnum((*src)[8]) && (*src)[8] != '_')
        {
            extern char* cpp_funcname_tab[1];
            extern char* xlate_tab[1];
            char *q = (*src) + 8, **search = xlate_tab + 3;
            int i = 3;
            while (isspace(*q))
                q++;
            while (*search)
            {
                if (!strncmp(*search, q, strlen(*search)))
                    break;
                i++;
                search++;
            }
            if (*search)
            {
                *src = q + strlen(*search);
                strcpy(p, cpp_funcname_tab[i]);
                p += strlen(p);
            }
            else
            {
                if (towarn)
                    ExtendedMessageBox("Evaluation", 0, "Invalid operator");
                return;
            }
            break;
        }
        else if (isalnum(*(*src)) || *(*src) == '_' || *(*src) == '~')
        {
            *p++ = *(*src)++;
            *p = 0;
        }
        else
            break;
    }
    // check for constructors and destructors
    if (!type)
    {
        int in = 0;
        extern char* cpp_funcname_tab[1];
        p = dest + strlen(dest) - 1;
        while (p > dest)
        {
            if (*p == '~')
            {
                if (!in && p[-1] == '@')
                    break;
                in++;
            }
            if (*p == '#')
            {
                if (!in)
                    break;
                in--;
            }
            if (!in && *p == '@')
                break;
            p--;
        }
        if (*p == '~')
        {
            in = strlen(p + 1);
            if (p - in > dest && !strncmp(p - in - 1, p + 1, strlen(p + 1)))
            {
                strcpy(p, cpp_funcname_tab[1]);  // This results in an OOB, is this intentional?
            }
        }
        else
        {
            in = strlen(p + 1);
            if (p - in > dest && !strncmp(p - in, p + 1, strlen(p + 1)))
            {
                strcpy(p + 1, cpp_funcname_tab[0]);
            }
        }
        SimplifyNames(dest);
        p = dest + strlen(dest);
    }
    else  // used as a template argument
    {
        char len[200], *q = len;
        char* z = dest;
        int n = strlen(dest);
        if (!strchr(z + 1, '@'))
            z++;
        q = GetQualifiers(q, src, flags);
        sprintf(q, "%d%s", strlen(z), z);
        strcpy(dest, len);
        p = dest + strlen(dest);
    }
}
static VARINFO* castnode(char** text, DEBUG_INFO** dbg, SCOPE* sc, int towarn)
{
    BOOL hasUDT = FALSE;
    VARINFO* var1 = 0;
    int tp, i;
    int indir = 0, l;
    char* start = (*text) + 1;
    char buf[2048], *q = buf;

    GetQualifiedName(q, &start, TRUE, towarn);
    while (*q == 'r' || *q == 'p' || *q == 'x' || *q == 'y')
    {
        if (*q == 'p' || *q == 'r')
            indir++;
        q++;
    }
    if (isdigit(*q))
    {
        while (isdigit(*q))
            q++;
        if ((tp = FindTypeSymbol(dbg, sc, (char*)q)) == 0)
        {
            if ((tp = FindTypeSymbol(dbg, sc, (char*)q + 1)) == 0)
            {
                q[-1] = '@';
                if ((tp = FindTypeSymbol(dbg, sc, (char*)q - 1)) == 0)
                    return 0;
            }
        }
        hasUDT = TRUE;
    }
    else
    {
        for (i = 0; i < sizeof(casts) / sizeof(casts[0]); i++)
            if (!strcmp(q, casts[i].mstr))
            {
                tp = casts[i].type;
                break;
            }
        if (i >= sizeof(casts) / sizeof(casts[0]))
            return 0;
    }
    while (isspace(*start))
        start++;
    if (*start++ != ')')
        return ieerr(text, var1, 0, "Missing ')'", towarn);
    *text = start;

    var1 = calloc(sizeof(VARINFO), 1);
    if (!var1)
        return 0;
    var1->type = tp;
    if (hasUDT)
    {
        if (!indir)
            return ieerr(text, var1, 0, "struct/union cast must have pointer", towarn);
        indir--;
        DeclType(*dbg, var1);
    }
    while (indir)
    {
        VARINFO* var2 = calloc(sizeof(VARINFO), 1);
        if (!var2)
        {
            FreeVarInfo(var1);
            return 0;
        }
        var2->subtype = var1;
        var2->pointer = TRUE;
        var1 = var2;
        indir--;
    }
    return var1;
}
static VARINFO* sizeofop(char** text, DEBUG_INFO** dbg, SCOPE* sc, int towarn)
{
    return ieerr(text, 0, 0, "sizeof not implemented", towarn);
}
static VARINFO** LookupStructMember(DEBUG_INFO** dbg, VARINFO** var3, char* name)
{
    VARINFO** varin = var3;
    while (*var3 && strcmp(name, (*var3)->membername) != 0)
    {
        var3 = &(*var3)->link;
    }
    if (*var3)
        return var3;
    var3 = varin;
    while (*var3)
    {
        if ((*var3)->structure)
        {
            // by convention if the member name is the same as the structure tag
            // it is a base class instance, so recurse into it looking for a match
            if (strcmp((*var3)->membername, (*var3)->structtag + 1) == 0)
            {
                VARINFO** var4 = LookupStructMember(dbg, &(*var3)->subtype, name);
                if (*var4)
                    return var4;
            }
        }
        var3 = &(*var3)->link;
    }
    return var3;
}
static VARINFO* ieprimary(char** text, DEBUG_INFO** dbg, SCOPE* sc, int towarn)
/*
 * PRimary integer
 *    id
 *    iconst
 *    (cast )intexpr
 *    (intexpr)
 *    intexpr.id
 *    intexpr->id
 */
{
    VARINFO *var1 = 0, *var2, **var3;
    char buf[256], *p;
    int needclose;
    skipspace(text);
    if (isdigit(**text) || **text == '.')
        var1 = constnode(text, towarn);
    else if (isalpha(**text) || **text == '_' || **text == ':')
    {
        char* p = *text;
        var1 = lookupsym(text, dbg, sc, towarn);
        if (!var1)
        {
            *text = p;
            var1 = regnode(text);
            if (!var1)
            {
                return ieerr(text, 0, 0, "Undefined symbol", towarn);
            }
        }
    }
    else if (!memcmp(*text, "sizeof", 6))
        return sizeofop(text, dbg, sc, towarn);
    else if (**text == '(')
    {
        var1 = castnode(text, dbg, sc, towarn);
        if (var1)
        {
            PUSH(var1);
            var2 = ieprimary(text, dbg, sc, towarn);
            if (!var2)
                return 0;
            var1 = POP();
            if (var2->constant)
            {
                var1->address = var2->ival;
                var1->derefaddress = var1->address;
                var1->ival = var2->ival;
                var1->fval = var2->fval;
            }
            else
            {
                var1->address = var2->address;
                var1->derefaddress = var1->address;
            }
            FreeVarInfo(var2);
        }
        else
        {
            (*text)++;
            var1 = iecondop(text, dbg, sc, towarn);
            if (**text != ')')
                return ieerr(text, var1, 0, "Missing ')'", towarn);
            (*text)++;
        }
    }
    skipspace(text);
    if (!var1)
        return var1;
    do
    {
        while (**text == '.' || (**text == '-' && *(*text + 1) == '>'))
        {
            int address;
            if (**text == '.')
            {
                if (!var1->structure && !var1->unionx)
                    return ieerr(text, var1, 0, "Structure or union type expected", towarn);
                if (var1->pointer)
                    return ieerr(text, var1, 0, "Address of structure expected", towarn);
                (*text)++;
                address = var1->address;
            }
            else
            {
                if (!var1->pointer)
                    return ieerr(text, var1, 0, "Pointer type expected", towarn);
                (*text) += 2;
                ExpandPointerInfo(*dbg, var1);
                var2 = var1->subtype;
                var1->subtype = 0;
                if (var1->derefaddress != -1)
                    var2->address = var1->derefaddress;
                else
                    ReadValue(var1->address, &var2->address, 4, var1);
                FreeVarInfo(var1);
                var1 = var2;
                if (!var1->structure && !var1->unionx)
                    return ieerr(text, var1, 0, "Structure or union type expected", towarn);
            }
            skipspace(text);
            if (!isalpha(**text) && **text != '_')
                return ieerr(text, var1, 0, "Identifier Expected", towarn);
            p = buf;
            while (isalnum(**text) || **text == '_')
                *p++ = *(*text)++;
            *p = 0;
            var3 = LookupStructMember(dbg, &var1->subtype, buf);
            var2 = *var3;
            if (!var2)
                return ieerr(text, var1, 0, "Unknown member name", towarn);
            (*var3) = var2->link;
            if (var2->offset == -1)
            {
                DEBUG_INFO* dbg;
                int i;
                char name[2048];
                sprintf(name, "%s@%s", var1->structtag, var2->membername);
                dbg = findDebug(var1->address);
                var2->address = GetSymbolAddress(dbg, name);
                // static member data
            }
            else
            {
                var2->address = var1->address + var2->offset;
            }
            var2->offset = 0;
            FreeVarInfo(var1);
            var1 = var2;
            var1->link = 0;
            skipspace(text);
        }
        while (**text == '[')
        {
            (*text)++;
            if (!var1->array && !var1->pointer)
                return ieerr(text, var1, 0, "Array expected", towarn);
            skipspace(text);
            if (!isdigit(**text))
            {
                VARINFO* vx;
                int issigned;
                vx = iecondop(text, dbg, sc, towarn);
                if (!vx)
                    return vx;
                if (!vx->constant)
                    EvalBasicType(vx, &issigned, sc);
                ival = vx->ival;
                FreeVarInfo(vx);
            }
            else
            {
                inputNum(text, towarn);
                if (lastst != IVAL && lastst != IUVAL)
                    return ieerr(text, var1, 0, "Invalid array index", towarn);
            }
            skipspace(text);
            if (**text != ']')
                return ieerr(text, var1, 0, "Missing ']'", towarn);
            (*text)++;
            skipspace(text);
            if (!var1->array)
            {
                ExpandPointerInfo(*dbg, var1);
                if (var1->derefaddress != -1)
                    var1->subtype->address = var1->derefaddress;
                else
                    ReadValue(var1->address, &var1->subtype->address, 4, var1);
                var2 = var1;
                var1 = var1->subtype;
                var1->address = var1->address + pointedsize(dbg, var1) * ival;

                var2->subtype = NULL;
                FreeVarInfo(var2);
            }
            else
            {
                ExpandPointerInfo(*dbg, var1);
                var2 = var1;
                var1 = var1->subtype;
                var1->address = var2->address + var2->itemsize * ival;
                var2->subtype = var1->link;
                var1->link = NULL;
                FreeVarInfo(var2);
            }
        }
    } while (**text == '.' || (**text == '-' && *(*text + 1) == '>'));
    return var1;
}
/*
 * Integer unary
 *   - unary
 *   ! unary
 *   ~unary
 *   primary
 */
static VARINFO* ieunary(char** text, DEBUG_INFO** dbg, SCOPE* sc, int towarn)
{
    VARINFO *val1, *val2;
    switch (**text)
    {
        case '-':
            if (*(*text + 1) != '>')
            {
                (*text)++;
                val1 = ieunary(text, dbg, sc, towarn);
                val1 = makeconst(val1, sc);
                if (val1->type >= eFloat && val1->type <= eImaginaryLongDouble)
                    val1->fval = -val1->fval;
                else
                    val1->ival = -val1->ival;
                truncateconst(val1, 0);
            }
            else
                val1 = ieprimary(text, dbg, sc, towarn);
            break;
        case '!':
            (*text)++;
            val1 = ieunary(text, dbg, sc, towarn);
            val1 = makeconst(val1, sc);
            if (val1->type >= eFloat && val1->type <= eImaginaryLongDouble)
                val1->fval = !val1->fval;
            else
                val1->ival = !val1->ival;
            break;
        case '~':
            (*text)++;
            val1 = ieunary(text, dbg, sc, towarn);
            val1 = makeconst(val1, sc);
            if (val1->type >= eFloat && val1->type <= eImaginaryLongDouble)
                return ieerr(text, 0, 0, "Invalid Floating Operation", towarn);
            else
                val1->ival = ~val1->ival;
            truncateconst(val1, 0);
            break;
        case '+':
            (*text)++;
            val1 = ieunary(text, dbg, sc, towarn);
            val1 = makeconst(val1, sc);
            break;
        case '*':
            (*text)++;
            val1 = ieunary(text, dbg, sc, towarn);
            if (!val1 || !val1->pointer)
                return ieerr(text, val1, 0, "Pointer Type Expected", towarn);
            if (val1->type <= eULongLong || (val1->type >= eFloat && val1->type <= eComplexLongDouble))
            {
                //                    val1->type &= ~CV_MMASK;
                val1->pointer = FALSE;
                if (val1->derefaddress != -1)
                    val1->address = val1->derefaddress;
                else
                    ReadValue(val1->address, &val1->address, 4, val1);
            }
            else
            {
                ExpandPointerInfo(*dbg, val1);
                val2 = val1->subtype;
                if (val1->derefaddress != -1)
                    val2->address = val1->derefaddress;
                else
                    ReadValue(val1->address, &val2->address, 4, val1);
                val1->subtype = 0;
                FreeVarInfo(val1);
                val1 = val2;
            }
            break;
        case '&':
            (*text)++;
            val1 = ieunary(text, dbg, sc, towarn);
            if (val1)
            {
                val1 = makeconst(val1, sc);
                val1->type = eVoid;  // using eVoid as a placeholder for addresses
            }
            break;
        default:
            val1 = ieprimary(text, dbg, sc, towarn);
            break;
    }
    return (val1);
}
static VARINFO* iemultops(char** text, DEBUG_INFO** dbg, SCOPE* sc, int towarn)
/* Multiply ops */
{
    VARINFO *val1, *val2;
    PUSH(val1 = ieunary(text, dbg, sc, towarn));
    skipspace(text);
    while (val1 && (*(*text) == '*' || *(*text) == '%' || *(*text) == '\\'))
    {
        int ch = *(*text)++;
        val2 = ieunary(text, dbg, sc, towarn);
        if (!val2)
            return 0;
        val1 = makeconst(POP(), sc);
        val2 = makeconst(val2, sc);
        switch (ch)
        {
            case '*':
                if (val1->type >= eFloat && val1->type <= eImaginaryLongDouble)
                    if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                        val1->fval = val1->fval * val2->fval;
                    else
                        val1->fval = val1->fval * val2->ival;
                else if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                    val1->fval = val1->ival * val2->fval;
                else
                    val1->ival = val1->ival * val2->ival;
                break;
            case '/':
                if (val1->type >= eFloat && val1->type <= eImaginaryLongDouble)
                    if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                        val1->fval = val1->fval / val2->fval;
                    else
                        val1->fval = val1->fval / val2->ival;
                else if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                    val1->fval = val1->ival / val2->fval;
                else
                    val1->ival = val1->ival / val2->ival;
                break;
            case '%':
                if ((val1->type >= eFloat && val1->type <= eImaginaryLongDouble) ||
                    (val2->type >= eFloat && val2->type <= eImaginaryLongDouble))
                    return ieerr(text, 0, 0, "Invalid Floating Operation", towarn);
                val1->ival = val1->ival % val2->ival;
                break;
        }
        truncateconst(val1, val2);
        FreeVarInfo(val2);
        PUSH(val1);
        skipspace(text);
    }
    return (POP());
}
static VARINFO* ieaddops(char** text, DEBUG_INFO** dbg, SCOPE* sc, int towarn)
/* Add ops */
{

    VARINFO *val1, *val2;
    int mul1 = 1, mul2 = 1;
    PUSH(val1 = iemultops(text, dbg, sc, towarn));
    skipspace(text);
    while (val1 && (*(*text) == '+' || *(*text) == '-') && *(*text + 1) != '>')
    {
        int ch = *(*text);
        (*text)++;
        val2 = iemultops(text, dbg, sc, towarn);
        if (!val2)
            return 0;
        val1 = POP();
        if (val1->pointer || val1->array || val2->pointer || val2->array)
        {
            if (ch == '+')
            {
                if ((val1->pointer || val1->array) && (val2->pointer || val2->array))
                    return ieerr(text, val1, val2, "Too many pointers in addition", towarn);
                if (val2->pointer || val2->array)
                {
                    VARINFO* val3 = val2;
                    val2 = val1;
                    val1 = val3;
                }
                if (val1->pointer)
                {
                    if (val1->derefaddress == -1)
                        ReadValue(val1->address, &val1->derefaddress, 4, val1);
                    val2 = makeconst(val2, sc);
                    val1->derefaddress += val2->ival * pointedsize(dbg, val1);
                }
                else
                {
                    val2 = makeconst(val2, sc);
                    val1->address += val2->ival * val1->arraysize;
                }
            }
            else
            {
                int size;
                if (!val1->pointer && !val1->array)
                    return ieerr(text, val1, val2, "Invalid pointer subtraction", towarn);
                if (val1->pointer)
                {
                    if (val1->derefaddress == -1)
                        ReadValue(val1->address, &val1->derefaddress, 4, val1);
                    size = pointedsize(dbg, val1);
                }
                else
                    size = val1->itemsize;
                if (val2->pointer)
                    if (val2->derefaddress == -1)
                        ReadValue(val2->address, &val2->derefaddress, 4, val2);
                if (val2->pointer)
                {
                    unsigned v;
                    if (val1->pointer)
                    {
                        v = (val1->derefaddress - val2->derefaddress) / size;
                    }
                    else
                    {
                        v = (val1->address - val2->derefaddress) / size;
                    }
                    val1 = makeconst(val1, sc);
                    val1->ival = v;
                }
                else if (val2->array)
                {
                    unsigned v;
                    if (val1->pointer)
                    {
                        v = (val1->derefaddress - val2->address) / size;
                    }
                    else
                    {
                        v = (val1->address - val2->address) / size;
                    }
                    val1 = makeconst(val1, sc);
                    val1->ival = v;
                }
                else
                {
                    if (val1->pointer)
                    {
                        val2 = makeconst(val2, sc);
                        val1->derefaddress -= val2->ival * pointedsize(dbg, val1);
                    }
                    else
                    {
                        val2 = makeconst(val2, sc);
                        val1->address -= val2->ival * val1->itemsize;
                    }
                }
            }
        }
        else
        {
            // join:
            if (val1->structure || val1->unionx)
                mul1 = val1->arraysize;
            else if (val2->structure || val2->unionx)
                mul2 = val2->arraysize;
            else if (val1->array)
            {
                mul1 = val1->itemsize;
            }
            else if (val2->array)
            {
                mul2 = val2->itemsize;
            }
            val1 = makeconst(val1, sc);
            val2 = makeconst(val2, sc);
            if (ch == '+')
            {
                if (val1->type >= eFloat && val1->type <= eImaginaryLongDouble)
                {
                    if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                        val1->fval = val1->fval * mul2 + val2->fval * mul1;
                    else
                        val1->fval = val1->fval * mul2 + val2->ival * mul1;
                }
                else
                {
                    if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                        val1->fval = val1->ival * mul2 + val2->fval * mul1;
                    else
                        val1->ival = val1->ival * mul2 + val2->ival * mul1;
                }
            }
            else
            {
                if (val1->type >= eFloat && val1->type <= eImaginaryLongDouble)
                {
                    if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                        val1->fval = val1->fval * mul2 - val2->fval * mul1;
                    else
                        val1->fval = val1->fval * mul2 - val2->ival * mul1;
                }
                else
                {
                    if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                        val1->fval = val1->ival * mul2 - val2->fval * mul1;
                    else
                        val1->ival = val1->ival * mul2 - val2->ival * mul1;
                }
            }
            truncateconst(val1, val2);
        }
        FreeVarInfo(val2);
        PUSH(val1);
        skipspace(text);
    }
    return POP();
}
static VARINFO* ieshiftops(char** text, DEBUG_INFO** dbg, SCOPE* sc, int towarn)
/* Shift ops */
{
    VARINFO *val1, *val2;
    PUSH(val1 = ieaddops(text, dbg, sc, towarn));
    skipspace(text);
    while (val1 && ((*(*text) == '<' || *(*text) == '>') && *(*text) == *(*text + 1)))
    {
        long oper = *(*text) == '<';
        (*text) += 2;
        val2 = ieaddops(text, dbg, sc, towarn);
        if (!val2)
            return 0;
        val1 = POP();
        val1 = makeconst(POP(), sc);
        val2 = makeconst(val2, sc);
        if ((val1->type >= eFloat && val1->type <= eImaginaryLongDouble) ||
            (val2->type >= eFloat && val2->type <= eImaginaryLongDouble))
            return ieerr(text, val1, val2, "Invalid floating operation", towarn);
        if (oper)
        {
            val1->ival = val1->ival << val2->ival;
        }
        else
            val1->ival = val1->ival >> val2->ival;
        truncateconst(val1, val2);
        FreeVarInfo(val2);
        PUSH(val1);
        skipspace(text);
    }
    return POP();
}
static VARINFO* ierelation(char** text, DEBUG_INFO** dbg, SCOPE* sc, int towarn)
/* non-eq relations */
{
    VARINFO *val1, *val2;
    PUSH(val1 = ieshiftops(text, dbg, sc, towarn));
    skipspace(text);
    while (val1 && ((*(*text) == '<' || *(*text) == '>') && (*(*text + 1) != *(*text))))
    {
        int oper = *(*text) == '<';
        (*text)++;
        if (**text == '=')
        {
            (*text)++;
            oper |= 2;
        }
        val2 = ieshiftops(text, dbg, sc, towarn);
        if (!val2)
            return 0;
        val1 = makeconst(POP(), sc);
        val2 = makeconst(val2, sc);
        switch (oper)
        {
            case 0:
                if (val1->type >= eFloat && val1->type <= eImaginaryLongDouble)
                    if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                        val1->ival = val1->fval < val2->fval;
                    else
                        val1->ival = val1->fval < val2->ival;
                else if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                    val1->ival = val1->ival < val2->fval;
                else
                    val1->ival = val1->ival < val2->ival;
                break;
            case 1:
                if (val1->type >= eFloat && val1->type <= eImaginaryLongDouble)
                    if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                        val1->ival = val1->fval > val2->fval;
                    else
                        val1->ival = val1->fval > val2->ival;
                else if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                    val1->ival = val1->ival > val2->fval;
                else
                    val1->ival = val1->ival > val2->ival;
                break;
            case 2:
                if (val1->type >= eFloat && val1->type <= eImaginaryLongDouble)
                    if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                        val1->ival = val1->fval <= val2->fval;
                    else
                        val1->ival = val1->fval <= val2->ival;
                else if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                    val1->ival = val1->ival <= val2->fval;
                else
                    val1->ival = val1->ival <= val2->ival;
                break;
            case 3:
                if (val1->type >= eFloat && val1->type <= eImaginaryLongDouble)
                    if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                        val1->ival = val1->fval >= val2->fval;
                    else
                        val1->ival = val1->fval >= val2->ival;
                else if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                    val1->ival = val1->ival >= val2->fval;
                else
                    val1->ival = val1->ival >= val2->ival;
                break;
        }
        val1->type = eUInt;
        FreeVarInfo(val2);
        PUSH(val1);
        skipspace(text);
    }
    return POP();
}
static VARINFO* ieequalops(char** text, DEBUG_INFO** dbg, SCOPE* sc, int towarn)
/* eq relations */
{
    VARINFO *val1, *val2;
    PUSH(val1 = ierelation(text, dbg, sc, towarn));
    skipspace(text);
    while (val1 && (*(*text + 1) == '=' && (*(*text) == '=' || *(*text) == '!')))
    {
        int ch = *(*text);
        *(*text) += 2;

        val2 = ierelation(text, dbg, sc, towarn);
        if (!val2)
            return 0;
        val1 = makeconst(POP(), sc);
        val2 = makeconst(val2, sc);
        if (ch == '!')
        {
            if (val1->type >= eFloat && val1->type <= eImaginaryLongDouble)
            {
                if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                    val1->ival = val1->fval != val2->fval;
                else
                    val1->ival = val1->fval != val2->ival;
            }
            else
            {
                if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                    val1->ival = val1->ival != val2->fval;
                else
                    val1->ival = val1->ival != val2->ival;
            }
        }
        else
        {
            if (val1->type >= eFloat && val1->type <= eImaginaryLongDouble)
                if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                    val1->ival = val1->fval == val2->fval;
                else
                    val1->ival = val1->fval == val2->ival;
            else if (val2->type >= eFloat && val2->type <= eImaginaryLongDouble)
                val1->ival = val1->ival == val2->fval;
            else
                val1->ival = val1->ival == val2->ival;
        }
        val1->type = eUInt;
        FreeVarInfo(val2);
        PUSH(val1);
        skipspace(text);
    }
    return POP();
}
static VARINFO* ieandop(char** text, DEBUG_INFO** dbg, SCOPE* sc, int towarn)
/* and op */
{
    VARINFO *val1, *val2;
    PUSH(val1 = ieequalops(text, dbg, sc, towarn));
    skipspace(text);
    while (val1 && *(*text) == '&')
    {
        (*text)++;
        val2 = ieequalops(text, dbg, sc, towarn);
        if (!val2)
            return 0;
        val1 = POP();
        val1 = makeconst(POP(), sc);
        val2 = makeconst(val2, sc);
        if ((val1->type >= eFloat && val1->type <= eImaginaryLongDouble) ||
            (val2->type >= eFloat && val2->type <= eImaginaryLongDouble))
            return ieerr(text, val1, val2, "Invalid floating operation", towarn);
        val1->ival = val1->ival & val2->ival;
        truncateconst(val1, val2);
        FreeVarInfo(val2);
        PUSH(val1);
        skipspace(text);
    }
    return POP();
}
static VARINFO* iexorop(char** text, DEBUG_INFO** dbg, SCOPE* sc, int towarn)
/* xor op */
{
    VARINFO *val1, *val2;
    PUSH(val1 = ieandop(text, dbg, sc, towarn));
    skipspace(text);
    while (val1 && *(*text) == '^')
    {
        (*text)++;
        val2 = ieandop(text, dbg, sc, towarn);
        if (!val2)
            return 0;
        val1 = POP();
        val1 = makeconst(POP(), sc);
        val2 = makeconst(val2, sc);
        if ((val1->type >= eFloat && val1->type <= eImaginaryLongDouble) ||
            (val2->type >= eFloat && val2->type <= eImaginaryLongDouble))
            return ieerr(text, val1, val2, "Invalid floating operation", towarn);
        val1->ival = val1->ival ^ val2->ival;
        truncateconst(val1, val2);
        FreeVarInfo(val2);
        PUSH(val1);
        skipspace(text);
    }
    return POP();
}
static VARINFO* ieorop(char** text, DEBUG_INFO** dbg, SCOPE* sc, int towarn)
/* or op */
{
    VARINFO *val1, *val2;
    PUSH(val1 = iexorop(text, dbg, sc, towarn));
    skipspace(text);
    while (val1 && *(*text) == '|')
    {
        (*text)++;
        val2 = iexorop(text, dbg, sc, towarn);
        if (!val2)
            return 0;
        val1 = POP();
        val1 = makeconst(POP(), sc);
        val2 = makeconst(val2, sc);
        if ((val1->type >= eFloat && val1->type <= eImaginaryLongDouble) ||
            (val2->type >= eFloat && val2->type <= eImaginaryLongDouble))
            return ieerr(text, val1, val2, "Invalid floating operation", towarn);
        val1->ival = val1->ival | val2->ival;
        truncateconst(val1, val2);
        FreeVarInfo(val2);
        PUSH(val1);
        skipspace(text);
    }
    return POP();
}
static VARINFO* ielandop(char** text, DEBUG_INFO** dbg, SCOPE* sc, int towarn)
/* logical and op */
{
    VARINFO *val1, *val2;
    PUSH(val1 = ieorop(text, dbg, sc, towarn));
    skipspace(text);
    while (val1 && (**text == '&' && *(*text + 1) == '&'))
    {
        (*text) += 2;
        val2 = ieorop(text, dbg, sc, towarn);
        if (!val2)
            return 0;
        val1 = POP();
        val1 = makeconst(POP(), sc);
        val2 = makeconst(val2, sc);
        if ((val1->type >= eFloat && val1->type <= eImaginaryLongDouble) ||
            (val2->type >= eFloat && val2->type <= eImaginaryLongDouble))
            return ieerr(text, val1, val2, "Invalid floating operation", towarn);
        val1->ival = val1->ival && val2->ival;
        truncateconst(val1, val2);
        FreeVarInfo(val2);
        PUSH(val1);
        skipspace(text);
    }
    return POP();
}
static VARINFO* ielorop(char** text, DEBUG_INFO** dbg, SCOPE* sc, int towarn)
/* logical or op */
{
    VARINFO *val1, *val2;
    PUSH(val1 = ielandop(text, dbg, sc, towarn));
    skipspace(text);
    while (val1 && (**text == '|' && *(*text + 1) == '|'))
    {
        (*text) += 2;
        val2 = ielandop(text, dbg, sc, towarn);
        if (!val2)
            return 0;
        val1 = POP();
        val1 = makeconst(POP(), sc);
        val2 = makeconst(val2, sc);
        if ((val1->type >= eFloat && val1->type <= eImaginaryLongDouble) ||
            (val2->type >= eFloat && val2->type <= eImaginaryLongDouble))
            return ieerr(text, val1, val2, "Invalid floating operation", towarn);
        val1->ival = val1->ival || val2->ival;
        truncateconst(val1, val2);
        FreeVarInfo(val2);
        PUSH(val1);
        skipspace(text);
    }
    return POP();
}
static VARINFO* iecondop(char** text, DEBUG_INFO** dbg, SCOPE* sc, int towarn)
/* Hook op */
{
    VARINFO *val1, *val2, *val3, *val4;
    PUSH(val1 = ielorop(text, dbg, sc, towarn));
    skipspace(text);
    if (val1 && **text == '?')
    {
        (*text)++;
        PUSH(iecondop(text, dbg, sc, towarn));
        skipspace(text);
        if (**text != ':')
            return ieerr(text, 0, 0, "Expected ':'", towarn);
        (*text)++;
        val3 = iecondop(text, dbg, sc, towarn);
        val2 = POP();
        val1 = POP();
        val1 = makeconst(val1, sc);
        if (val1->type >= eFloat && val1->type <= eImaginaryLongDouble)
            if (val1->fval)
                val4 = val2;
            else
                val4 = val3;
        else if (val1->ival)
            val4 = val2;
        else
            val4 = val3;
        FreeVarInfo(val1);
        if (val4 == val2)
            FreeVarInfo(val3);
        else
            FreeVarInfo(val2);
        PUSH(val4);
    }
    return POP();
}
static void SelectScope(VARINFO* var, SCOPE* sc)
{
    VARINFO* t = var;
    while (t)
    {
        SelectScope(t->subtype, sc);
        t->scope = sc;
        t = t->link;
    }
}
VARINFO* EvalExpr(DEBUG_INFO** dbg, SCOPE* sc, char* text, int towarn)
{
    VARINFO* var;
    char* p = text;
    if (!*text)
        return ieerr(&text, 0, 0, "Malformed Expression", towarn);
    varinfo_count = 0;
    var = iecondop(&text, dbg, sc, towarn);
    if (varinfo_count || *text != 0)
        return ieerr(&text, var, 0, "Malformed Expression", towarn);
    if (!var)
        return var;
    if (var->constant)
        if (!(var->type <= eULongLong || (var->type >= eFloat && var->type <= eComplexLongDouble)) || var->array)
        {
            var->constant = FALSE;
            var->address = var->ival;
        }
    strcpy(var->membername, p);
    SelectScope(var, sc);
    return var;
}
