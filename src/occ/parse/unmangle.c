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

extern char anonymousNameSpaceName[512];
    char *tn_void = "void";
    char *tn_bool = "bool";
    char *tn_char = "char";
    char *tn_int = "int";
    char *tn_long = "long";
    char *tn_longlong = "long long";
    char *tn_short = "short ";
    char *tn_unsigned = "unsigned ";
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
            "$basxor", "$binc", "$bdec", "$barray", "$bcall", "$bpstar", 
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

static char *unmangcpptype(char *buf, char *name);
static char *unmang1(char *buf, char *name);

#define MAX_MANGLE_NAME_COUNT 36
static int manglenamecount =  - 1;
static char manglenames[MAX_MANGLE_NAME_COUNT][512];


static char *unmang_intrins(char *buf, char *name, char *last)
{
    char cur[512],  *p = cur,  *q;
    int i;
    *p++ = *name++; // past the '$'
    while (*name != '@' &&  *name != '$' &&  *name)
        *p++ = *name++;
    *p = 0;
    if (cur[1] == 'o')
    {
        p = cur;
        strcpy(buf, "operator ");
        buf += strlen(buf);
        p = unmang1(buf, p + 2);
        buf += strlen(buf);
        if (p[0] == '$')
        {
            unmang1(buf, p +1);
            buf += strlen(buf);
        }
    }
    else
    {
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
                            name = unmang1(buf, name+1);
                            strcat(buf, "()");
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
    }
    return name;

}

/* Argument unmangling for C++ */
static char *unmangptr(char *buf , char *name)
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
        name = unmangptr(buf, name+1);
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
            name = unmangptr(buf, name);
        }
    }
    else if (*name != 'A')
    { 
        char basetp[256];
        int l;
        name = unmang1(basetp, name);
        l = strlen(basetp);
        memmove(buf + l, buf, strlen(buf)+1);
        memcpy(buf, basetp, l);
    }
    return name;
}

/* Argument unmangling for C++ */
static char *unmang1(char *buf, char *name)
{
    int v;
    int cvol = 0, cconst = 0;
    char buf1[256],  *p, buf2[256], buf3[256];
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
    start: if (isdigit(*name))
    {
        v =  *name++ - '0';
        while (isdigit(*name))
            v = v * 10+ *name++ - '0';
        if (name[0] == '#')
        {
            name++;
            while (*name &&  *name != '$' &&  *name != '#')
                *buf++ =  *name++;
            *buf = 0;
            if (*name == '$')
            {
                name++;
                name = unmang1(buf, name);
            }
            name++;
        }
        else
        {
            char *s = buf;
            while (v--)
            if (*name == '@')
            {
                *buf++ = ':';
                *buf++ = ':';
                name++;
            }
            else
                *buf++ =  *name++;
            *buf = 0;
            if (manglenamecount < MAX_MANGLE_NAME_COUNT)
                strcpy(manglenames[manglenamecount++], s);
        }
    }
    else
    switch (*name++)
    {
        case '$':
            p = buf1;
            if (*name == 't')
            {
                name = unmangcpptype(buf, name);
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
                name = unmang1(p, name);
                p += strlen(p);
                *p++ = ',';
                *p++ = ' ';
                *p = 0;
            }
            // discard the return code for now...
            p -= 2;
            *p = 0;
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
                name = unmang1(buf3, name);
            }
            buf1[0] = 0;
            if (name[0] == '$')
            {
                name = unmang1(buf1, ++name);
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
                name = unmang1(buf, name);
                buf = buf + strlen(buf);
            }
            else
            {
                name--;
                name -= cvol + cconst;
                name = unmangptr(buf, name);
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
            name = unmang1(buf, name);
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
            name = unmang1(buf, name);
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

static char *unmangcpptype(char *buf, char *name)
{
    int i;
    *buf++ = '<';
    while (*name &&  *name != '$' &&  *name != '@' &&  *name != '#')
    {
        name = unmang1(buf, name);
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
char *unmangle(char *val, char *name)
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
        if (name[0] != '@' && name[0] != '#')
        {
            strcpy(buf, name);
        }
        else
        {
            while (*name)
            {
                if (*name == '$')
                {
                    name = unmang1(buf, name+1);
                    buf += strlen(buf);
                }
                else if (*name == '@')
                {
                    name++;
                    if (*name == '$' &&( name[1] == 'b' || name[1] == 'o'))
                    {
                        name = unmang_intrins(buf, name, last);
                        buf += strlen(buf);
                    }
                    else
                    {
                        last = buf;
                        while (*name && *name != '$' && *name != '@')
                        {
                            if (*name == '#')
                                name++;
                            else
                                *buf++ = *name++;
                        }
                        if (!strncmp(anonymousNameSpaceName, last, buf- last))
                        {
                            strcpy(last,"{unique}");
                            buf = last + strlen(last);
                        }
                        if (*name == '@')
                        {
                            *buf++ = ':';
                            *buf++ = ':';
                        }
                        *buf = 0;
                    }
                }
                else
                {
                    name++;
                }
            }
        }
    return val;
}
