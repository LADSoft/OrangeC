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

extern SYMBOL *theCurrentFunc;

    char *overloadNameTab[] = 
    {
        "$bctr", "$bdtr", "$bnew", "$bdel", "$badd", "$bsub", "$bmul", "$bdiv",
            "$bshl", "$bshr", "$bmod", "$bequ", "$bneq", "$blt", "$bleq", 
            "$bgt", "$bgeq", "$basn", "$basadd", "$bassub", "$basmul", 
            "$basdiv", "$basmod", "$basshl", "$bsasshr", "$basand", "$basor", 
            "$basxor", "$binc", "$bdec", "$barray", "$bcast", "$bpstar", 
            "$barrow", "$bcomma", "$blor", "$bland", "$bnot", "$bor", "$band", "$bxor", 
            "$bcpl", "$bnwa", "$bdla", "$blit"

    };
    char *overloadXlateTab[] = 
    {
        0, 0, "new", "delete", "+", "-", "*", "/", "<<", ">>", "%", "==", "!=",
            "<", "<=", ">", ">=", "=", "+=", "-=", "*=", "/=", "%=", "<<=", 
            ">>=", "&=", "|=", "^=", "++", "--", "[]", "()", "->*", "->", ",", "||",
            "&&", "!", "|", "&", "^", "~", "new[]", "delete[]", "\"\""
    };
    #define IT_THRESHOLD 2
    #define IT_OV_THRESHOLD 2
    #define IT_SIZE (sizeof(cpp_funcname_tab)/sizeof(char *))

#define MAX_MANGLE_NAME_COUNT 36

static char mangledNames[MAX_MANGLE_NAME_COUNT][256];
static int mangledNamesCount ;

