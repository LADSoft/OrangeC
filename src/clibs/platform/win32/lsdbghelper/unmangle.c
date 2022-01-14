/* Software License Agreement
 * 
 *     Copyright(C) 1994-2021 David Lindauer, (LADSoft)
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
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *     
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include "windows.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

char* unmangle(char* val, const char* name);


static char anonymousNameSpaceName[512] = "{anonymous}";
const char* tn_void = "void";
const char* tn_BOOL = "BOOL";
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
const char* tn_char16_t = "char16_t";
const char* tn_char32_t = "char32_t";

const char* cpp_funcname_tab[] = {
    "$bctr",   "$bdtr",    "$bcast",  "$bnew",  "$bdel",   "$badd", "$bsub", "$bmul",   "$bdiv",   "$bshl",   "$bshr",   "$bmod",
    "$bequ",   "$bneq",    "$blt",    "$bleq",  "$bgt",    "$bgeq", "$basn", "$basadd", "$bassub", "$basmul", "$basdiv", "$basmod",
    "$basshl", "$bsasshr", "$basand", "$basor", "$basxor", "$binc", "$bdec", "$barray", "$bcall",  "$bstar",  "$barrow", "$bcomma",
    "$blor",   "$bland",   "$bnot",   "$bor",   "$band",   "$bxor", "$bcpl", "$bnwa",   "$bdla",   "$blit"

};
const char* xlate_tab[] = {
    0,    0,     0,    "new", "delete", "+",  "-",  "*",  "/",   "<<",  ">>", "%",     "==",       "!=",    "<",  "<=",
    ">",  ">=",  "=",  "+=",  "-=",     "*=", "/=", "%=", "<<=", ">>=", "&=", "|=",    "^=",       "++",    "--", "[]",
    "()", "->*", "->", ",",   "||",     "&&", "!",  "|",  "&",   "^",   "~",  "new[]", "delete[]", "\"\" ",
};
#define IT_THRESHOLD 3
#define IT_OV_THRESHOLD 5
#define IT_SIZE (sizeof(cpp_funcname_tab) / sizeof(const char*))

static const char* unmangcpptype(char* buf, const char* name, const char* last);
const char* unmang1(char* buf, const char* name, const char* last, BOOL tof);
static const char* unmangTemplate(char* buf, const char* name, const char* last);

#define MAX_MANGLE_NAME_COUNT 36
static int manglenamecount = -1;
static char manglenames[MAX_MANGLE_NAME_COUNT][512];

const char* unmang_intrins(char* buf, const char* name, const char* last)
{
    char cur[4096], *p = cur;
    int i;
    *p++ = *name++;  // past the '$'
    while (*name != '@' && *name != '$' && *name)
        *p++ = *name++;
    *p = 0;
    for (i = 0; i < IT_SIZE; i++)
        if (!strcmp(cur, cpp_funcname_tab[i]))
            break;
    if (i >= IT_SIZE)
        strcpy(buf, cur);
    else
    {
        if (i < IT_THRESHOLD)
        {
            switch (i)
            {
            case 2:  // cast op
                strcpy(buf, "operator ");
                if (*name == '$')
                {
                    buf += strlen(buf);
                    name = unmang1(buf, name + 2, last, FALSE);
                }
                break;
            case 1:  // delete
                *buf++ = '~';
                // fallthrough
            case 0:  // new
            {
                const char* p = last;
                char* q = buf;
                while (*p && *p != '<' && *p != ':')
                    *q++ = *p++;
                *q = 0;
            }
            break;
            }
        }
        else
        {
            strcpy(buf, "operator ");
            strcat(buf, xlate_tab[i]);
        }
    }
    return name;
}

/* Argument unmangling for C++ */
static const char* unmangptr(char* buf, const char* name, const char* last)
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
        name = unmangptr(buf, name + 1, last);
        strcat(buf, "*");
        if (cconst)
        {
            strcat(buf, tn_const);
        }
        if (cvol)
        {
            strcat(buf, tn_volatile);
        }
        if (*name == 'A')
        {
            while (*name == 'A')
            {
                int n = 0;
                name++;
                while (isdigit(*name))
                    n = n * 10 + (*name++ & 0xf);
                sprintf(buf + strlen(buf), "[%d]", n);
            }
            name = unmangptr(buf, name, last);
        }
    }
    else if (*name != 'A')
    {
        char basetp[4096];
        int l;
        if (cconst)
        {
            strcat(buf, tn_const);
        }
        if (cvol)
        {
            strcat(buf, tn_volatile);
        }
        if (*name == '#')
        {
            name = unmangTemplate(basetp, name, last);
        }
        else
        {
            name = unmang1(basetp, name, last, FALSE);
        }
        l = strlen(basetp);
        memmove(buf + l, buf, strlen(buf) + 1);
        memcpy(buf, basetp, l);
    }
    return name;
}
char* unmangleExpression(char* dest, const char** name)
{
    if (isdigit(*(*name)) || *(*name) == '_')
    {
        int n = 0;
        BOOL minus = FALSE;
        if (*(*name) == '_')
        {
            minus = TRUE;
            (*name)++;
        }
        while (isdigit(*(*name)))
        {
            n *= 10;
            n += *(*name)++ - '0';
        }
        if (**name == '?')
        {
            (*name)++;
            if (minus)
                *dest++ = '-';
            sprintf(dest, "%d", n);
        }
        else
        {
            char buf[5000];
            memcpy(buf, *name, n);
            buf[n] = 0;
            unmangle(dest, buf);
            *name += n;
        }
        return dest + strlen(dest);
    }
    else
    {
        switch (*(*name)++)
        {
        case 'n':
            strcpy(dest, "nullptr");
            break;
        case 'p':
            dest = unmangleExpression(dest, name);
            *dest++ = '+';
            dest = unmangleExpression(dest, name);
            break;
        case 's':
            dest = unmangleExpression(dest, name);
            *dest++ = '-';
            dest = unmangleExpression(dest, name);
            break;
        case 'u':
            *dest++ = '-';
            dest = unmangleExpression(dest, name);
            break;
        case 'i':
        {
            char next = *(*name)++;
            dest = unmangleExpression(dest, name);
            if (next == 'p')
            {
                *dest++ = '+';
                *dest++ = '+';
            }
            else
            {
                *dest++ = '-';
                *dest++ = '-';
            }
            break;
        }
        case 'm':
            dest = unmangleExpression(dest, name);
            *dest++ = '*';
            dest = unmangleExpression(dest, name);
            break;
        case 'o':
            dest = unmangleExpression(dest, name);
            *dest++ = '%';
            dest = unmangleExpression(dest, name);
            break;
        case 'd':
            dest = unmangleExpression(dest, name);
            *dest++ = '/';
            dest = unmangleExpression(dest, name);
            break;
        case 'h':
        {
            char next = *(*name)++;
            dest = unmangleExpression(dest, name);
            if (next == 'l')
            {
                *dest++ = '<';
                *dest++ = '<';
            }
            else
            {
                *dest++ = '>';
                *dest++ = '>';
            }
            dest = unmangleExpression(dest, name);
            break;
        }
        case 'C':
            dest = unmangleExpression(dest, name);
            *dest++ = '?';
            dest = unmangleExpression(dest, name);
            *dest++ = ':';
            dest = unmangleExpression(dest, name);
            break;
        case 'c':
        {
            char next = *(*name)++, next1 = 0;
            if (next != 'e' && next != 'n')
            {
                next1 = *(*name)++;
            }
            dest = unmangleExpression(dest, name);
            switch (next)
            {
            case 'e':
                *dest++ = '=';
                *dest++ = '=';
                break;
            case 'n':
                *dest++ = '!';
                *dest++ = '=';
                break;
            case 'g':
                *dest++ = '>';
                if (next1 == 'e')
                    *dest++ = '=';
                break;
            case 'l':
                *dest++ = '<';
                if (next1 == 'e')
                    *dest++ = '=';
                break;
            }
            dest = unmangleExpression(dest, name);
            break;
        }
        case 'b':
        {
            char next = *(*name)++;
            if (next == 'n')
            {
                *dest++ = '~';
                dest = unmangleExpression(dest, name);
            }
            else
            {
                dest = unmangleExpression(dest, name);
                switch (next)
                {
                case 'a':
                    *dest++ = '&';
                    break;
                case 'o':
                    *dest++ = '|';
                    break;
                case 'x':
                    *dest++ = '^';
                    break;
                }
                dest = unmangleExpression(dest, name);
            }
            break;
        }
        case 'l':
        {
            char next = *(*name)++;
            if (next == 'n')
            {
                *dest++ = '!';
                dest = unmangleExpression(dest, name);
            }
            else
            {
                dest = unmangleExpression(dest, name);
                switch (next)
                {
                case 'a':
                    *dest++ = '&';
                    *dest++ = '&';
                    break;
                case 'o':
                    *dest++ = '|';
                    *dest++ = '|';
                    break;
                }
                dest = unmangleExpression(dest, name);
            }
            break;
        }
        case 't':
        {
            char next = *(*name)++;
            if (next == 's')
            {
                int v;
                if (*(*name) == '#')
                {
                    (*name) = unmangTemplate(dest, (*name), "");
                    dest += strlen(dest);
                }
                else if (isdigit(*(*name)))
                {
                    v = *(*name)++ - '0';
                    while (isdigit(*(*name)))
                        v = v * 10 + *(*name)++ - '0';
                    strncpy(dest, (*name), v);
                    (*name) += v;
                    dest += strlen(dest);
                }
                else if (*(*name) == 'n')
                {
                    (*name)++;
                    v = *(*name)++ - '0';
                    if (v > 9)
                        v -= 7;
                    strcpy(dest, manglenames[v]);
                    dest += strlen(dest);
                }
                while (*(*name) == 't')
                {
                    (*name)++;
                    *dest++ = ':';
                    *dest++ = ':';
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
                        strcpy(dest, manglenames[v]);
                        dest += strlen(dest);
                    }
                }
            }
            else
            {
                int v;
                (*name)++;
                if (isdigit(*(*name)))
                {
                    v = *(*name)++ - '0';
                    while (isdigit(*(*name)))
                        v = v * 10 + *(*name)++ - '0';
                    strncpy(dest, (*name), v);
                    (*name) += v;
                    dest += strlen(dest);
                }
                else if (*(*name) == 'n')
                {
                    (*name)++;
                    v = *(*name)++ - '0';
                    if (v > 9)
                        v -= 7;
                    strcpy(dest, manglenames[v]);
                    dest += strlen(dest);
                }
            }
            break;
        }
        case 'f':
        {
            int v;
            if (isdigit(*(*name)))
            {

                v = *(*name)++ - '0';
                while (isdigit(*(*name)))
                    v = v * 10 + *(*name)++ - '0';
                strncpy(dest, (*name), v);
                (*name) += v;
                dest += strlen(dest);
            }
            else if (*(*name) == 'n')
            {
                (*name)++;
                v = *(*name)++ - '0';
                if (v > 9)
                    v -= 7;
                strcpy(dest, manglenames[v]);
                dest += strlen(dest);
            }
            *dest++ = '(';
            while (*(*name) == 'f')
            {
                (*name)++;
                dest = unmangleExpression(dest, name);
            }
            *dest++ = ')';
            *dest = 0;
            break;
        }
        case 'e':
            if (*(*name) == '?')
            {
                *dest++ = '*';
                (*name)++;
            }
            while (*(*name) && *(*name) != '$')
                *dest++ = *(*name)++;
            if (*(*name))
            {
                char bft[4096];
                (*name)++;
                unmang1(bft, (*name), "", FALSE);
            }
            break;
        case 'g':
            if (*(*name) == '?')
            {
                *dest++ = '*';
                (*name)++;
            }
            while (*(*name) && *(*name) != '$')
                *dest++ = *(*name)++;
            if (*(*name))
            {
                (*name)++;
            }
            break;
        }
    }
    *dest = 0;
    return dest;
}
static const char* unmangTemplate(char* buf, const char* name, const char* last)
{
    if (*name == '#')
    {
        name++;
        if (*name == '$' && (name[1] == 'b' || name[1] == 'o'))
        {
            buf[0] = 0;
            name = unmang_intrins(buf, name, last);
            buf += strlen(buf);
        }
        else
        {
            while (*name && *name != '$')
                *buf++ = *name++;
        }
        *buf++ = '<';
        if (*name)
        {
            name++;
            if (*name == '~')
            {
                *buf++ = ',';
                *buf++ = ' ';
            }
            else
            {
                while (*name && *name != '~')
                {
                    if (*name == '#')
                    {
                        name = unmangTemplate(buf, name, last);
                        buf += strlen(buf);
                    }
                    else
                    {
                        char tname[10000];
                        *buf = 0;
                        memset(tname, 0, sizeof(tname));
                        tname[0] = 0;
                        if (*name == 'e')
                        {
                            strcpy(tname, "...");
                            name++;
                        }
                        if (*name != '~')
                        {
                            name = unmang1(tname + strlen(tname), name, last, FALSE);
                            if (*name == '?')
                            {
                                name++;
                                strcat(tname, "=");
                                char temp[5000];
                                memset(temp, 0, sizeof(temp));
                                unmangleExpression(temp, &name);
                                unmangle(tname + strlen(tname), temp);
                            }
                        }
                        strcpy(buf, tname);
                        buf += strlen(buf);
                    }
                    *buf++ = ',';
                    *buf++ = ' ';
                }
            }
            if (*name)
                name++;
        }
        else
        {
            *buf++ = ',';
            *buf++ = ' ';
        }
        buf[-2] = '>';
        buf[-1] = 0;
    }
    return name;
}
/* Argument unmangling for C++ */
const char* unmang1(char* buf, const char* name, const char* last, BOOL tof)
{
    buf[0] = 0;
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
            strcat(buf, tn_const);
        if (cvol)
            strcat(buf, tn_volatile);
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
            return unmang1(buf, name, last, tof);  // out of sequence, recover
        while (v > 0)
        {
            const char* newname;
            if (name[0] == '#')
            {
                newname = unmangTemplate(buf, name, last);
                v -= newname - name;
                name = newname;
                buf += strlen(buf);
            }
            else if (name[0] == '$')
            {
                if (name[1] == '$')  // in case of $$lambda
                {
                    *buf++ = *name++;
                    *buf++ = *name++;
                    *buf = 0;
                    v -= 2;
                }
                else
                {
                    name++;
                    newname = unmang1(buf, name, last, FALSE);
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
                    *buf++ = ':';
                    *buf++ = ':';
                    name++;
                }
                else
                    *buf++ = *name++;
                *buf = 0;
            }
        }
        if (manglenamecount < MAX_MANGLE_NAME_COUNT)
            strcpy(manglenames[manglenamecount++], s);
    }
    else
        switch (*name++)
        {
        case '$':
            p = buf1;
            if (*name == 't')
            {
                name = unmangcpptype(buf, name, last);
            }
            else if (isdigit(*name))
            {
                *p++ = '(';
                while (*name && *name != '$')
                {
                    *p++ = *name++;
                }
                if (*name)
                    name++;
                *p++ = ')';
                *p = 0;
            }
            else if (*name == 's')
            {
                name++;
                *p++ = '(';
                *p++ = '"';
                while (*name && *name != '$')
                {
                    *p++ = *name++;
                }
                if (*name)
                    name++;
                *p++ = '"';
                *p++ = ')';
                *p = 0;
            }
            break;
        case 'Q':
        case 'q':
            p = buf1;
            if (!tof)
            {
                *p++ = '(';
                *p++ = '*';
                *p++ = ')';
            }
            *p++ = '(';
            *p = 0;
            if (*name == 'v')
            {
                // special case for func with void argument
                *p++ = ',';
                *p++ = ' ';
                *p = 0;
                name++;
            }
            else
            {
                while (*name && *name != '$')
                {
                    if (*name == '#')
                    {
                        name = unmangTemplate(p, name, last);
                    }
                    else
                    {
                        name = unmang1(p, name, last, FALSE);
                    }
                    p += strlen(p);
                    *p++ = ',';
                    *p++ = ' ';
                    *p = 0;
                }
            }
            if (*name == '$')
                name++;
            if (p >= buf1 + 2)
                p -= 2;
            *p = 0;
            if (*name)
            {
                // discard return value
                buf2[0] = 0;
                name = unmang1(buf2, name, last, FALSE);
            }
            strcpy(buf, buf1);
            strcat(buf, ")");
            if (cconst)
                strcat(buf, tn_const);
            if (cvol)
                strcat(buf, tn_volatile);
            if (clrqual)
                strcat(buf, "& ");
            if (crrqual)
                strcat(buf, "&& ");
            buf += strlen(buf);
            break;
        case 'h':
            strcpy(buf, tn_char16_t);
            buf += strlen(buf);
            break;
        case 'H':
            strcpy(buf, tn_char32_t);
            buf += strlen(buf);
            break;
        case 'S':
            strcpy(buf, tn_signed);
            buf = buf + 7;
            if (*name++ == 'c')
                strcpy(buf, tn_char);
            break;
        case 'u':
            strcpy(buf, "unsigned ");
            buf = buf + 9;
            if (*name == 'N')
            {
                strcpy(buf, "unative");
            }
            else
                switch (*name++)
                {
                case 'i':
                    strcpy(buf, tn_int);
                    break;
                case 'l':
                    strcpy(buf, tn_long);
                    break;
                case 'L':
                    strcpy(buf, tn_longlong);
                    break;
                case 's':
                    strcpy(buf, tn_short);
                    break;
                case 'c':
                    strcpy(buf, tn_char);
                    break;
                case 'N':
                    strcpy(buf, "native int");
                    break;
                }
            break;
        case 'M':
            buf2[0] = 0;
            if (*name == '#')
            {
                name = unmangTemplate(buf2, name, last);
                p = buf2;
            }
            else if (*name == 'n')
            {
                name++;
                v = *name++ - '0';
                if (v > 9)
                    v -= 7;
                strcpy(buf2, manglenames[v]);
                p = buf2 + strlen(buf2);
            }
            else
            {
                v = *name++ - '0';
                while (isdigit(*name))
                    v = v * 10 + *name++ - '0';
                p = buf3;
                while (v--)
                {
                    if (*name == '@')
                    {
                        name++;
                        *p++ = ':';
                        *p++ = ':';
                    }
                    else
                        *p++ = *name++;
                    *p = 0;
                }
                if (manglenamecount < MAX_MANGLE_NAME_COUNT)
                    strcpy(manglenames[manglenamecount++], buf2);
                if (buf3[0] == '#')
                {
                    unmangTemplate(buf2, buf3, last);
                }
                else
                {
                    strcpy(buf2, buf3);
                }
                p = buf2 + strlen(buf2);
            }

            strcpy(p, "::*");
            buf3[0] = 0;
            if (name[0] == 'q')
            {
                name = unmang1(buf3, name, last, TRUE);
            }
            buf1[0] = 0;
            if (name[0] == '$')
            {
                name = unmang1(buf1, ++name, last, FALSE);
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
            strcpy(buf, manglenames[v]);
            break;
        case 'v':
            strcpy(buf, tn_void);
            break;
        case 'F':
            strcpy(buf, tn_floatcomplex);
            break;
        case 'D':
            strcpy(buf, tn_doublecomplex);
            break;
        case 'G':
            strcpy(buf, tn_longdoublecomplex);
            break;
        case 'f':
            strcpy(buf, tn_float);
            break;
        case 'd':
            strcpy(buf, tn_double);
            break;
        case 'g':
            strcpy(buf, tn_longdouble);
            break;
        case 'i':
            strcpy(buf, tn_int);
            break;
        case 'N':
            strcpy(buf, "native int");
            break;
        case 'l':
            strcpy(buf, tn_long);
            break;
        case 'L':
            strcpy(buf, tn_longlong);
            break;
        case 's':
            strcpy(buf, tn_short);
            break;
        case 'c':
            strcpy(buf, tn_char);
            break;
        case 'C':
            strcpy(buf, tn_wchar_t);
            break;
        case 'p':
        case 'P':
            if (*name == 'q' || *name == 'Q')
            {
                strcpy(buf, "(*");
                if (cconst)
                    strcat(buf, tn_const);
                if (cvol)
                    strcat(buf, tn_volatile);
                strcat(buf, ")");
                buf += strlen(buf);
                name = unmang1(buf, name, last, FALSE);
                if (clrqual)
                    strcat(buf, "& ");
                if (crrqual)
                    strcat(buf, "&& ");
                buf = buf + strlen(buf);
            }
            else
            {
                name--;
                name -= cvol + cconst;
                name = unmangptr(buf, name, last);
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
                strcat(buf, tn_const);
            }
            if (cvol)
            {
                strcat(buf, tn_volatile);
            }
            buf = buf + strlen(buf);
            if (*name == '#')
            {
                name = unmangTemplate(buf, name, last);
            }
            else
            {
                name = unmang1(buf, name, last, FALSE);
            }
            buf = buf + strlen(buf);
            *buf++ = '&';
            *buf++ = '&';
            *buf = 0;
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
                strcat(buf, tn_const);
            }
            if (cvol)
            {
                strcat(buf, tn_volatile);
            }
            buf = buf + strlen(buf);
            if (*name == '#')
            {
                name = unmangTemplate(buf, name, last);
            }
            else
            {
                name = unmang1(buf, name, last, FALSE);
            }
            buf = buf + strlen(buf);
            *buf++ = '&';
            *buf = 0;
            break;
        case 'e':
            strcpy(buf, tn_ellipse);
            break;
        case 'E':
            name += 3;
            strcpy(buf, "decltype(...) ");
            break;
        case '#':
            buf += strlen(buf);
            name = unmangTemplate(buf, name - 1, last);
            break;
        case 'a':
            strcpy(buf, "auto");
            break;
        }
    return name;
}

