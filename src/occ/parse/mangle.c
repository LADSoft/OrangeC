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
        "$bctr", "$bdtr", "$bcast", "$bnew", "$bdel", "$badd", "$bsub", "$bmul", "$bdiv",
            "$bshl", "$bshr", "$bmod", "$bequ", "$bneq", "$blt", "$bleq", 
            "$bgt", "$bgeq", "$basn", "$basadd", "$bassub", "$basmul", 
            "$basdiv", "$basmod", "$basshl", "$bsasshr", "$basand", "$basor", 
            "$basxor", "$binc", "$bdec", "$barray", "$bcall", "$bpstar", 
            "$barrow", "$bcomma", "$blor", "$bland", "$bnot", "$bor", "$band", "$bxor", 
            "$bcpl", "$bnwa", "$bdla", "$blit"

    };
    char *overloadXlateTab[] = 
    {
        0, 0, 0, "new", "delete", "+", "-", "*", "/", "<<", ">>", "%", "==", "!=",
            "<", "<=", ">", ">=", "=", "+=", "-=", "*=", "/=", "%=", "<<=", 
            ">>=", "&=", "|=", "^=", "++", "--", "[]", "()", "->*", "->", ",", "||",
            "&&", "!", "|", "&", "^", "~", "new[]", "delete[]", "\"\""
    };
    #define IT_THRESHOLD 2
    #define IT_OV_THRESHOLD 2
    #define IT_SIZE (sizeof(cpp_funcname_tab)/sizeof(char *))

#define MAX_MANGLE_NAME_COUNT 36

static char mangledNames[MAX_MANGLE_NAME_COUNT][256];
int mangledNamesCount ;

