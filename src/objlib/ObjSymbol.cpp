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

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#include "ObjExpression.h"
#include "ObjSymbol.h"
#include <ctype.h>
#include <stdio.h>


static char *unmangle(char *val, char *name);

ObjString ObjSymbol::GetDisplayName()
{
    char val[1024];
    return unmangle(val, (char *)name.c_str());
}

static char anonymousNameSpaceName[1024];
    char *tn_void = "void";
    char *tn_bool = "bool";
    char *tn_char = "char";
    char *tn_int = "int";
    char *tn_long = "long";
    char *tn_longlong = "long long";
    char *tn_short = "short ";
    char *tn_unsigned = "unsigned ";
    char *tn_signed = "signed ";
    char *tn_ellipse = "...";
    char *tn_float = "float";
    char *tn_double = "double";
    char *tn_longdouble = "long double";
    char *tn_volatile = " volatile ";
    char *tn_const = " const ";
    char *tn_class = "class ";
    char *tn_struct = "struct ";
    char *tn_union = "union ";
    char *tn_floatcomplex = "float complex";
    char *tn_doublecomplex = "double complex";
    char *tn_longdoublecomplex = "long double complex";
    char *tn_floatimaginary = "float imaginary";
    char *tn_doubleimaginary = "double imaginary";
    char *tn_longdoubleimaginary = "long double imaginary";
    char *tn_wchar_t = "wchar_t";
    char *tn_char16_t = "char16_t";
    char *tn_char32_t = "char32_t";
    
    char *cpp_funcname_tab[] = 
    {
        "$bctr", "$bdtr", "$bcast", "$bnew", "$bdel", "$badd", "$bsub", "$bmul", "$bdiv",
            "$bshl", "$bshr", "$bmod", "$bequ", "$bneq", "$blt", "$bleq", 
            "$bgt", "$bgeq", "$basn", "$basadd", "$bassub", "$basmul", 
            "$basdiv", "$basmod", "$basshl", "$bsasshr", "$basand", "$basor", 
            "$basxor", "$binc", "$bdec", "$barray", "$bcall", "$bstar", 
            "$barrow", "$bcomma", "$blor", "$bland", "$bnot", "$bor", "$band", "$bxor", 
            "$bcpl", "$bnwa", "$bdla", "$blit"

    };
    char *xlate_tab[] = 
    {
        0, 0, 0, "new", "delete", "+", "-", "*", "/", "<<", ">>", "%", "==", "!=",
            "<", "<=", ">", ">=", "=", "+=", "-=", "*=", "/=", "%=", "<<=", 
            ">>=", "&=", "|=", "^=", "++", "--", "[]", "()", "->*", "->", ",", "||",
            "&&", "!", "|", "&", "^", "~", "new[]", "delete[]", "\"\" ",
    };
#define IT_THRESHOLD 3
#define IT_OV_THRESHOLD 5
#define IT_SIZE (sizeof(cpp_funcname_tab)/sizeof(char *))

static char *unmangcpptype(char *buf, char *name, char *last);
char *unmang1(char *buf, char *name, char *last);

#define MAX_MANGLE_NAME_COUNT 36
static int manglenamecount =  - 1;
static char manglenames[MAX_MANGLE_NAME_COUNT][512];


