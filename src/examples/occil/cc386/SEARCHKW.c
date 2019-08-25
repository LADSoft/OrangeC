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
/*
 * keyword module
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"

#define KWHASHSIZE 253

extern int prm_c99;
extern int global_flag;
extern int prm_ansi;
extern int asmline;
extern ASMNAME oplst[];
extern ASMREG reglst[];
extern char lastid[];
extern enum e_sym lastst;
extern int prm_cplusplus, prm_cmangle;
extern KEYWORDS prockeywords[];
extern ASMNAME *keyimage;

static HASHREC **hashtable = 0;

KEYWORDS keywords[] = 
{

    {
        0, "int", kw_int
    } , 
    {
        0, "char", kw_char
    }
    , 
    {
        0, "long", kw_long
    }
    , 
    {
        0, "float", kw_float
    }
    , 
    {
        0, "double", kw_double
    }
    , 
    {
        0, "return", kw_return
    }
    , 
    {
        0, "struct", kw_struct
    }
    , 
    {
        0, "union", kw_union
    }
    , 
    {
        0, "typedef", kw_typedef
    }
    , 
    {
        0, "enum", kw_enum
    }
    , 
    {
        0, "static", kw_static
    }
    , 
    {
        0, "auto", kw_auto
    }
    , 
    {
        0, "sizeof", kw_sizeof
    }
    , 
    {
        0, "do", kw_do
    }
    , 
    {
        0, "if", kw_if
    }
    , 
    {
        0, "else", kw_else
    }
    , 
    {
        0, "for", kw_for
    }
    , 
    {
        0, "switch", kw_switch
    }
    , 
    {
        0, "while", kw_while
    }
    , 
    {
        0, "short", kw_short
    }
    , 
    {
        0, "extern", kw_extern
    }
    , 
    {
        0, "case", kw_case
    }
    , 
    {
        0, "goto", kw_goto
    }
    , 
    {
        0, "default", kw_default
    }
    , 
    {
        0, "register", kw_register
    }
    , 
    {
        0, "unsigned", kw_unsigned
    }
    , 
    {
        0, "signed", kw_signed
    }
    , 
    {
        0, "break", kw_break
    }
    , 
    {
        0, "continue", kw_continue
    }
    , 
    {
        0, "void", kw_void
    }
    , 
    {
        0, "volatile", kw_volatile
    }
    , 
    {
        0, "const", kw_const
    }
    , 
    {
        0, "inline", kw_inline, 9
    }
    , 
    {
        0, "__inline", kw_inline
    }
    , 
    {
        0, "restrict", kw_restrict,8
    }
    , 
    {
        0, "_Bool", kw_bool, 8
    }
    , 
    {
        0, "_Complex", kw__Complex, 8
    }
    , 
    {
        0, "_Imaginary", kw__Imaginary, 8
    }
    , 
    {
        0, "__I", kw___I, 8
    }
    , 
    {
        0, "_NAN", kw__NAN
    }
    , 
    {
        0, "__func__", kw___func__, 8
    }
    , 
    {
        0, "_Pragma", kw__Pragma, 8
    }
    , 
    {
        0, "cdecl", kw__cdecl
    }
    , 
    {
        0, "typeof", kw_typeof
    }
    , 
    {
        0, "__typeid", kw___typeid
    }
    , 
        {
            0, "static_cast", kw_static_cast, 1
        }
        , 
        {
            0, "dynamic_cast", kw_dynamic_cast, 1
        }
        , 
        {
            0, "typeid", kw_typeid, 1
        }
        , 
        {
            0, "virtual", kw_virtual, 1
        }
        , 
        {
            0, "public", kw_public, 1
        }
        , 
        {
            0, "private", kw_private, 1
        }
        , 
        {
            0, "protected", kw_protected, 1
        }
        , 
        {
            0, "class", kw_class, 1
        }
        , 
        {
            0, "friend", kw_friend, 1
        }
        , 
        {
            0, "this", kw_this, 1
        }
        , 
        {
            0, "operator", kw_operator, 1
        }
        , 
        {
            0, "new", kw_new, 1
        }
        , 
        {
            0, "delete", kw_delete, 1
        }
        , 
        {
            0, "try", kw_try, 1
        }
        , 
        {
            0, "catch", kw_catch, 1
        }
        , 
        {
            0, "template", kw_template, 1
        }
        , 
        {
            0, "throw", kw_throw, 1
        }
        , 
        {
            0, "namespace", kw_namespace, 1
        }
        , 
        {
            0, "using", kw_using, 1
        }
        , 
        {
            0, "bool", kw_bool, 1
        }
        , 
        {
            0, "true", kw_true, 1
        }
        , 
        {
            0, "false", kw_false, 1
        }
        , 
        {
            0, "wchar_t", kw_wchar_t, 1
        }
        , 
        {
            0, "typename", kw_typename, 1
        }
        , 
        {
            0, "explicit", kw_explicit, 1
        }
        , 
        {
            0, "export", kw_export, 1
        }
        , 
        {
            0, "alloca", kw_alloca, 4
        }
        , 
        {
            0, "_alloca", kw_alloca, 4
        }
        , 
    {
        0, "asm", kw_asm, 4
    }
    , 
    {
        0, "_asm", kw_asm, 4
    }
    , 
    {
        0, "__int64", kw___int64, 0
    }
    ,
    {
        0, "__int16", kw_short, 0
    }
    ,
    {
        0, "__int32", kw_int, 0
    }
    ,
    {
        0, "__int8", kw_char, 0
    }
    ,
    {
        0, 0, 0
    }
};

void kwini(void)
/*
 * create a keyword hash table
 */
{
    struct kwblk *q = keywords;
    ASMNAME *r;
    ASMREG *s;
    global_flag++;
    hashtable = (HASHREC*)xalloc(KWHASHSIZE *sizeof(HASHREC*));
    memset(hashtable, 0, KWHASHSIZE *sizeof(HASHREC*));
    while (q->word)
    {
        if (!q->flags || (prm_cplusplus && (q->flags &1)) || (prm_c99 && (q
            ->flags &8)) || ((q->flags &4) && !prm_ansi))
            AddHash(q, hashtable, KWHASHSIZE);
        q++;
    } q = prockeywords;
    while (q->word)
    {
        if (!q->flags || (prm_cplusplus && (q->flags &1)) || (prm_c99 && (q
            ->flags &8)) || ((q->flags &4) && !prm_ansi))
            AddHash(q, hashtable, KWHASHSIZE);
        q++;
    }
    #ifndef ICODE
        if (!prm_ansi)
        {
            r = oplst;
            while (r->word)
            {
                if (r->atype)
                {
                    q = malloc(sizeof(struct kwblk));
                    q->next = 0;
                    q->word = r->word;
                    q->stype = kw_asminst;
                    q->flags = KW_INLINEASM;
                    q->data = r;
                    AddHash(q, hashtable, KWHASHSIZE);
                }
                r++;
            }
            s = reglst;
            while (s->word)
            {
                if (s->regtype)
                {
                    q = malloc(sizeof(struct kwblk));
                    q->next = 0;
                    q->word = s->word;
                    q->stype = kw_asmreg;
                    q->flags = KW_INLINEASM;
                    q->data = s;
                    AddHash(q, hashtable, KWHASHSIZE);
                }
                s++;
            }
        }
    #endif 
    global_flag--;
}

//-------------------------------------------------------------------------

int searchkw(void)
/*
 * see if the current symbol is a keyword
 */
{
    char *ident = lastid;
    struct kwblk **kwbp;
    if (lastst != id)
        return 0;
    if (prm_cmangle)
        ident++;
    kwbp = LookupHash(ident, hashtable, KWHASHSIZE);

    if (kwbp)
    {
        #ifndef ICODE
            if (((*kwbp)->flags == KW_INLINEASM) && !asmline)
                return 0;
            keyimage = (*kwbp)->data;
        #endif 
        return lastst = (*kwbp)->stype;
    } return (0);
}