static char *mangleNameSpaces(char *in, SYMBOL *sp)
{
    if (!sp)
        return in;
    in = mangleNameSpaces(in, sp->parentNameSpace);
    sprintf(in, "@%s", sp->name);
    return in + strlen(in);
}
static char * mangleTemplate(char *buf, SYMBOL *sym, TEMPLATEPARAM *params);
static char *getName(char *in, SYMBOL *sp);
static char *mangleClasses(char *in, SYMBOL *sp)
{
    if (!sp)
        return in;
    if (sp->parentClass)
        in = mangleClasses(in, sp->parentClass);
    else
        in = mangleNameSpaces(in, sp->parentNameSpace);
    if (sp->castoperator)
        strcat(in, "@");
    else if (sp->templateLevel)
    {
        *in++ = '@';
        mangleTemplate(in, sp, sp->templateParams);
    }
    else
        sprintf(in, "@%s", sp->name);
    return in + strlen(in);
}
static char * mangleTemplate(char *buf, SYMBOL *sym, TEMPLATEPARAM *params)
{
    BOOLEAN bySpecial = FALSE;
    if (sym->instantiated && !sym->templateLevel || params && params->bySpecialization.types)
    {
        params = params->bySpecialization.types;
        bySpecial = TRUE;
    }
    *buf++ = '#';
    strcpy(buf, sym->name);
    strcat(buf, "$");
    buf += strlen(buf);
    while (params)
    {
        switch(params->type)
        {
            case kw_typename:
                if (params->packed)
                {
                    if (params->byPack.pack)
                    {
                        TEMPLATEPARAM *pack = params->byPack.pack;
                        while (pack)
                        {
                            buf = mangleType(buf, pack->byClass.val, TRUE); 
                            pack = pack->next;
                        }
                    }
                    else
                    {
                        *buf++ = 'e';
                        buf = getName(buf, params->sym);
                    }
                }
                else if (bySpecial)
                {
                    buf = mangleType(buf, params->byClass.dflt, TRUE); 
                }
                else if (sym->instantiated)
                {
                    buf = mangleType(buf, params->byClass.val, TRUE); 
                }
                else
                {
                    buf = getName(buf, params->sym);
                }
                break;
            case kw_template:
                if (params->packed)
                    *buf++ = 'e';
                if (bySpecial && params->byTemplate.dflt && params->byTemplate.val)
                {
                    buf = mangleTemplate(buf, params->byTemplate.dflt, params->byTemplate.val->templateParams);
                }
                else if (sym->instantiated && params->byTemplate.val)
                {
                    buf = mangleTemplate(buf, params->byTemplate.val, params->byTemplate.val->templateParams);
                }
                else if (params->sym)
                {
                    buf = getName(buf, params->sym);
                }
                else
                {
                    buf = getName(buf, params->byTemplate.dflt);
                }
                break;
            case kw_int:
                if (params->packed)
                    *buf++ = 'e';
                buf = mangleType(buf, params->byNonType.tp, TRUE);
                if (bySpecial || sym->instantiated)
                {
                    EXPRESSION *exp = bySpecial ? params->byNonType.dflt : params->byNonType.val;
                    if (exp)
                    {
                        *buf++= '$';
                        if (isintconst(exp))
                        {
                            sprintf(buf, "%d", exp->v.i);
                        }
                        else
                        {
                            while (lvalue(exp))
                                exp = exp->left;
                            switch (exp->type)
                            {
                                case en_func:
                                        *buf++ = 'e';
                                        *buf++ = '&';
                                        strcpy(buf, exp->v.func->sp->name);
                                        buf += strlen(buf);
                                        *buf++ = '$';
                                        buf = mangleType( buf, exp->v.func->sp->tp, TRUE);
                                        break;
                                case en_pc:
                                case en_global:
                                case en_label:
                                    if (isfunction(exp->v.sp->tp))
                                    {
                                        *buf++ = 'e';
                                        *buf++ = '&';
                                        strcpy(buf, exp->v.sp->name);
                                        buf += strlen(buf);
                                        *buf++ = '$';
                                        buf = mangleType( buf, exp->v.sp->tp, TRUE);
                                    }
                                    else
                                    {
                                        *buf++ = 'g';
                                        if (ispointer(params->byNonType.tp))
                                            *buf++ = '&';
                                        strcpy(buf, exp->v.sp->name);
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }    
                        buf += strlen(buf);                    
                        *buf++= '$';
                    }
                }
                break;
        }
        params = params->next;
    }
    *buf++ = '~';
    *buf = 0;
    return buf;
}
static char *getName(char *in, SYMBOL *sp)
{
    int i;
    char buf[512], *p;
    p = mangleClasses(buf, sp->parentClass);
    if (p != buf)
        *p++ = '@';
    if (sp->templateLevel)
    {
        p = mangleTemplate(p, sp, sp->templateParams);
    }
    else
    {
        strcpy(p, sp->name);
    }
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
char *mangleType (char *in, TYPE *tp, BOOLEAN first)
{
    char nm[512];
    int i;
    HASHREC *hr ;
    if(!tp)
    {
        sprintf(in, "%d%s", strlen("initializer-list"), "initializer-list");
        in += strlen(in);
    }
    else if (tp->type == bt_typedef)
    {
       in = mangleType(in, tp->btp, FALSE);
    }
    else
    {
        if (ispointer(tp) || isref(tp))
        {
            if (basetype(tp)->btp)
            {
                if (isconst(basetype(tp)->btp))
                    *in++ = 'x';
                if (isvolatile(basetype(tp)->btp))
                    *in++ = 'y';
            }
        }
        if (isfunction(tp))
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
                    if (!sp->thisPtr)
                        in = mangleType(in, sp->tp, TRUE);
                    hr = hr->next ;
                }
                *in++ = '$';
                // return value
                in = mangleType(in, tp->btp, TRUE);
                break;
            case bt_memberptr:
                *in++ = 'M';
                in = getName(in, tp->sp);
                if (isfunction(tp->btp))
                {
                    *in++ = 'q';
                    hr = basetype(tp->btp)->syms->table[0];
                    while (hr)
                    {
                        SYMBOL *sp = (SYMBOL *)hr->p;
                        if (!sp->thisPtr)
                            in = mangleType(in, sp->tp, TRUE);
                        hr = hr->next ;
                    }
                    *in++ = '$';
                    in = mangleType (in, tp->btp->btp, TRUE);
                }
                else
                {
                    *in++ = '$';
                    in = mangleType (in, basetype(tp)->btp, TRUE);
                }
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
            case bt_signed_char:
                *in++ = 'S';
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
            case bt_any:
                *in++ = 'v';
                break;
            case bt_templateparam:
                in = getName(in, tp->templateParam->sym);
                break;
            case bt_templateselector:
            {
                TEMPLATESELECTOR *s = tp->sp->templateSelector;
                s = s->next;
                strcpy(nm, s->sym->name);
                s = s->next ;
                while (s)
                {
                    strcat(nm , "@");
                    strcat(nm , s->name);
                    s= s->next;
                }
                sprintf(in, "%d%s", strlen(nm), nm);
                in += strlen(in);
            }
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
                && (isfunction(sym->tp) || istype(sym) || sym->parentNameSpace || sym->parentClass))
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
    if (linkage == lk_inline)
        if (cparams.prm_cplusplus)
            linkage = lk_cpp;
        else
            linkage = lk_c;
                
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
            p = mangleClasses(p, sym->parentClass);
            *p++ = '@';
            if (sym->templateLevel)
            {
                p = mangleTemplate(p, sym, sym->templateParams);
            }
            else
            {
                strcpy(p, sym->name);
                p += strlen(p);
            }
            if (isfunction(sym->tp))
            {
                *p++ = '$';
                if (sym->castoperator)
                {
                    int tmplCount = 0;
                    *p++ = 'o';
                    p = mangleType(p, basetype(sym->tp)->btp, TRUE); // cast operators get their cast type in the name
                    *p++ = '$';
                    p = mangleType(p, sym->tp, TRUE); // add the $qv
                    while (*--p != '$' || tmplCount) 
                        if (*p == '~')
                            tmplCount++;
                        else if (*p == '#')
                            tmplCount--;
                    p[1] = 0;
                }
                else
                {
                    p = mangleType(p, sym->tp, TRUE); // otherwise functions get their parameter list in the name
                    if (!sym->templateLevel)
                    {
                        int tmplCount = 0;
                        while (*--p != '$' || tmplCount) 
                            if (*p == '~')
                                tmplCount++;
                            else if (*p == '#')
                                tmplCount--;
                        p[1] = 0;
                    }
                }            
            }
            *p = 0;
            break;
    }
    sym->decoratedName = sym->errname = litlate(errbuf);
}