static char *mangleNameSpaces(char *in, SYMBOL *sp)
{
    if (!sp)
        return in;
    in = mangleNameSpaces(in, sp->parentNameSpace);
    sprintf(in, "@%s", sp->name);
    return in + strlen(in);
}
static char *mangleClasses(char *in, SYMBOL *sp)
{
    if (!sp)
        return in;
    if (sp->parentClass)
        in = mangleClasses(in, sp->parentClass);
    else
        in = mangleNameSpaces(in, sp->parentNameSpace);
    sprintf(in, "@%s", sp->name);
    return in + strlen(in);
}
static char *getName(char *in, SYMBOL *sp)
{
    int i;
    char buf[512], *p;
    p = mangleClasses(buf, sp->parentClass);
    if (p != buf)
        *p++ = '@';
    strcpy(p, sp->name);
    for (i=0; i < mangledNamesCount; i++)
        if (!strcmp(buf, mangledNames[i]))
            break;
    if (i < mangledNamesCount)
    {
        sprintf(in, "n%c", i < 10 ? i + '0' : i - 10 + 'A');
    }
    else
    {
        if (mangledNamesCount < MAX_MANGLE_NAME_COUNT)
            strcpy(mangledNames[mangledNamesCount++], buf);
        if (isdigit(in[-1]))
            *in++ = '_';
        sprintf(in, "%d%s", strlen(buf), buf);
    }
    in += strlen(in);
    return in;
}
static char *mangleType (char *in, TYPE *tp, BOOL first)
{
    char nm[512];
    int i;
    HASHREC *hr ;
    if (tp->type == bt_typedef)
    {
       in = mangleType(in, tp->btp, FALSE);
    }
    else
    {
        if (ispointer(tp) || isfunction(tp))
        {
            if (isconst(tp))
                *in++ = 'x';
            if (isvolatile(tp))
                *in++ = 'y';
        }
        tp = basetype(tp);
        switch (tp->type)
        {
        /*
            case bt_templateplaceholder:
                tplPlaceholder(nm, tp->lst.head->name, tp->lst.tail);
                sprintf(buf, "%d%s", strlen(nm), nm);
                buf += strlen(buf);
                break;
        */
            case bt_func:
            case bt_ifunc:
                *in++ = 'q';
                hr = tp->syms->table[0];
                while (hr)
                {
                    SYMBOL *sp = (SYMBOL *)hr->p;
                    in = mangleType(in, sp->tp, TRUE);
                    hr = hr->next ;
                }
                *in++ = '$';
                break;
            case bt_memberptr:
                *in++ = 'M';
                in = getName(in, tp->sp);
                break;
            case bt_enum:
            case bt_struct:
            case bt_union:
            case bt_class:
                in = getName(in, tp->sp);
                break;
            case bt_bool:
                strcpy(in, "4bool");
                in += 5;
                break;
            case bt_unsigned_short:
                *in++ = 'u';
            case bt_short:
                *in++ = 's';
                break;
            case bt_unsigned:
                *in++ = 'u';
            case bt_int:
                *in++ = 'i';
                break;
            case bt_char16_t:
                *in++ = 'h';
                break;
            case bt_char32_t:
                *in++ = 'H';
                break;
            case bt_unsigned_long:
                *in++ = 'u';
            case bt_long:
                *in++ = 'l';
                break;
            case bt_unsigned_long_long:
                *in++ = 'u';
            case bt_long_long:
                *in++ = 'L';
                break;
            case bt_unsigned_char:
                *in++ = 'u';
            case bt_char:
                *in++ = 'c';
                break;
            case bt_wchar_t:
                *in++ = 'C';
                break;
            case bt_float_complex:
                *in++ = 'F';
                break;
            case bt_double_complex:
                *in++ = 'D';
                break;
            case bt_long_double_complex:
                *in++ = 'G';
                break;
            case bt_float:
                *in++ = 'f';
                break;
            case bt_double:
                *in++ = 'd';
                break;
            case bt_long_double:
                *in++ = 'g';
                break;
            case bt_pointer:
                if (first|| !tp->array)
                {
                    *in++ = 'p';
                }
                else
                {
                    sprintf(in,"A%d",tp->size / tp->btp->size);
                    in += strlen(in);
                }
                in = mangleType(in, tp->btp, FALSE);
                break;
            case bt_far:
                *in++ = 'P';
                in = mangleType(in, tp->btp, FALSE);
                break;
            case bt_lref:
                *in++ = 'r';
                in = mangleType(in, tp->btp, FALSE);
                break;
            case bt_rref:
                *in++ = 'R';
                in = mangleType(in, tp->btp, FALSE);
                break;
            case bt_ellipse:
                *in++ = 'e';
                break;
            case bt_void:
                *in++ = 'v';
                break;
            default:
                diag("mangleType: unknown type");
                break;
        }
    }
    *in= 0;
    return in;
}
void SetLinkerNames(SYMBOL *sym, enum e_lk linkage)
{
    char errbuf[1024], *p = errbuf;
    mangledNamesCount = 0;
    if (linkage == lk_none || linkage == lk_cdecl)
    {
        if (cparams.prm_cplusplus)
        {
            if (sym->storage_class != sc_label && sym->storage_class != sc_parameter && sym->storage_class != sc_namespace &&
                sym->storage_class != sc_namespacealias && sym->storage_class != sc_ulabel 
                && (isfunction(sym->tp) || istype(sym->storage_class) || sym->parentNameSpace || sym->parentClass))
                linkage = lk_cpp;
            else
                linkage = lk_c;
        }
        else
        {
            linkage = lk_c;
        }
    }
    if (linkage == lk_auto && !cparams.prm_cplusplus)
        linkage = lk_c;
    if (linkage == lk_c && !cparams.prm_cmangle)
        linkage = lk_stdcall;
    
    switch (linkage)
    {
        case lk_auto:
            p = mangleClasses(p, theCurrentFunc);
            sprintf(p, "@%s", sym->name);
            
            break;
        case lk_pascal:
            strcpy(errbuf, sym->name);
            while (*p)
            {
                *p = toupper(*p);
                p++;
            }
            break;
        case lk_stdcall:
            strcpy(errbuf, sym->name);
            break;
        case lk_c:
        default:
            errbuf[0] = '_';
            strcpy(errbuf+1, sym->name);
            break;
        case lk_cpp:
            p = mangleClasses(p, sym);
            if (isfunction(sym->tp))
            {
                *p++ = '$';
                p = mangleType(p, sym->tp, TRUE);
            }
            *p = 0;
            break;
    }
    sym->decoratedName = sym->errname = litlate(errbuf);
}