static const char* unmangcpptype(char* buf, const char* name, const char* last)
{
    *buf++ = '<';
    while (*name && *name != '$' && *name != '@' && *name != '#')
    {
        name = unmang1(buf, name, last, FALSE);
        buf = buf + strlen(buf);
        if (*name && *name != '$' && *name != '@' && *name != '#')
        {
            *buf++ = ',';
        }
        else
        {
            *buf++ = '>';
            *buf++ = ' ';
        }
    }
    if (*name && *name == '$')
        name++;
    *buf = 0;
    return name;
}

/* Name unmangling in general */
char* unmangle(char* val, const char* name)
{
    char* buf = val;
    char* last = buf;
    buf[0] = 0;
    manglenamecount = 0;
    if (!strncmp(name, "_Anonymous++", 12))
    {
        strcpy(buf, "{anonymous}");
    }
    else if (!strncmp(name, "_Unnamed++", 10))
    {
        strcpy(buf, "{unnamed}");
    }
    else if (name[0] == '_')
    {
        strcpy(buf, &name[1]);
    }
    else if (name[0] != '@')
    {
        strcpy(buf, name);
    }
    else
    {
        name++;
        if (name[0] == '@') 
            name++; // past the extra @ to denote template definitions
        last = buf;
        while (*name)
        {
            if (*name == '#')
            {
                *buf = 0;
                name = unmangTemplate(buf, name, last);
                buf += strlen(buf);
            }
            else if (*name == '$')
            {
                // discard the template params if they are there
                if (name[1] == 'b' || name[1] == 'o')
                {
                    name = unmang_intrins(buf, name, last);
                    buf += strlen(buf);
                }
                else {
                    const char* find = name + 1;
                    while (*find == 'x' || *find == 'y')
                        find++;
                    if (*find == 'q')
                    {
                        *buf = 0;
                        name = unmang1(buf, name + 1, last, TRUE);
                        buf += strlen(buf);
                    }
                    else
                    {
                        name++;
                        *buf++ = '<';
                        while (*name && *name != '$')
                        {
                            *buf = 0;
                            name = unmang1(buf, name, last, FALSE);
                            buf += strlen(buf);
                            if (*name == '?')
                            {
                                char temp[5000];
                                memset(temp, 0, sizeof(temp));
                                name++;
                                *buf++ = '=';
                                *buf = 0;
                                unmangleExpression(temp, &name);
                                unmangle(buf, temp);
                                buf += strlen(buf);
                            }
                            *buf++ = ',';
                            *buf++ = ' ';
                        }
                        buf[-2] = '>';
                        buf--;
                        *buf = 0;
                    }
                }
            }
            else if (*name == '@')
            {
                name++;
                *buf = 0;
                if (!strcmp(anonymousNameSpaceName, last))
                {
                    strcpy(last, "{unique}");
                    buf = last + strlen(last);
                }
                *buf++ = ':';
                *buf++ = ':';
                if (*name != '$' && (*name != '#' || name[1] != '$'))
                    last = buf;
            }
            else
            {
                *buf++ = *name++;
            }
        }
        *buf = 0;
    }
    return val;
}