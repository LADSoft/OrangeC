/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 * 
 */

#ifndef LINKEXPRESSION_H
#define LINKEXPRESSION_H
#include "ObjTypes.h"
#include <set>
#include <memory>

class ObjSection;

class LinkExpression;
class LinkExpressionSymbol
{
  public:
    LinkExpressionSymbol(const ObjString& Name, LinkExpression* Value) : name(Name), value(Value) {}
    ~LinkExpressionSymbol();
    ObjString& GetName() { return name; }
    LinkExpression* GetValue() { return value; }
    void SetValue(LinkExpression* v) { value = v; }

  private:
    ObjString name;
    LinkExpression* value;
};
struct leltcompare
{
    bool operator()(LinkExpressionSymbol* left, LinkExpressionSymbol* right) const { return left->GetName() < right->GetName(); }
};
class LinkExpression
{

  public:
    // clang-format off
        enum eOperator { eValue, ePC, eSymbol, eUnresolvedSection, eSection, eAdd, eSub, eMul, eDiv, eNeg, eCpl };
    // clang-format on
    LinkExpression() : op(eValue), left(nullptr), right(nullptr), symbolName(""), value(0), sect(0), unresolvedSection(nullptr) {}
    LinkExpression(eOperator Op) :
        op(Op), left(nullptr), right(nullptr), symbolName(""), value(0), sect(0), unresolvedSection(nullptr)
    {
    }
    LinkExpression(ObjInt Value) :
        op(eValue), left(nullptr), right(nullptr), symbolName(""), value(Value), sect(0), unresolvedSection(nullptr)
    {
    }
    LinkExpression(int section, ObjInt base, ObjInt offs);
    LinkExpression(const ObjString& SymbolName) :
        op(eSymbol), left(nullptr), right(nullptr), symbolName(SymbolName), value(0), sect(0), unresolvedSection(nullptr)
    {
    }
    LinkExpression(eOperator Op, LinkExpression* Left) :
        op(Op), left(Left), right(nullptr), symbolName(""), value(0), sect(0), unresolvedSection(nullptr)
    {
    }
    LinkExpression(eOperator Op, LinkExpression* Left, LinkExpression* Right) :
        op(Op), left(Left), right(Right), symbolName(""), value(0), sect(0), unresolvedSection(nullptr)
    {
    }
    LinkExpression(const LinkExpression& exp);
    LinkExpression(ObjSection* Unresolved) :
        op(eUnresolvedSection), unresolvedSection(Unresolved), left(nullptr), right(nullptr), value(0), sect(0)
    {
    }
    ~LinkExpression();
    eOperator GetOperator() { return op; }
    ObjSection* GetUnresolvedSection() { return unresolvedSection; }
    LinkExpression* GetLeft() { return left; }
    LinkExpression* GetRight() { return right; }
    ObjString& GetSymbol() { return symbolName; }
    ObjInt& GetValue() { return value; }
    int GetSection() { return sect; }
    static bool EnterSymbol(LinkExpressionSymbol* Symbol, bool removeOld = false);
    static LinkExpressionSymbol* FindSymbol(const std::string& name);
    typedef std::set<LinkExpressionSymbol*, leltcompare>::iterator iterator;
    static iterator begin() { return symbols.begin(); }
    static iterator end() { return symbols.end(); }
    ObjInt Eval(ObjInt pc);
    LinkExpression* Eval(int sect, ObjInt base, ObjInt offset);

  private:
    LinkExpression* LookupSymbol(ObjString& symbol);
    eOperator op;
    LinkExpression* left;
    LinkExpression* right;
    ObjString symbolName;
    ObjInt value;
    ObjInt sect;
    static std::set<LinkExpressionSymbol*, leltcompare> symbols;
    ObjSection* unresolvedSection;
};
#endif