char *unmang_intrins(char *buf, char *name, char *last)
{
    char cur[1024],  *p = cur,  *q;
    int i;
    *p++ = *name++; // past the '$'
    while (*name != '@' &&  *name != '$' &&  *name)
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
                    case 2: // cast op
                        strcpy(buf, "operator ");
                        if (*name == '$')
                        {
                            buf += strlen(buf);
                            name = unmang1(buf, name+2, last);
                        }
                        break;
                    case 1: // delete
                        *buf++ = '~';
                        // fallthrough
                    case 0: // new
                    {
                        char *p = last,*q=buf;
                        while (*p && *p != ':')
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
static char *unmangptr(char *buf , char *name, char *last)
{
    int cvol = 0;
    int cconst = 0;
    while (*name == 'x' ||  *name == 'y')
    {
        if (*name == 'y')
            cvol++;
        if (*name == 'x')
            cconst++;
        name++;
    }
    if (*name == 'p' || *name == 'P')
    {
        name = unmangptr(buf, name+1, last);
        strcat(buf, "*");
        if (cconst)
        {
            strcat(buf, tn_const);
        }
        if (cvol)
        {
            strcat(buf , tn_volatile);
        }
        if (*name == 'A')
        {
            while (*name == 'A')
            {
                int n = 0;
                name++;
                while (isdigit (*name))
                    n = n * 10 + (*name ++ & 0xf);
                sprintf(buf + strlen(buf), "[%d]", n);
            }
            name = unmangptr(buf, name, last);
        }
    }
    else if (*name != 'A')
    { 
        char basetp[1024];
        int l;
        if (cconst)
        {
            strcat(buf, tn_const);
        }
        if (cvol)
        {
            strcat(buf , tn_volatile);
        }
        name = unmang1(basetp, name, last);
        l = strlen(basetp);
        memmove(buf + l, buf, strlen(buf)+1);
        memcpy(buf, basetp, l);
    }
    return name;
}
static char *unmangTemplate(char *buf, char *name, char *last)
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
                *buf ++ = *name ++;
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
                        char tname[1024];
                        *buf = 0;
                        tname[0] = 0;
                        if (*name == 'e')
                        {
                            strcpy(tname, "...");
                            name++;
                        }
                        name = unmang1(tname+strlen(tname), name, last);
                        if (*name == '$' && (name[1] == 'g' || name[1] == 'e' || isdigit(name[1])))
                        {
                            name++;
                            if (isdigit(*name) || *name == '_')
                            {
                                if (*name == '_')
                                {
                                    *buf++ = '-';
                                    name++;
                                }
                                while (*name && *name != '$')
                                    *buf++ = *name++;
                            }
                            else if (*name == 'e')
                            {
                                name++;
                                while (*name && *name != '$')
                                    *buf++ = *name++;
                                if (name)
                                {
                                    name = unmang1(buf, name+1, last);
                                    buf += strlen(buf);
                                }
                            }
                            else if (*name == 'g')
                            {
                                name++;
                                while (*name && *name != '$')
                                    *buf++ = *name++;
                            }
                            if (*name == '$')
                               name++;
                        }
                        else
                        {
                            strcpy(buf, tname);
                            buf += strlen(buf);
                        }
                    }
                    
                    *buf++ = ',';
                    *buf++ = ' ';
                }
            }
            if (*name) name++;
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
char *unmang1(char *buf, char *name, char *last)
{
    int v;
    int cvol = 0, cconst = 0;
    char buf1[1024],  *p, buf2[1024], buf3[1024];
    while (*name == '_')
        name++;
    while (*name == 'x' ||  *name == 'y')
    {
        if (*name == 'y')
            cvol++;
        if (*name == 'x')
            cconst++;
        name++;
    }
    if (*name != 'p' && *name != 'P')
    {
        if (cconst)
            strcat(buf, tn_const);
        if (cvol)
            strcat(buf, tn_volatile);
    }
        
    start: if (isdigit(*name))
    {
        char *s = buf;
        v =  *name++ - '0';
        while (isdigit(*name))
            v = v * 10+ *name++ - '0';
        if (name[0] == '@')
            name++, v--;            
        while (v > 0)
        {
            char *newname;
            if (name[0] == '#')
            {
                newname = unmangTemplate(buf, name, last);
                v -= newname - name;
                name = newname;
                buf += strlen(buf);
            }
            else if (name[0] == '$')
            {
                name++;
                newname = unmang1(buf, name, last);
                v -= newname - name-1;
                name = newname;
                buf += strlen(buf);
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
                    *buf++ =  *name++;
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
            *p++ = '(';
            *p = 0;
            while (*name && (*name != '$' || name[1] == 'q' || name[1] == 't'))
            {
                if (name[0] == '$'&&name[1] == 'q')
                {
                    strcpy(p, "(*)");
                    p += 3;
                }
                name = unmang1(p, name, last);
                p += strlen(p);
                *p++ = ',';
                *p++ = ' ';
                *p = 0;
            }
            if (*name == '$')
                name++;
            p -= 2;
            *p = 0;
            if (*name)
            {
                char buf2[1024];
                // discard return value
                name = unmang1(buf2, name, last);
            }            
            strcpy(buf, buf1);
            strcat(buf, ")");
            if (cconst)
                strcat(buf, tn_const);
            if (cvol)
                strcat(buf, tn_volatile);
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
            }
            break;
        case 'M':
            if (*name == 'n')
            {
                name++;
                v =  *name++ - '0';
                if (v > 9)
                    v -= 7;
                strcpy(buf2, manglenames[v]);
                p = buf2 + strlen(buf2);
            }
            else
            {
                v =  *name++ - '0';
                while (isdigit(*name))
                    v = v * 10+ *name++ - '0';
                p = buf2;
                while (v--)
                {
                    if (*name == '@')
                    {
                        name++;
                        *p++ = ':';
                        *p++ = ':';
                    }
                    else
                        *p++ =  *name++;
                    *p = 0;
                }
                if (manglenamecount < MAX_MANGLE_NAME_COUNT)
                    strcpy(manglenames[manglenamecount++], buf2);
            }

            strcpy(p, "::*");
            buf3[0] = 0;
            if (name[0] == 'q')
            {
                name = unmang1(buf3, name, last);
            }
            buf1[0] = 0;
            if (name[0] == '$')
            {
                name = unmang1(buf1, ++name, last);
            }
            if (buf3[0])
            {
                sprintf(buf, "%s (%s)%s", buf1, buf2, buf3);
            }
            else
            {
                sprintf(buf, "%s %s", buf1, buf2);
            }
            break;
        case 'n':
            v =  *name++ - '0';
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
                name = unmang1(buf, name, last);
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
            if (cconst)
            {
                strcat(buf, tn_const);
            }
            if (cvol)
            {
                strcat(buf , tn_volatile);
            }
            buf = buf + strlen(buf);
            name = unmang1(buf, name, last);
            buf = buf + strlen(buf);
            *buf++ = '&';
            *buf++ = '&';
            *buf = 0;
            break;
        case 'r':
            if (cconst)
            {
                strcat(buf, tn_const);
            }
            if (cvol)
            {
                strcat(buf , tn_volatile);
            }
            buf = buf + strlen(buf);
            name = unmang1(buf, name, last);
            buf = buf + strlen(buf);
            *buf++ = '&';
            *buf = 0;
            break;
        case 'e':
            strcpy(buf, tn_ellipse);
            break;
    }
    return name;
}

static char *unmangcpptype(char *buf, char *name, char *last)
{
    int i;
    *buf++ = '<';
    while (*name &&  *name != '$' &&  *name != '@' &&  *name != '#')
    {
        name = unmang1(buf, name, last);
        buf = buf + strlen(buf);
        if (*name &&  *name != '$' &&  *name != '@' &&  *name != '#')
        {
            *buf++ = ',';
        }
        else
        {
            *buf++ = '>';
            *buf++ = ' ';
        }
    }
    if (*name &&  *name == '$')
        name++;
    *buf = 0;
    return name;
}


/* Name unmangling in general */
static char *unmangle(char *val, char *name)
{
    char *buf = val;
    char *last = buf;
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
    else
        if (name[0] != '@')
        {
            strcpy(buf, name);
        }
        else
        {
            name++;
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
                    *buf = 0;
                    if (name[1] == 'b' || name[1] == 'o')
                    {
                        name = unmang_intrins(buf, name, last);
                    }
                    else
                    {
                        name = unmang1(buf, name+1, last);
                    }
                    buf += strlen(buf);
                }
                else if (*name == '@')
                {
                    name++;
                    *buf = 0;
                    if (!strcmp(anonymousNameSpaceName, last))
                    {
                        strcpy(last,"{unique}");
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
