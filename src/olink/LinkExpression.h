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
#ifndef LINKEXPRESSION_H
#define LINKEXPRESSION_H
#include "ObjTypes.h"
#include <set>

class LinkExpression;
class LinkExpressionSymbol
{
    public:
        LinkExpressionSymbol(const ObjString &Name, LinkExpression *Value) :
            name(Name), value(Value) {}
        ~LinkExpressionSymbol() { delete value; }
        ObjString &GetName() { return name; }
        LinkExpression* GetValue() { return value; }
        void SetValue(LinkExpression *v) { value = v; }
    private:
        ObjString name;
        LinkExpression *value;
} ;
struct leltcompare
{
    bool operator ()(LinkExpressionSymbol *left, LinkExpressionSymbol *right) const
    {return left->GetName() < right->GetName(); }
};
class LinkExpression
{

    public:
        enum eOperator { eValue, ePC, eSymbol, eSection, eAdd, eSub, eMul, eDiv, eNeg, eCpl };
        LinkExpression() : op (eValue), left(NULL), right(NULL), symbolName(""), value(0), sect(0) {}
        LinkExpression(eOperator Op) : op (Op), left(NULL), right(NULL), symbolName(""), value(0) {}
        LinkExpression(ObjInt Value) : op (eValue), left(NULL), right(NULL), symbolName(""), value(Value) {}
        LinkExpression(int section, ObjInt base, ObjInt offs);
        LinkExpression(const ObjString &SymbolName) : op (eSymbol), left(NULL), right(NULL), symbolName(SymbolName), value(0) {}
        LinkExpression(eOperator Op, LinkExpression *Left)
            : op (Op), left(Left), right(NULL), symbolName(""), value(0) {}
        LinkExpression(eOperator Op, LinkExpression *Left, LinkExpression *Right)
            : op (Op), left(Left), right(Right), symbolName(""), value(0) {}
        LinkExpression(const LinkExpression &exp);
        ~LinkExpression() 
        {
            delete left;
             delete right;
        }
        eOperator GetOperator() { return op; }
        LinkExpression *GetLeft() { return left; }
        LinkExpression *GetRight() { return right; }
        ObjString &GetSymbol() { return symbolName; }
        ObjInt &GetValue() { return value; }
        int GetSection() { return sect; }
        static bool EnterSymbol(LinkExpressionSymbol *Symbol, bool removeOld = false);
        static LinkExpressionSymbol *FindSymbol(const std::string &name);
        typedef std::set<LinkExpressionSymbol *, leltcompare>::iterator iterator;
        static iterator begin() { return symbols.begin(); }
        static iterator end() { return symbols.end(); }
        ObjInt Eval(ObjInt pc);
        LinkExpression *Eval(int sect, ObjInt base, ObjInt offset);
    private:
        LinkExpression *LookupSymbol(ObjString &symbol);
        eOperator op;
        LinkExpression *left;
        LinkExpression *right;
        ObjString symbolName;
        ObjInt value;
        ObjInt sect;
        static std::set<LinkExpressionSymbol *, leltcompare> symbols;
};
#endif
