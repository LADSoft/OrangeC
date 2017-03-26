/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    '
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

extern ARCH_ASM *chosenAssembler;
extern INCLUDES *includes;
extern SYMBOL *theCurrentFunc;

    char *overloadNameTab[] = 
    {
        "$bctr", "$bdtr", "$bcast", "$bnew", "$bdel", "$badd", "$bsub", "$bmul", "$bdiv",
            "$bshl", "$bshr", "$bmod", "$bequ", "$bneq", "$blt", "$bleq", 
            "$bgt", "$bgeq", "$basn", "$basadd", "$bassub", "$basmul", 
            "$basdiv", "$basmod", "$basshl", "$bsasshr", "$basand", "$basor", 
            "$basxor", "$binc", "$bdec", "$barray", "$bcall", "$bstar", 
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

static int declTypeIndex;
static char *lookupName(char *in, char *name);
char *mangleNameSpaces(char *in, SYMBOL *sp)
{
    if (!sp)
        return in;
//    if (!sp || sp->value.i > 1)
//        return in;
    in = mangleNameSpaces(in, sp->parentNameSpace);
    my_sprintf(in, "@%s", sp->name);
    return in + strlen(in);
}
static char * mangleTemplate(char *buf, SYMBOL *sym, TEMPLATEPARAMLIST *params);
static char *getName(char *in, SYMBOL *sp);
static char *mangleClasses(char *in, SYMBOL *sp)
{
    if (!sp)
        return in;
    if (sp->parentClass)
        in = mangleClasses(in, sp->parentClass);
    if (sp->castoperator)
    {
        strcat(in, "@");
    }
    else if (sp->templateLevel && sp->templateParams)
    {
        *in++ = '@';
        mangleTemplate(in, sp, sp->templateParams);
    }
    else
        my_sprintf(in, "@%s", sp->name);
    return in + strlen(in);
}
static char * mangleExpressionInternal (char *buf, EXPRESSION *exp)
{
    while (castvalue(exp))
        exp = exp->left;
	if (isintconst(exp))
	{
        if (exp->type == en_const)
        {
            my_sprintf(buf, "%lld&", exp->v.sp->value.i);
        }
        else
        {
    		my_sprintf(buf, "%lld&", exp->v.i);
        }
		if (buf[0] == '-')
			buf[0] = '_';
	}
	else
	{
        BOOLEAN nonpointer = FALSE;
		while (lvalue(exp))
        {
            nonpointer = TRUE;
			exp = exp->left;
        }
		switch (exp->type)
		{
            case en_nullptr:
                *buf++ = 'n';
				*buf = 0;
                break;
            case en_arrayadd:
            case en_structadd:
            case en_add:
                *buf++ = 'p';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_sub:
                *buf++ = 's';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_mul:
            case en_umul:
            case en_arraymul:
                *buf++ = 'm';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_umod:
            case en_mod:
                *buf++ = 'o';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_div:
            case en_udiv:
            case en_arraydiv:                
                *buf++ = 'd';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_lsh:
            case en_arraylsh:
                *buf++ = 'h';
                *buf++ = 'l';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_rsh:
            case en_ursh:
                *buf++ = 'h';
                *buf++ = 'r';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_cond:
                *buf++ = 'C';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right->left);
                buf = mangleExpressionInternal(buf, exp->right->right);
				*buf = 0;
                break;
            case en_assign:
                *buf++ = 'a';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_eq:
                *buf++ = 'c';
                *buf++ = 'e';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_ne:
                *buf++ = 'c';
                *buf++ = 'n';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_uminus:
                *buf++ = 'u';
                buf = mangleExpressionInternal(buf, exp->left);
                break;
            case en_not:
                *buf++ = 'l';
                *buf++ = 'n';
                buf = mangleExpressionInternal(buf, exp->left);
                break;
            case en_compl:
                *buf++ = 'b';
                *buf++ = 'n';
                buf = mangleExpressionInternal(buf, exp->left);
				*buf = 0;
                break;
            case en_ascompl:
                *buf++ = 'a';
                *buf++ = 'n';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_ult:
            case en_lt:
                *buf++ = 'c';
                *buf++ = 'l';
                *buf++ = 't';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_ule:
            case en_le:
                *buf++ = 'c';
                *buf++ = 'l';
                *buf++ = 'e';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_ugt:
            case en_gt:
                *buf++ = 'c';
                *buf++ = 'g';
                *buf++ = 't';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_uge:
            case en_ge:
                *buf++ = 'c';
                *buf++ = 'g';
                *buf++ = 'e';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_and:
                *buf++ = 'b';
                *buf++ = 'a';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_land:
                *buf++ = 'l';
                *buf++ = 'a';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_or:
                *buf++ = 'b';
                *buf++ = 'o';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_lor:
                *buf++ = 'l';
                *buf++ = 'o';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_xor:
                *buf++ = 'b';
                *buf++ = 'x';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_autoinc:
                *buf++ = 'i';
                *buf++ = 'p';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_autodec:
                *buf++ = 'i';
                *buf++ = 's';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
				*buf = 0;
                break;
            case en_templateselector:
            {
                TEMPLATESELECTOR *tsl = exp->v.templateSelector, *find = tsl->next->next;
                SYMBOL *ts = tsl->next->sym;
                *buf++ = 't';
                *buf++ = 's';
                if (tsl->next->isTemplate && tsl->next->templateParams) // may be an empty variadic
                {
                    buf = mangleTemplate(buf, ts, tsl->next->templateParams);
                }
                while (find)
                {
                    *buf++ = 't';
                    buf = lookupName(buf, find->name);
                    find = find->next;
                }
				*buf = 0;
                break;
            }
            case en_templateparam:
                *buf++ = 't';
                *buf++ = 'p';
                buf = lookupName(buf,exp->v.sp->name);
				*buf = 0;
                break;

            case en_funcret:
                buf = mangleExpressionInternal(buf, exp->left);
				*buf = 0;
                break;                
			case en_func:
                if (exp->v.func->ascall)
                {
                    INITLIST *args = exp->v.func->arguments;
                    *buf++ = 'f';
                    buf = lookupName(buf, exp->v.func->sp->name);
                    while(args)
                    {
                        *buf++='f';
                        buf = mangleExpressionInternal(buf, args->exp);
                        args = args->next;
                    }
                }
                else
                {
					*buf++ = 'e';
					*buf++ = '&';
					strcpy(buf, exp->v.func->sp->name);
					buf += strlen(buf);
					*buf++ = '$';
					buf = mangleType( buf, exp->v.func->sp->tp, TRUE);
                }
                break;
			case en_pc:
			case en_global:
			case en_label:
            case en_const:
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
					if (!nonpointer)
						*buf++ = '&';
					strcpy(buf, exp->v.sp->name);
					*buf++ = '$';
                    *buf = 0;
				}
				break;
			default:
				*buf = 0;
				break;
		}
	}    
	buf += strlen(buf);                    
    return buf;
}
static char * mangleExpression(char *buf, EXPRESSION *exp)
{
	if (exp)
	{
		*buf++= '$';
        buf = mangleExpressionInternal(buf, exp);
	}
	return buf;
}
static char * mangleTemplate(char *buf, SYMBOL *sym, TEMPLATEPARAMLIST *params)
{
    BOOLEAN bySpecial = FALSE;
    if (params && params->p->type == kw_new && ((sym->instantiated && !sym->templateLevel) || (params && params->p->bySpecialization.types)))
    {
        params = params->p->bySpecialization.types;
        bySpecial = TRUE;
    }
	if ((sym->isConstructor || sym->isDestructor) && sym->templateLevel == sym->parentClass->templateLevel)
	{
		strcpy(buf, sym->name);
        while (*buf) buf++;
	}
	else
	{
		*buf++ = '#';
		strcpy(buf, sym->name);
		strcat(buf, "$");
		buf += strlen(buf);
		while (params)
		{
			switch(params->p->type)
			{
				case kw_typename:
					if (params->p->packed)
					{
						if (params->p->byPack.pack)
						{
							TEMPLATEPARAMLIST *pack = params->p->byPack.pack;
							while (pack)
							{
								buf = mangleType(buf, pack->p->byClass.val, TRUE); 
								pack = pack->next;
							}
						}
						else
						{
							*buf++ = 'e';
							buf = getName(buf, params->argsym);
						}
					}
					else if (bySpecial)
					{
						buf = mangleType(buf, params->p->byClass.dflt, TRUE); 
					}
					else if (sym->instantiated && params->p->byClass.val)
					{
						buf = mangleType(buf, params->p->byClass.val, TRUE); 
					}
					else
					{
						if (params->p->byClass.dflt)
						{
							buf = mangleType(buf, params->p->byClass.dflt, TRUE); 
						}
						else
						{
							buf = getName(buf, params->argsym);
						}
					}
					break;
				case kw_template:
					if (params->p->packed)
						*buf++ = 'e';
					if (bySpecial && params->p->byTemplate.dflt && params->p->byTemplate.val)
					{
						buf = mangleTemplate(buf, params->p->byTemplate.dflt, params->p->byTemplate.val->templateParams);
					}
					else if (sym->instantiated && params->p->byTemplate.val)
					{
						buf = mangleTemplate(buf, params->p->byTemplate.val, params->p->byTemplate.val->templateParams);
					}
					else if (params->argsym)
					{
						buf = getName(buf, params->argsym);
					}
					else
					{
						buf = getName(buf, params->p->byTemplate.dflt);
					}
					break;
				case kw_int:
					if (params->p->packed)
                    {
						*buf++ = 'e';
                        if (params->p->byPack.pack)
                        {
                            params = params->p->byPack.pack;
                            continue;
                        }
                    }
                    else
                    {
    					buf = mangleType(buf, params->p->byNonType.tp, TRUE);
    					if (bySpecial || sym->instantiated)
    					{
    						EXPRESSION *exp = bySpecial ? params->p->byNonType.dflt : params->p->byNonType.val;
                            buf = mangleExpression(buf, exp);
    					}
                        else if (params->p->byNonType.dflt)
                        {
                            buf = mangleExpression(buf, params->p->byNonType.dflt);
                        }
                    }
					break;
				default:
					break;
			}
			params = params->next;
		}
		*buf++ = '~';
	    *buf = 0;
	}
    return buf;
}
static char *lookupName(char *in, char *name)
{
    int i;
    for (i=0; i < mangledNamesCount; i++)
        if (!strcmp(name, mangledNames[i]))
            break;
    if (i < mangledNamesCount)
    {
        my_sprintf(in, "n%c", i < 10 ? i + '0' : i - 10 + 'A');
    }
    else
    {
        if (mangledNamesCount < MAX_MANGLE_NAME_COUNT)
            strcpy(mangledNames[mangledNamesCount++], name);                    
        my_sprintf(in, "%d%s", strlen(name), name);
    }
    return in;
}
static char *getName(char *in, SYMBOL *sp)
{
    if (!sp)
    {
        strcpy(in, "????");
    }
    else
    {
        int i;
        char buf[4096], *p;
        p = mangleClasses(buf, sp->parentClass);
        if (p != buf)
            *p++ = '@';
        if (sp->templateLevel && sp->templateParams)
        {
            p = mangleTemplate(p, sp, sp->templateParams);
        }
        else
        {
            strcpy(p, sp->name);
        }
        in = lookupName(in, buf);
    }
    while (*in) in++;
    return in;
}
char *mangleType (char *in, TYPE *tp, BOOLEAN first)
{
    char nm[4096];
    int i;
    HASHREC *hr ;
    if(!tp)
    {
        my_sprintf(in, "%d%s", strlen("initializer-list"), "initializer-list");
        while (*in) in++;
        return in;
    }
    while (tp)
    {
        while(tp->type == bt_typedef) tp = tp->btp;
        if (isstructured(tp) && basetype(tp)->sp->templateLevel)
        {
            {
                if (isconst(tp))
                    *in++ = 'x';
                if (isvolatile(tp))
                    *in++ = 'y';
                if (islrqual(tp))
                    *in++ = 'r';
                if (isrrqual(tp))
                    *in++ = 'R';
            }
            in = mangleTemplate(in, basetype(tp)->sp, basetype(tp)->sp->templateParams);
            return in;
        }
        else
        {
            if (isconst(tp))
                *in++ = 'x';
            if (isvolatile(tp))
                *in++ = 'y';
            if (islrqual(tp))
                *in++ = 'r';
            if (isrrqual(tp))
                *in++ = 'R';
            tp = basetype(tp);
            switch (tp->type)
            {
            case bt_func:
            case bt_ifunc:
                if (basetype(tp)->sp && ismember(basetype(tp)->sp) && !first)
                {
                    *in++ = 'M';
                    in = getName(in, tp->sp->parentClass);
                    while (*in) in++;
                }
                *in++ = 'q';
                hr = tp->syms->table[0];
                while (hr)
                {
                    SYMBOL *sp = (SYMBOL *)hr->p;
                    if (!sp->thisPtr)
                        in = mangleType(in, sp->tp, TRUE);
                    hr = hr->next;
                }
                *in++ = '$';
                // return value comes next
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
                        hr = hr->next;
                    }
                    *in++ = '$';
                    tp = tp->btp; // so we can get to tp->btp->btp
                }
                else
                {
                    *in++ = '$';
                }
                break;
            case bt_enum:
            case bt_struct:
            case bt_union:
            case bt_class:
                in = getName(in, tp->sp);
                return in;
            case bt_bool:
                in = lookupName(in, "bool");
                while (*in) in++;
                break;
            case bt___string:
                in = lookupName(in, "__string");
                while (*in) in++;
                break;
            case bt___object:
                in = lookupName(in, "__object");
                while (*in) in++;
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
                if (tp->nullptrType)
                {
                    in = lookupName(in, "nullptr_t");
                    while (*in) in++;
                    return;
                }
                else
                {
                    if (first || !tp->array)
                    {
                        *in++ = 'p';
                    }
                    else
                    {
                        my_sprintf(in, "A%ld", tp->btp->size ? tp->size / tp->btp->size : 0);
                        while (*in) in++;
                    }
                }
                break;
            case bt_far:
                *in++ = 'P';
                break;
            case bt_lref:
                *in++ = 'r';
                break;
            case bt_rref:
                *in++ = 'R';
                break;
            case bt_ellipse:
                *in++ = 'e';
                break;
            case bt_void:
            case bt_any:
                *in++ = 'v';
                break;
            case bt_templateparam:
                in = getName(in, tp->templateParam->argsym);
                break;
            case bt_templateselector:
            {
                TEMPLATESELECTOR *s = tp->sp->templateSelector;
                char *p;
                s = s->next;
                if (s->isTemplate)
                    p = mangleTemplate(nm, s->sym, s->templateParams);
                else
                    p = getName(nm, s->sym);
                p[0] = 0;
                if (strlen(nm) > sizeof(nm))
                    p = mangleTemplate(nm, s->sym, s->templateParams);
                s = s->next;
                while (s)
                {
                    strcat(nm, "@");
                    strcat(nm, s->name);
                    s = s->next;
                }
                p = nm;
                while (isdigit(*p))
                    p++;
                my_sprintf(in, "%d%s", strlen(p), p);
                while (*in) in++;
                return in;
            }
            break;
            case bt_templatedecltype:
                // the index is being used to make names unique so two decltypes won't collide when storing them
                // in a symbol table...
                declTypeIndex = (declTypeIndex + 1) % 1000;
                *in++ = 'E';
                my_sprintf(in, "%03d", declTypeIndex);
                in += 3;
                break;
            case bt_aggregate:
                in = getName(in, tp->sp);
                break;
            default:
                diag("mangleType: unknown type");
                break;
            }
        }
        tp = tp->btp;
    }
    *in= 0;
    return in;
}
void SetLinkerNames(SYMBOL *sym, enum e_lk linkage)
{
    char errbuf[8192], *p = errbuf;
    SYMBOL *lastParent;
    mangledNamesCount = 0;
    if (linkage == lk_none || linkage == lk_cdecl)
    {
        if (cparams.prm_cplusplus || chosenAssembler->msil)
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
    if (linkage == lk_virtual)
    {
        if (cparams.prm_cplusplus)
            linkage = lk_cpp;
        else
            linkage = lk_c;  
    }   
    switch (linkage)
    {
        case lk_auto:
            p = mangleClasses(p, theCurrentFunc);
            my_sprintf(p, "@%s", sym->name);
            
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
            lastParent = sym;
            while (lastParent->parentClass)
                lastParent = lastParent->parentClass;
            p = mangleNameSpaces(p, lastParent->parentNameSpace);
            p = mangleClasses(p, sym->parentClass);
            *p++ = '@';
            if (sym->templateLevel && sym->templateParams)
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
                    while (p > errbuf && (*--p != '$' || tmplCount)) 
                        if (*p == '~')
                            tmplCount++;
                        else if (*p == '#')
                            tmplCount--;
                    p[1] = 0;
                }
                else
                {
                    p = mangleType(p, sym->tp, TRUE); // otherwise functions get their parameter list in the name
//                    if (!sym->templateLevel)
                    {
                        int tmplCount = 0;
                        while (p > errbuf && (*--p != '$' || tmplCount)) 
                            if (*p == '~')
                                tmplCount++;
                            else if (*p == '#')
                                tmplCount--;
                        if (basetype(sym->tp)->btp->type == bt_memberptr)
                        {
                            while (p > errbuf && (*--p != '$' || tmplCount)) 
                                if (*p == '~')
                                    tmplCount++;
                                else if (*p == '#')
                                    tmplCount--;
                        }
                        p[1] = 0;
                    }
                }            
            }
            *p = 0;
            break;
    }
    sym->decoratedName = sym->errname = litlate(errbuf);
}
