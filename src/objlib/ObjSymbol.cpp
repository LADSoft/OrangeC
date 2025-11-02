/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

#include "ObjExpression.h"
#include "ObjSymbol.h"
#include <cctype>
#include <cstdio>
#include <cstring>
#include "Utils.h"
#ifndef _CRT_SECURE_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS
#endif

#define UNMANGLE_BUFFER_SIZE 10000

static char* unmangle(char* val, const char* name);

ObjString ObjSymbol::GetDisplayName()
{
    char val[20000]; 
    if (!strncmp(name.c_str(), "@.xt@", 5))
    {
        unmangle(val, name.c_str() + 4);
        char* p = val;
        if (isdigit(*p))
        {
            while (isdigit(*p))
                p++;
            while (*p == ':')
                p++;
        }
        return std::string(".xt:") + p;
    }
    return unmangle(val, (const char*)name.c_str());
}

#define UNMANGLE_SIZE(buf) (n - (buf - orig))
#define PUTCH(buf, ch) { if (n - (buf - orig) > 0) *buf++ = ch; }
#define PUTZERO(buf) { if (n - (buf - orig) > 0) *buf = '\0'; }

const char* tn_void = "void";
const char* tn_bool = "bool";
const char* tn_char = "char";
const char* tn_int = "int";
const char* tn_long = "long";
const char* tn_longlong = "long long";
const char* tn_short = "short ";
const char* tn_unsigned = "unsigned ";
const char* tn_signed = "signed ";
const char* tn_ellipse = "...";
const char* tn_float = "float";
const char* tn_double = "double";
const char* tn_longdouble = "long double";
const char* tn_volatile = " volatile ";
const char* tn_const = " const ";
const char* tn_class = "class ";
const char* tn_struct = "struct ";
const char* tn_union = "union ";
const char* tn_floatcomplex = "float complex";
const char* tn_doublecomplex = "double complex";
const char* tn_longdoublecomplex = "long double complex";
const char* tn_floatimaginary = "float imaginary";
const char* tn_doubleimaginary = "double imaginary";
const char* tn_longdoubleimaginary = "long double imaginary";
const char* tn_wchar_t = "wchar_t";
const char* tn_char8_t = "char8_t";
const char* tn_char16_t = "char16_t";
const char* tn_char32_t = "char32_t";

const char* cpp_funcname_tab[] = {
    ".bctr",   ".bdtr",   ".bcast",  ".bnew",   ".bdel",   ".badd",    ".bsub",   ".bmul",  ".bdiv",   ".bshl",
    ".bshr",   ".bmod",   ".bequ",   ".bneq",   ".blt",    ".bleq",    ".bgt",    ".bgeq",  ".basn",   ".basadd",
    ".bassub", ".basmul", ".basdiv", ".basmod", ".basshl", ".bsasshr", ".basand", ".basor", ".basxor", ".binc",
    ".bdec",   ".barray", ".bcall",  ".bstar",  ".barrow", ".bcomma",  ".blor",   ".bland", ".bnot",   ".bor",
    ".band",   ".bxor",   ".bcpl",   ".bnwa",   ".bdla",   ".blit",    ".badd",   ".bsub",  ".bmul",   ".band",

};
const char* xlate_tab[] = {
    0,    0,   0,    "new", "delete", "+",  "-",  "*",   "/",   "<<",    ">>",       "%",     "==", "!=", "<",  "<=", ">",
    ">=", "=", "+=", "-=",  "*=",     "/=", "%=", "<<=", ">>=", "&=",    "|=",       "^=",    "++", "--", "[]", "()", "->*",
    "->", ",", "||", "&&",  "!",      "|",  "&",  "^",   "~",   "new[]", "delete[]", "\"\" ", "+",  "-",  "*",  "&"};
#define IT_THRESHOLD 3
#define IT_OV_THRESHOLD 5
#define IT_SIZE (sizeof(cpp_funcname_tab) / sizeof(char*))

template <int n>
static const char* unmangcpptype(char (&orig)[n], char* buf, const char* name, const char* last);
template <int n>
static const char* unmang1(char (&orig)[n], char *buf, const char* name, const char* last, bool tof);
template <int n>
static char* unmang2(char(&orig)[n], char* val, const char* name);
template <int n>
static const char* unmangTemplate(char (&orig)[n], char* buf, const char* name, const char* last);

#define MAX_MANGLE_NAME_COUNT 36
static int manglenamecount = -1;
static char manglenames[MAX_MANGLE_NAME_COUNT][512];

const char* LookupIntrinsicName(const char* mangled)
{
    int i;
    for (i = 0; i < IT_SIZE; i++)
        if (!strcmp(mangled, cpp_funcname_tab[i]))
            break;
    if (i < IT_THRESHOLD)
    {
        switch (i)
        {
        case 0:
            return "constructor";
        case 1:
            return "destructor";
        case 2:
            return "cast operator";
        default:
            return "????";
        }
    }
    else if (i < IT_SIZE)
    {
        static char buf[512];
        sprintf(buf, "operator %s", xlate_tab[i]);
        return buf;
    }
    return nullptr;
}


