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
#include "LinkExpression.h"
#include "LinkManager.h"
#include "ObjSection.h"
#include <exception>
std::set<LinkExpressionSymbol *, leltcompare> LinkExpression::symbols;
LinkExpression::LinkExpression(const LinkExpression &exp)
{
    op = exp.op;
    value = exp.value;
    symbolName = exp.symbolName;
    sect = exp.sect;
    if (exp.left)
        left = new LinkExpression(*exp.left);
    else
        left = NULL;
    if (exp.right)
        right = new LinkExpression(*exp.right);
    else
        right = NULL;
        
}
LinkExpression::LinkExpression(int section, ObjInt base, ObjInt offs) : op (eAdd), 
        left(NULL), right(new LinkExpression(offs)), symbolName(""), 
        value(0), sect(0)
{
    left = new LinkExpression;
    left->op = eSection;
    left->value = base; 
    left->sect = section;
}
LinkExpression *LinkExpression::Eval(int section, int base, ObjInt offset)
{
    if (left)
        left = left->Eval(section, base, offset);
    if (right)
        right = right->Eval(section, base, offset);
    LinkExpression *rv = this;
    switch(op)
    {
        case ePC:
            delete rv;
            rv = new LinkExpression(section, base, offset);
            break;
        case eSymbol:
        {
            LinkExpressionSymbol *exp = FindSymbol(symbolName);
            if (!exp)
            {
                LinkManager::LinkError("Variable "+symbolName+" is undefined");
            }
            else
            {
                delete rv;
                rv = new LinkExpression(*(exp->GetValue()));
            }
            break;
        }
    }
    return rv;
}
ObjInt LinkExpression::Eval(ObjInt pc)
{
    switch(op)
    {
        case eValue:
        case eSection:
            return value;
        case eUnresolvedSection:
            return unresolvedSection->GetBase() + unresolvedSection->GetOffset()->Eval(pc);
        case ePC:
            return pc;
        case eSymbol:
        {
            LinkExpressionSymbol *exp = FindSymbol(symbolName);
            ObjInt n=0;
            if (!exp)
            {
                LinkManager::LinkError("Variable "+symbolName+" is undefined");
            }
            else
            {
                n = exp->GetValue()->Eval(pc);
            }
            return n;
        }
        case eAdd:
            return left->Eval(pc) + right->Eval(pc);
        case eSub:
            return left->Eval(pc) - right->Eval(pc);
        case eMul:
            return left->Eval(pc) * right->Eval(pc);
        case eDiv:
            return left->Eval(pc) / right->Eval(pc);
        case eNeg:
            return -left->Eval(pc);
        case eCpl:
            return ~left->Eval(pc);
        default:
        {
            std::bad_exception v;
            throw v;
        }
    }
}
bool LinkExpression::EnterSymbol(LinkExpressionSymbol *Symbol, bool removeOld)
{
    std::set<LinkExpressionSymbol *, leltcompare>::iterator it = symbols.find(Symbol);
    if (it == symbols.end())
    {
        symbols.insert(Symbol);
        return true;
    }
    else if (removeOld)
    {
        LinkExpressionSymbol *v = *it;
        symbols.erase(it);
        delete v;
        symbols.insert(Symbol);
        return true;
    }
    return false;
}
LinkExpressionSymbol *LinkExpression::FindSymbol(const std::string &name)
{
    LinkExpressionSymbol sym(name, 0);
    std::set<LinkExpressionSymbol *, leltcompare>::iterator it = symbols.find(&sym);
    if (it != symbols.end())
    {
        return *it;
    }
    return NULL;
}
LinkExpression *LinkExpression::LookupSymbol(ObjString &symbolName)
{
    LinkExpressionSymbol sym(symbolName, NULL);
    LinkExpressionSymbol *exp = NULL;
    std::set<LinkExpressionSymbol *, leltcompare>::iterator it = symbols.find(&sym);
    if (it != symbols.end())
        exp = *it;
    if (!exp)
    {
        std::bad_exception v;
        throw v;
    }
    return exp->GetValue();
}

