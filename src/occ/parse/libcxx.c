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

extern char *overloadNameTab[];
extern TYPE stdint;

void libcxx_init(void)
{
}
static LEXEME *getTypeList(LEXEME *lex, SYMBOL *funcsp, INITLIST **lptr)
{
    *lptr = NULL;
    do
    {
        TYPE *tp = NULL;
        lex = getsym(); /* past ( or , */
        lex = get_type_id(lex, &tp, funcsp, sc_cast, FALSE, TRUE);
        if (!tp)
            break;
        if (tp->type != bt_templateparam)
        {
            *lptr = Alloc(sizeof(INITLIST));
            (*lptr)->tp = tp;
            (*lptr)->exp = intNode(en_c_i, 0);
            lptr = &(*lptr)->next;
        }
        else if (tp->templateParam->p->packed)
        {
            TEMPLATEPARAMLIST *tpl = tp->templateParam->p->byPack.pack;
            needkw(&lex, ellipse);
            while (tpl)
            {
                if (tpl->p->byClass.val)
                {
                    *lptr = Alloc(sizeof(INITLIST));
                    (*lptr)->tp = tpl->p->byClass.val;
                    (*lptr)->exp = intNode(en_c_i, 0);
                    lptr = &(*lptr)->next;
                }
                tpl = tpl->next;
            }
            
        }
        else   
        {
            if (tp->templateParam->p->byClass.val)
            {
                *lptr = Alloc(sizeof(INITLIST));
                (*lptr)->tp = tp->templateParam->p->byClass.val;
                (*lptr)->exp = intNode(en_c_i, 0);
                lptr = &(*lptr)->next;
            }
        }
    } while (MATCHKW(lex, comma));
    needkw(&lex, closepa);
    return lex;
}
BOOLEAN parseBuiltInTypelistFunc(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    if (!strcmp(sym->name, "__is_constructible"))
    {
        INITLIST *lst;
        BOOLEAN rv = FALSE;
        FUNCTIONCALL funcparams;
        memset(&funcparams, 0, sizeof(funcparams));
        funcparams.sp = sym;
        *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
        lst = funcparams.arguments;
        while (lst)
        {
            lst->tp = PerformDeferredInitialization(lst->tp, NULL);
            lst = lst->next;
            
        }
        if (funcparams.arguments)        
        {
            TYPE *tp2 = funcparams.arguments->tp;
            if (isarray(tp2))
            {
                while (isarray(tp2) && tp2->size != 0)
                    tp2 = tp2->btp;
                    
                if (isarray(tp2))
                {
                    tp2 = FALSE;
                }
            }
            if (tp2)
            {
                if (isarithmetic(tp2) || ispointer(tp2) || basetype(tp2)->type == bt_enum)
                {
                    if (!funcparams.arguments->next)
                    {
                        rv = TRUE;
                    }
                    else if (!funcparams.arguments->next->next)
                    {
                        rv = comparetypes(tp2, funcparams.arguments->next->tp, TRUE);
                    }
                }
                else if (isref(tp2))
                {
                    if (funcparams.arguments->next && !funcparams.arguments->next->next)
                    {
                        rv = comparetypes(tp2, funcparams.arguments->next->tp, TRUE);
                    }
                }
                else if (isstructured(tp2))
                {
                    TYPE *ctp = tp2;
                    EXPRESSION *cexp = NULL;
                    SYMBOL *cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(tp2)->syms);
                    funcparams.thisptr = intNode(en_c_i, 0);
                    funcparams.thistp = Alloc(sizeof(TYPE));
                    funcparams.thistp->type = bt_pointer;
                    funcparams.thistp->btp = basetype(tp2);
                    funcparams.thistp->size = getSize(bt_pointer);
                    funcparams.ascall = TRUE;
                    funcparams.arguments = funcparams.arguments->next;
                    rv = GetOverloadedFunction(tp, &funcparams.fcall, cons, &funcparams, NULL, FALSE, 
                                  FALSE, FALSE, _F_SIZEOF) != NULL;
                }
            }
        }
        *exp = intNode(en_c_i, rv);
        *tp = &stdint;
        return TRUE;
    }
    else if (!strcmp(sym->name, "__is_convertible_to"))
    {
        BOOLEAN rv = TRUE;
        FUNCTIONCALL funcparams;
        memset(&funcparams, 0, sizeof(funcparams));
        funcparams.sp = sym;
        *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
        if (funcparams.arguments && funcparams.arguments->next && !funcparams.arguments->next->next) 
        {
            TYPE *from = funcparams.arguments->tp;
            TYPE *to = funcparams.arguments->next->tp;
            if (isref(from) && isref(to))
            {
                if (basetype(to)->type == bt_lref)
                {
                    if (basetype(from)->type == bt_rref)
                        rv = FALSE;
                }
            }
            else if (isref(from))
                rv = FALSE;
            if (isfunction(from))
                from = basetype(from)->btp;
            if (rv)
            {
                while (isref(from))
                    from = basetype(from)->btp;
                while (isref(to))
                    to = basetype(to)->btp;
                rv = comparetypes(to, from, FALSE);
                if (!rv && isstructured(from) && isstructured(to))
				{
                   if (classRefCount(basetype(to)->sp, basetype(from)->sp) == 1)
                       rv = TRUE;
				}
				if (!rv && isstructured(from))
				{
					SYMBOL *sp = search("$bcall", basetype(from)->syms);
					if (sp)
					{
						HASHREC *hr = sp->tp->syms->table[0];
						while (hr)
						{
							if (comparetypes(basetype(((SYMBOL *)hr->p)->tp)->btp, to, FALSE))
							{
								rv= TRUE;
								break;
							}
							hr = hr->next;
						}
					}
				}
            }
        }
        else
        {
            rv = FALSE;
        }
        *exp = intNode(en_c_i, rv);
        *tp = &stdint;
        return TRUE;
    }
    return FALSE;
}