template <int n>
static const char* unmang_intrins(char (&orig)[n], char *buf, const char* name, const char* last)
{
    char cur[4096], *p = cur;
    int i;
    PUTCH(p, *name++);  // past the '.'
    int hashlevel = 0;
    while (*name != '@' && (*name != '.' || hashlevel) && *name)
    {
        if (*name == '#')
            hashlevel++;
        else if (*name == '~')
            hashlevel--;
        PUTCH(p, *name++);
    }
    PUTZERO(p);
    *p = 0;
    for (i = 0; i < IT_SIZE; i++)
        if (!strcmp(cur, cpp_funcname_tab[i]))
            break;
    if (i >= IT_SIZE)
        Utils::StrCpy(buf, UNMANGLE_SIZE(buf), cur);
    else
    {
        if (i < IT_THRESHOLD)
        {
            switch (i)
            {
                case 2:  // cast op
                    Utils::StrCpy(buf, UNMANGLE_SIZE(buf), "operator ");
                    if (*name == '.' && *(name + 1) == 'o')
                    {
                        buf += strlen(buf);
                        name = unmang1(orig, buf, name + 2, last, false);
                    }
                    break;
                case 1:  // delete
                {
                    PUTCH(buf, '~');
                    const char* p = last;
                    char* q = buf;
                    while (*p && *p != '<' && *p != ':')
                        PUTCH(q, *p++);
                    PUTZERO(q);
                }
                break;
                case 0:  // new
                {
                    const char* p = last;
                    char* q = buf;
                    while (*p && *p != '<' && *p != ':')
                        PUTCH(q, *p++);
                    PUTZERO(q);
                }
                break;
            }
        }
        else
        {
            Utils::StrCpy(buf, UNMANGLE_SIZE(buf), "operator ");
            Utils::StrCat(buf, UNMANGLE_SIZE(buf), xlate_tab[i]);
        }
    }
    return name;
}
const char* unmang_intrins(char* val, const char* name, const char* last)
{
    char(&buf)[UNMANGLE_BUFFER_SIZE] = (char(&)[UNMANGLE_BUFFER_SIZE]) * val;
    return unmang_intrins(buf, buf, name, last);
}

/* Argument unmangling for C++ */
template <int n>
static const char* unmangptr(char (&orig)[n], char *buf, const char* name, const char* last)
{
    int cvol = 0;
    int cconst = 0;
    while (*name == 'x' || *name == 'y')
    {
        if (*name == 'y')
            cvol++;
        if (*name == 'x')
            cconst++;
        name++;
    }
    if (*name == 'p' || *name == 'P')
    {
        name = unmangptr(orig, buf, name + 1, last);
        Utils::StrCat(buf, UNMANGLE_SIZE(buf), "*");
        if (cconst)
        {
            Utils::StrCat(buf, UNMANGLE_SIZE(buf), tn_const);
        }
        if (cvol)
        {
            Utils::StrCat(buf, UNMANGLE_SIZE(buf), tn_volatile);
        }
        if (*name == 'A')
        {
            while (*name == 'A')
            {
                name++;
                Utils::StrCat(buf, UNMANGLE_SIZE(buf), "[]");
            }
            name = unmangptr(orig, buf, name, last);
        }
    }
    else if (*name != 'A')
    {
        char basetp[4096];
        int l;
        if (cconst)
        {
            Utils::StrCat(buf, UNMANGLE_SIZE(buf), tn_const);
        }
        if (cvol)
        {
            Utils::StrCat(buf, UNMANGLE_SIZE(buf), tn_volatile);
        }
        if (*name == '#')
        {
            name = unmangTemplate(orig, basetp, name, last);
        }
        else
        {
            name = unmang1(orig, basetp, name, last, false);
        }
        l = strlen(basetp);
        memmove(buf + l, buf, strlen(buf) + 1);
        memcpy(buf, basetp, l);
    }
    return name;
}
template <int n>
char* unmangleExpression(char (&orig)[n], char* dest, const char** name)
{
    if (isdigit(*(*name)) || *(*name) == '_')
    {
        int n1 = 0;
        bool minus = false;
        if (*(*name) == '_')
        {
            minus = true;
            (*name)++;
        }
        while (isdigit(*(*name)))
        {
            n1 *= 10;
            n1 += *(*name)++ - '0';
        }
        if (**name == '?')
        {
            (*name)++;
            if (minus)
                PUTCH(dest,  '-');
            sprintf(dest, "%d", n1);
        }
        else
        {
            char* buf = (char*)alloca(10000);
            memcpy(buf, *name, n1);
            buf[n1] = 0;
            unmang2(orig, dest, buf);
            *name += n1;
        }
        return dest + strlen(dest);
    }
    else
    {
        switch (*(*name)++)
        {
            case 'n':
                Utils::StrCpy(dest, UNMANGLE_SIZE(dest), "nullptr");
                break;
            case 'p':
                dest = unmangleExpression(orig, dest, name);
                PUTCH(dest,  '+');
                PUTZERO(dest);
                dest = unmangleExpression(orig, dest, name);
                PUTZERO(dest);
                break;
            case 's':
                dest = unmangleExpression(orig, dest, name);
                PUTCH(dest,  '-');
                PUTZERO(dest);
                dest = unmangleExpression(orig, dest, name);
                PUTZERO(dest);
                break;
            case 'u':
                PUTCH(dest,  '-');
                PUTZERO(dest);
                dest = unmangleExpression(orig, dest, name);
                PUTZERO(dest);
                break;
            case 'i': {
                char next = *(*name)++;
                dest = unmangleExpression(orig, dest, name);
                if (next == 'p')
                {
                    PUTCH(dest,  '+');
                    PUTCH(dest,  '+');
                }
                else
                {
                    PUTCH(dest,  '-');
                    PUTCH(dest,  '-');
                }
                PUTZERO(dest);
                break;
            }
            case 'm':
                dest = unmangleExpression(orig, dest, name);
                PUTCH(dest,  '*');
                PUTZERO(dest);
                dest = unmangleExpression(orig, dest, name);
                PUTZERO(dest);
                break;
            case 'o':
                dest = unmangleExpression(orig, dest, name);
                PUTCH(dest,  '%');
                PUTZERO(dest);
                dest = unmangleExpression(orig, dest, name);
                PUTZERO(dest);
                break;
            case 'd':
                dest = unmangleExpression(orig, dest, name);
                PUTCH(dest,  '/');
                PUTZERO(dest);
                dest = unmangleExpression(orig, dest, name);
                PUTZERO(dest);
                break;
            case 'D':
                dest = unmangleExpression(orig, dest, name);
                PUTCH(dest,  '.');
                PUTZERO(dest);
                dest = unmangleExpression(orig, dest, name);
                PUTZERO(dest);
                break;
            case 'P':
                dest = unmangleExpression(orig, dest, name);
                PUTCH(dest,  '-');
                PUTCH(dest,  '>');
                PUTZERO(dest);
                dest = unmangleExpression(orig, dest, name);
                break;
            case 'h': {
                char next = *(*name)++;
                dest = unmangleExpression(orig, dest, name);
                if (next == 'l')
                {
                    PUTCH(dest,  '<');
                    PUTCH(dest,  '<');
                }
                else
                {
                    PUTCH(dest,  '>');
                    PUTCH(dest,  '>');
                }
                PUTZERO(dest);
                dest = unmangleExpression(orig, dest, name);
                break;
            }
            case 'C':
                dest = unmangleExpression(orig, dest, name);
                PUTCH(dest,  '?');
                PUTZERO(dest);
                dest = unmangleExpression(orig, dest, name);
                PUTCH(dest,  ':');
                PUTZERO(dest);
                dest = unmangleExpression(orig, dest, name);
                break;
            case 'c': {
                char next = *(*name)++, next1 = 0;
                if (next != 'e' && next != 'n')
                {
                    next1 = *(*name)++;
                }
                dest = unmangleExpression(orig, dest, name);
                switch (next)
                {
                    case 'e':
                        PUTCH(dest,  '=');
                        PUTCH(dest,  '=');
                        PUTZERO(dest);
                        break;
                    case 'n':
                        PUTCH(dest,  '!');
                        PUTCH(dest,  '=');
                        PUTZERO(dest);
                        break;
                    case 'g':
                        PUTCH(dest,  '>');
                        if (next1 == 'e')
                            PUTCH(dest,  '=');
                        PUTZERO(dest);
                        break;
                    case 'l':
                        PUTCH(dest,  '<');
                        if (next1 == 'e')
                            PUTCH(dest,  '=');
                        PUTZERO(dest);
                        break;
                }
                dest = unmangleExpression(orig, dest, name);
                break;
            }
            case 'b': {
                char next = *(*name)++;
                if (next == 'n')
                {
                    PUTCH(dest,  '~');
                    PUTZERO(dest);
                    dest = unmangleExpression(orig, dest, name);
                }
                else
                {
                    dest = unmangleExpression(orig, dest, name);
                    switch (next)
                    {
                        case 'a':
                            PUTCH(dest,  '&');
                            break;
                        case 'o':
                            PUTCH(dest,  '|');
                            break;
                        case 'x':
                            PUTCH(dest,  '^');
                            break;
                    }
                    PUTZERO(dest);
                    dest = unmangleExpression(orig, dest, name);
                }
                break;
            }
            case 'l': {
                char next = *(*name)++;
                if (next == 'n')
                {
                    PUTCH(dest,  '!');
                    PUTZERO(dest);
                    dest = unmangleExpression(orig, dest, name);
                }
                else
                {
                    dest = unmangleExpression(orig, dest, name);
                    switch (next)
                    {
                        case 'a':
                            PUTCH(dest,  '&');
                            PUTCH(dest,  '&');
                            break;
                        case 'o':
                            PUTCH(dest,  '|');
                            PUTCH(dest,  '|');
                            break;
                    }
                    PUTZERO(dest);
                    dest = unmangleExpression(orig, dest, name);
                }
                break;
            }
            case 't': {
                char next = *(*name)++;
                if (next == 's')
                {
                    int v;
                    if (*(*name) == '#')
                    {
                        (*name) = unmangTemplate(orig, dest, (*name), "");
                        dest += strlen(dest);
                    }
                    else if (isdigit(*(*name)))
                    {
                        v = *(*name)++ - '0';
                        while (isdigit(*(*name)))
                            v = v * 10 + *(*name)++ - '0';
                        strncpy(dest, (*name), v);
                        dest[v] = 0;
                        (*name) += v;
                        dest += strlen(dest);
                    }
                    else if (*(*name) == 'n')
                    {
                        (*name)++;
                        v = *(*name)++ - '0';
                        if (v > 9)
                            v -= 7;
                        Utils::StrCpy(dest, UNMANGLE_SIZE(dest), manglenames[v]);
                        dest += strlen(dest);
                    }
                    while (*(*name) == 't')
                    {
                        (*name)++;
                        PUTCH(dest,  ':');
                        PUTCH(dest,  ':');
                        if (isdigit(*(*name)))
                        {
                            v = *(*name)++ - '0';
                            while (isdigit(*(*name)))
                                v = v * 10 + *(*name)++ - '0';
                            strncpy(dest, (*name), v);
                            dest[v] = 0;
                            (*name) += v;
                            dest += strlen(dest);
                        }
                        else if (*(*name) == 'n')
                        {
                            (*name)++;
                            v = *(*name)++ - '0';
                            if (v > 9)
                                v -= 7;
                            Utils::StrCpy(dest, UNMANGLE_SIZE(dest), manglenames[v]);
                            dest += strlen(dest);
                        }
                    }
                }
                else
                {
                    int v;
                    if (isdigit(*(*name)))
                    {
                        v = *(*name)++ - '0';
                        while (isdigit(*(*name)))
                            v = v * 10 + *(*name)++ - '0';
                        strncpy(dest, (*name), v);
                        dest[v] = 0;
                        (*name) += v;
                        dest += strlen(dest);
                    }
                    else if (*(*name) == 'n')
                    {
                        (*name)++;
                        v = *(*name)++ - '0';
                        if (v > 9)
                            v -= 7;
                        Utils::StrCpy(dest, UNMANGLE_SIZE(dest), manglenames[v]);
                        dest += strlen(dest);
                    }
                }
                break;
            }
            case 'f': {
                int v;
                if (isdigit(*(*name)))
                {

                    v = *(*name)++ - '0';
                    while (isdigit(*(*name)))
                        v = v * 10 + *(*name)++ - '0';
                    strncpy(dest, (*name), v);
                    dest[v] = 0;
                    (*name) += v;
                    dest += strlen(dest);
                }
                else if (*(*name) == 'n')
                {
                    (*name)++;
                    v = *(*name)++ - '0';
                    if (v > 9)
                        v -= 7;
                    Utils::StrCpy(dest, UNMANGLE_SIZE(dest), manglenames[v]);
                    dest += strlen(dest);
                }
                PUTCH(dest,  '(');
                while (*(*name) == 'F')
                {
                    (*name)++;
                    dest = unmangleExpression(orig, dest, name);
                }
                PUTCH(dest,  ')');
                PUTZERO(dest);
                break;
            }
            case 'e':
                if (*(*name) == '?')
                {
                    PUTCH(dest,  '*');
                    (*name)++;
                }
                while (*(*name) && *(*name) != '.')
                    PUTCH(dest,  *(*name)++);
                PUTZERO(dest);
                if (*(*name))
                {
                    char bft[4096];
                    (*name)++;
                    unmang1(orig, bft, (*name), "", false);
                }
                break;
            case 'g':
                if (*(*name) == '?')
                {
                    PUTCH(dest,  '*');
                    (*name)++;
                }
                while (*(*name) && *(*name) != '.')
                    PUTCH(dest,  *(*name)++);
                PUTZERO(dest);
                if (*(*name))
                {
                    (*name)++;
                }
                break;
            case 'v':
                break;
            case 'z': {
                Utils::StrCpy(dest, UNMANGLE_SIZE(dest), "sizeof...(");
                dest += strlen(dest);
                int v;
                (*name)++;
                if (isdigit(*(*name)))
                {
                    v = *(*name)++ - '0';
                    while (isdigit(*(*name)))
                        v = v * 10 + *(*name)++ - '0';
                    strncpy(dest, (*name), v);
                    dest[v] = 0;
                    (*name) += v;
                    dest += strlen(dest);
                }
                else if (*(*name) == 'n')
                {
                    (*name)++;
                    v = *(*name)++ - '0';
                    if (v > 9)
                        v -= 7;
                    Utils::StrCpy(dest, UNMANGLE_SIZE(dest), manglenames[v]);
                    dest += strlen(dest);
                }
                PUTCH(dest,  ')');
                PUTZERO(dest);
            }
        }
    }
    *dest = 0;
    return dest;
}
template <int n>
static const char* unmangTemplate(char (&orig)[n], char* buf, const char* name, const char* last)
{
    if (*name == '#')
    {
        name++;
        if (*name == '.' && (name[1] == 'b' || name[1] == 'o'))
        {
            buf[0] = 0;
            name = unmang_intrins(orig, buf, name, last);
            buf += strlen(buf);
        }
        else
        {
            while (*name && *name != '.')
                PUTCH(buf,  *name++);
        }
        PUTCH(buf,  '<');
        PUTZERO(buf);
        if (*name)
        {
            name++;
            if (*name == '~')
            {
                PUTCH(buf,  ',');
                PUTCH(buf,  ' ');
                PUTZERO(buf);
            }
            else
            {
                while (*name && *name != '~')
                {
                    if (*name == '#')
                    {
                        name = unmangTemplate(orig, buf, name, last);
                        buf += strlen(buf);
                    }
                    else
                    {
                        char tname[10000];
                        PUTZERO(buf);
                        memset(tname, 0, sizeof(tname));
                        tname[0] = 0;
                        if (*name == 'e')
                        {
                            Utils::StrCpy(tname, "...");
                            name++;
                        }
                        if (*name != '~')
                        {
                            name = unmang1(orig, tname + strlen(tname), name, last, false);
                            if (*name == '?')
                            {
                                name++;
                                Utils::StrCat(tname, "=");
                                char(&temp)[5000] = (char(&)[5000]) * (char*)alloca(5000);
                                temp[0] = 0;
                                unmangleExpression(temp, temp, &name);
                                unmang2(orig, tname + strlen(tname), temp);
                            }
                        }
                        Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tname);
                        buf += strlen(buf);
                    }
                    PUTCH(buf,  ',');
                    PUTCH(buf,  ' ');
                    PUTZERO(buf);
                }
            }
            if (*name)
                name++;
        }
        else
        {
            PUTCH(buf,  ',');
            PUTCH(buf,  ' ');
            PUTZERO(buf);
        }
        buf[-2] = '>';
        buf[-1] = 0;
    }
    if (*name == '.' && name[1] == 'o')
    {
        name = unmang1(orig, buf-1,name + 2, last, false);
        buf += strlen(buf);
    }
    return name;
}
const char* unmang1(char* val, const char* name, const char* last, bool tof)
{
    char(&buf)[UNMANGLE_BUFFER_SIZE] = (char(&)[UNMANGLE_BUFFER_SIZE]) * val;
    return unmang1(buf, val, name, last, tof);
}
/* Argument unmangling for C++ */
template <int n>
static const char* unmang1(char (&orig)[n], char* buf, const char* name, const char* last, bool tof)
{
    buf[0] = 0;
    if (*name == '@')
    {
       name++;
       while (*name && *name != '#' && orig + n > buf-1) PUTCH(buf, *name++);
       if (*name)
           name = unmangTemplate(orig, buf, name, last);
       return name;
    }
    int v;
    int cvol = 0, cconst = 0, clrqual = 0, crrqual = 0;
    char buf1[10000], *p, buf2[10000], buf3[1000];
    while (*name == '_')
        name++;
    while (*name == 'x' || *name == 'y')
    {
        if (*name == 'y')
            cvol++;
        if (*name == 'x')
            cconst++;
        if (*name == 'r')
            clrqual++;
        if (*name == 'R')
            crrqual++;
        name++;
    }
    if (*name != 'p' && *name != 'P' && *name != 'q')
    {
        if (cconst)
            Utils::StrCat(buf, UNMANGLE_SIZE(buf), tn_const);
        if (cvol)
            Utils::StrCat(buf, UNMANGLE_SIZE(buf), tn_volatile);
        buf += strlen(buf);
    }

    if (isdigit(*name))
    {
        buf += strlen(buf);
        char* s = buf;
        v = *name++ - '0';
        while (isdigit(*name))
            v = v * 10 + *name++ - '0';
        if (name[0] == '@')
            name++, v--;
        if (v < 0 || v > 200)
            return unmang1(orig, buf, name, last, tof);  // out of sequence, recover
        while (v > 0)
        {
            const char* newname;
            if (name[0] == '#')
            {
                newname = unmangTemplate(orig, buf, name, last);
                v -= newname - name;
                name = newname;
                buf += strlen(buf);
            }
            else if (name[0] == '.')
            {
                if (name[1] == '.')  // in case of ..lambda
                {
                    PUTCH(buf,  *name++);
                    PUTCH(buf,  *name++);
                    PUTZERO(buf);
                    v -= 2;
                }
                else
                {
                    name++;
                    newname = unmang1(orig, buf, name, last, false);
                    v -= newname - name - 1;
                    name = newname;
                    buf += strlen(buf);
                }
            }
            else
            {
                v--;
                if (*name == '@')
                {
                    PUTCH(buf,  ':');
                    PUTCH(buf,  ':');
                    name++;
                }
                else
                    PUTCH(buf,  *name++);
                PUTZERO(buf);
            }
        }
        if (manglenamecount < MAX_MANGLE_NAME_COUNT)
            Utils::StrCpy(manglenames[manglenamecount++], s);
    }
    else
        switch (*name++)
        {
            case '.':
                p = buf1;
                if (*name == 't')
                {
                    name = unmangcpptype(orig, buf, name, last);
                }
                else if (isdigit(*name))
                {
                    PUTCH(p, '(');
                    while (*name && *name != '.')
                    {
                        PUTCH(p, *name++);
                    }
                    if (*name)
                        name++;
                    PUTCH(p, ')');
                    *p = 0;
                }
                else if (*name == 's')
                {
                    name++;
                    PUTCH(p, '(');
                    PUTCH(p, '"');
                    while (*name && *name != '.')
                    {
                        PUTCH(p, *name++);
                    }
                    if (*name)
                        name++;
                    PUTCH(p, '"');
                    PUTCH(p, ')');
                    *p = 0;
                }
                PUTZERO(buf);
                break;
            case 'Q':
            case 'q':
                p = buf1;
                if (!tof)
                {
                    PUTCH(p, '(');
                    PUTCH(p, '*');
                    PUTCH(p, ')');
                }
                PUTCH(p, '(');
                PUTZERO(p);
                if (*name == 'v')
                {
                    // special case for func with void argument
                    PUTCH(p, ',');
                    PUTCH(p, ' ');
                    PUTZERO(p);
                    name++;
                }
                else
                {
                    while (*name && *name != '.')
                    {
                        if (*name == '#')
                        {
                            name = unmangTemplate(buf1, p, name, last);
                        }
                        else
                        {
                            name = unmang1(buf1, p, name, last, false);
                        }
                        p += strlen(p);
                        PUTCH(p, ',');
                        PUTCH(p, ' ');
                        PUTZERO(p);
                    }
                }
                if (*name == '.')
                    name++;
                if (p >= buf1 + 2)
                    p -= 2;
                PUTZERO(p);
                if (*name)
                {
                    // discard return value
                    buf2[0] = 0;
                    name = unmang1(buf2, buf2, name, last, false);
                }
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), buf1);
                Utils::StrCat(buf, UNMANGLE_SIZE(buf), ")");
                if (cconst)
                    Utils::StrCat(buf, UNMANGLE_SIZE(buf), tn_const);
                if (cvol)
                    Utils::StrCat(buf, UNMANGLE_SIZE(buf), tn_volatile);
                if (clrqual)
                    Utils::StrCat(buf, UNMANGLE_SIZE(buf), "& ");
                if (crrqual)
                    Utils::StrCat(buf, UNMANGLE_SIZE(buf), "&& ");
                buf += strlen(buf);
                break;
            case 'h':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_char16_t);
                buf += strlen(buf);
                break;
            case 'H':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_char32_t);
                buf += strlen(buf);
                break;
            case 'S':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_signed);
                buf = buf + 7;
                if (*name++ == 'c')
                    Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_char);
                break;
            case 'u':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), "unsigned ");
                buf = buf + 9;
                if (*name == 'B')
                {
                    Utils::StrCpy(buf, UNMANGLE_SIZE(buf), "_Bitint(");
                    const char* p = name + 1;
                    buf += strlen(buf);
                    while (isdigit(*p))
                        PUTCH(buf,  *p++);
                    PUTCH(buf,  ')');
                    PUTZERO(buf);
                }
                else if (*name == 'N')
                {
                    Utils::StrCpy(buf, UNMANGLE_SIZE(buf), "unative");
                }
                else
                    switch (*name++)
                    {
                        case 'i':
                            Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_int);
                            break;
                        case 'l':
                            Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_long);
                            break;
                        case 'L':
                            Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_longlong);
                            break;
                        case 's':
                            Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_short);
                            break;
                        case 'c':
                            Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_char);
                            break;
                        case 'N':
                            Utils::StrCpy(buf, UNMANGLE_SIZE(buf), "native int");
                            break;
                    }
                break;
            case 'B': {
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), "_Bitint(");
                const char* p = name + 1;
                buf += strlen(buf);
                while (isdigit(*p))
                    PUTCH(buf,  *p++);
                PUTCH(buf,  ')');
                PUTZERO(buf);
                break;
            }
            case 'M':
                buf2[0] = 0;
                buf3[0] = 0;
                if (*name == '#')
                {
                    name = unmangTemplate(buf2, buf2, name, last);
                    p = buf2;
                }
                else if (*name == 'n')
                {
                    name++;
                    v = *name++ - '0';
                    if (v > 9)
                        v -= 7;
                    Utils::StrCpy(buf2, manglenames[v]);
                    p = buf2 + strlen(buf2);
                }
                else
                {
                    v = *name++ - '0';
                    while (isdigit(*name))
                        v = v * 10 + *name++ - '0';
                    p = buf3;
                    PUTZERO(p);
                    while (v--)
                    {
                        if (*name == '@')
                        {
                            name++;
                            PUTCH(p, ':');
                            PUTCH(p, ':');
                            PUTZERO(p);
                        }
                        else if (*name == '#')
                        {

                            auto name1 = unmang1(p, name, last, false);
                            v -= name1 - name - 1;
                            name = name1;
                            p += strlen(p);
                        }
                        else
                            PUTCH(p, *name++);
                        PUTZERO(p);
                    }
                    if (manglenamecount < MAX_MANGLE_NAME_COUNT)
                        Utils::StrCpy(manglenames[manglenamecount++], buf2);
                    if (buf3[0] == '#')
                    {
                        unmangTemplate(buf2, buf2, buf3, last);
                    }
                    else
                    {
                        Utils::StrCpy(buf2, buf3);
                    }
                    p = buf2 + strlen(buf2);
                }

                Utils::StrCpy(p, UNMANGLE_SIZE(p), "::*");
                buf3[0] = 0;
                if (name[0] == 'q')
                {
                    name = unmang1(buf3, buf3, name, last, true);
                }
                buf1[0] = 0;
                if (name[0] == '.')
                {
                    name = unmang1(buf1, buf1, ++name, last, false);
                }
                if (buf3[0])
                {
                    sprintf(buf, "%s((%s)%s)", buf1, buf2, buf3);
                }
                else
                {
                    sprintf(buf, "%s %s", buf1, buf2);
                }
                break;
            case 'n':
                buf += strlen(buf);
                v = *name++ - '0';
                if (v > 9)
                    v -= 7;
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), manglenames[v]);
                break;
            case 'v':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_void);
                break;
            case 'F':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_floatcomplex);
                break;
            case 'D':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_doublecomplex);
                break;
            case 'G':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_longdoublecomplex);
                break;
            case 'f':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_float);
                break;
            case 'd':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_double);
                break;
            case 'g':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_longdouble);
                break;
            case 'i':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_int);
                break;
            case 'N':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), "native int");
                break;
            case 'l':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_long);
                break;
            case 'L':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_longlong);
                break;
            case 's':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_short);
                break;
            case 'c':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_char);
                break;
            case 'C':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_wchar_t);
                break;
            case 'A':
                buf = buf + strlen(buf);
                if (*name == '#')
                {
                    name = unmangTemplate(orig, buf, name, last);
                }
                else
                {
                    name = unmang1(orig, buf, name, last, false);
                }
                buf = buf + strlen(buf);
                PUTCH(buf,  '[');
                PUTCH(buf,  ']');
                PUTZERO(buf);
                break;
            case 'p':
            case 'P':
                if (*name == 'q' || *name == 'Q')
                {
                    Utils::StrCpy(buf, UNMANGLE_SIZE(buf), "(*");
                    if (cconst)
                        Utils::StrCat(buf, UNMANGLE_SIZE(buf), tn_const);
                    if (cvol)
                        Utils::StrCat(buf, UNMANGLE_SIZE(buf), tn_volatile);
                    Utils::StrCat(buf, UNMANGLE_SIZE(buf), ")");
                    buf += strlen(buf);
                    name = unmang1(orig, buf, name, last, false);
                    if (clrqual)
                        Utils::StrCat(buf, UNMANGLE_SIZE(buf), "& ");
                    if (crrqual)
                        Utils::StrCat(buf, UNMANGLE_SIZE(buf), "&& ");
                    buf = buf + strlen(buf);
                }
                else
                {
                    name--;
                    name -= cvol + cconst;
                    name = unmangptr(orig, buf, name, last);
                    buf = buf + strlen(buf);
                }
                break;
            case 'R':
                while (*name == 'x' || *name == 'y')
                {
                    if (*name == 'y')
                        cvol++;
                    if (*name == 'x')
                        cconst++;
                    if (*name == 'r')
                        clrqual++;
                    if (*name == 'R')
                        crrqual++;
                    name++;
                }
                if (cconst)
                {
                    Utils::StrCat(buf, UNMANGLE_SIZE(buf), tn_const);
                }
                if (cvol)
                {
                    Utils::StrCat(buf, UNMANGLE_SIZE(buf), tn_volatile);
                }
                buf = buf + strlen(buf);
                if (*name == '#')
                {
                    name = unmangTemplate(orig, buf, name, last);
                }
                else
                {
                    name = unmang1(orig, buf, name, last, false);
                }
                buf = buf + strlen(buf);
                PUTCH(buf,  '&');
                PUTCH(buf,  '&');
                PUTZERO(buf);
                break;
            case 'r':
                while (*name == 'x' || *name == 'y')
                {
                    if (*name == 'y')
                        cvol++;
                    if (*name == 'x')
                        cconst++;
                    if (*name == 'r')
                        clrqual++;
                    if (*name == 'R')
                        crrqual++;
                    name++;
                }
                if (cconst)
                {
                    Utils::StrCat(buf, UNMANGLE_SIZE(buf), tn_const);
                }
                if (cvol)
                {
                    Utils::StrCat(buf, UNMANGLE_SIZE(buf), tn_volatile);
                }
                buf = buf + strlen(buf);
                if (*name == '#')
                {
                    name = unmangTemplate(orig, buf, name, last);
                }
                else
                {
                    name = unmang1(orig, buf, name, last, false);
                }
                buf = buf + strlen(buf);
                PUTCH(buf,  '&');
                PUTZERO(buf);
                break;
            case 'e':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), tn_ellipse);
                break;
            case 'E':
                name++;  // past ?
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), "decltype(");
                buf += strlen(buf);
                buf = unmangleExpression(orig,buf, &name);
                PUTCH(buf,  ')');
                PUTZERO(buf);
                break;
            case '#':
                buf += strlen(buf);
                name = unmangTemplate(orig, buf, name - 1, last);
                break;
            case 'a':
                Utils::StrCpy(buf, UNMANGLE_SIZE(buf), "auto");
                break;
        }
    return name;
}

template <int n>
static const char* unmangcpptype(char (&orig)[n], char* buf, const char* name, const char* last)
{
    PUTCH(buf,  '<');
    PUTZERO(buf);
    while (*name && *name != '.' && *name != '@' && *name != '#')
    {
        name = unmang1(orig, buf, name, last, false);
        buf = buf + strlen(buf);
        if (*name && *name != '.' && *name != '@' && *name != '#')
        {
            PUTCH(buf,  ',');
        }
        else
        {
            PUTCH(buf,  '>');
            PUTCH(buf,  ' ');
        }
        PUTZERO(buf);
    }
    if (*name && *name == '.')
        name++;
    PUTZERO(buf);
    return name;
}
/* Name unmangling in general */
template <int n>
static char* unmang2(char (&orig)[n], char* val, const char* name)
{
    char* buf = val;
    char* last = buf;
    buf[0] = 0;
    if (!strncmp(name, "_Anonymous++", 12))
    {
        Utils::StrCpy(buf, UNMANGLE_SIZE(buf), "{anonymous}");
    }
    else if (!strncmp(name, "_Unnamed++", 10))
    {
        Utils::StrCpy(buf, UNMANGLE_SIZE(buf), "{unnamed}");
    }
    else if (name[0] == '_')
    {
        Utils::StrCpy(buf, UNMANGLE_SIZE(buf), &name[1]);
    }
    else if (name[0] != '@')
    {
        Utils::StrCpy(buf, UNMANGLE_SIZE(buf), name);
    }
    else
    {
        name++;
        if (name[0] == '@')
            name++;  // past the extra @ to denote template definitions
        last = buf;
        while (*name)
        {
            if (*name == '#')
            {
                PUTZERO(buf);
                name = unmangTemplate(orig, buf, name, last);
                buf += strlen(buf);
            }
            else if (*name == '.')
            {
                if (name[1] == '.')
                {
                    // symbol name in a lambda function
                    while (*name && *name != '@')
                        PUTCH(buf,  *name++);
                    PUTCH(buf,  ':');
                    PUTCH(buf,  ':');
                    PUTZERO(buf);
                }
                else if (name[1] == 'x' && name[2] == 'c')
                {
                    PUTCH(buf,  *name++);
                    PUTZERO(buf);
                }
                // discard the template params if they are there
                else if (name[1] == 'b' || name[1] == 'o')
                {
                    name = unmang_intrins(orig, buf, name, last);
                    buf += strlen(buf);
                }
                else
                {
                    const char* find = name + 1;
                    while (*find == 'x' || *find == 'y')
                        find++;
                    if (*find == 'q')
                    {
                        PUTZERO(buf);
                        name = unmang1(orig, buf, name + 1, last, true);
                        buf += strlen(buf);
                    }
                    else
                    {
                        name++;
                        PUTCH(buf,  '<');
                        while (*name && *name != '~')
                        {
                            PUTZERO(buf);
                            name = unmang1(orig, buf, name, last, false);
                            buf += strlen(buf);
                            if (*name == '?')
                            {
                                char(&temp)[5000] = (char(&)[5000]) * (char *)alloca(5000);
                                temp[0] = 0;
                                name++;
                                PUTCH(buf,  '=');
                                PUTZERO(buf);
                                unmangleExpression(temp, temp, &name);
                                unmang2(orig, buf, temp);
                                buf += strlen(buf);
                            }
                            if (*name && *name != '~')
                            {
                                PUTCH(buf,  ',');
                                PUTCH(buf,  ' ');
                            }
                            PUTZERO(buf);
                        }
                        buf[-2] = '>';
                        buf--;
                        PUTZERO(buf);
                    }
                }
            }
            else if (*name == '@')
            {
                name++;
                PUTZERO(buf);
                if (!strncmp(last, "_Anonymous++", 12))
                {
                    Utils::StrCpy(last, UNMANGLE_SIZE(last), "{unique}");
                    buf = last + strlen(last);
                }
                PUTCH(buf,  ':');
                PUTCH(buf,  ':');
                PUTZERO(buf);
                if (*name != '.' && (*name != '#' || name[1] != '.'))
                    last = buf;
            }
            else
            {
                PUTCH(buf,  *name++);
                PUTZERO(buf);
            }
        }
    }
    return val;
}
char* unmangle(char *val, const char* name)
{
    char(&buf)[UNMANGLE_BUFFER_SIZE] = (char(&)[UNMANGLE_BUFFER_SIZE])*val;
    manglenamecount = 0;
    return unmang2(buf, buf, name);
}
