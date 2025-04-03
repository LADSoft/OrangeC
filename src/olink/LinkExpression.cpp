/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#include "LinkExpression.h"
#include "LinkManager.h"
#include "ObjSection.h"
#include <exception>
LinkExpressionSymbol::~LinkExpressionSymbol() {}

std::set<LinkExpressionSymbol*, leltcompare> LinkExpression::symbols;
LinkExpression::LinkExpression(const LinkExpression& exp) : unresolvedSection(nullptr)
{
    op = exp.op;
    value = exp.value;
    symbolName = exp.symbolName;
    sect = exp.sect;
    if (exp.left)
        left = new LinkExpression(*exp.left);
    else
        left = nullptr;
    if (exp.right)
        right = new LinkExpression(*exp.right);
    else
        right = nullptr;
}
LinkExpression::LinkExpression(int section, ObjInt base, ObjInt offs) :
    op(eAdd),

    left(new LinkExpression()),
    right(new LinkExpression(offs)),
    symbolName(""),
    value(0),
    sect(0),
    unresolvedSection(nullptr)
{
    left->op = eSection;
    left->value = base;
    left->sect = section;
}
LinkExpression::~LinkExpression()
{
    delete left;
    delete right;
}

LinkExpression* LinkExpression::Eval(int section, int base, ObjInt offset)
{
    if (left)
        left = left->Eval(section, base, offset);
    if (right)
        right = right->Eval(section, base, offset);
    LinkExpression* rv = this;
    switch (op)
    {
        case ePC:
            delete rv;
            rv = new LinkExpression(section, base, offset);
            break;
        case eSymbol: {
            LinkExpressionSymbol* exp = FindSymbol(symbolName);
            if (!exp)
            {
                LinkManager::LinkError("Variable " + symbolName + " is undefined");
            }
            else
            {
                delete rv;
                rv = new LinkExpression(*(exp->GetValue()));
            }
            break;
        }
        default:
            break;
    }
    return rv;
}
ObjInt LinkExpression::Eval(ObjInt pc)
{
    switch (op)
    {
        case eValue:
        case eSection:
            return value;
        case eUnresolvedSection:
            return unresolvedSection->GetBase() + unresolvedSection->GetOffset()->Eval(pc);
        case ePC:
            return pc;
        case eSymbol: {
            LinkExpressionSymbol* exp = FindSymbol(symbolName);
            ObjInt n = 0;
            if (!exp)
            {
                LinkManager::LinkError("Variable " + symbolName + " is undefined");
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
        default: {
            std::bad_exception v;
            throw v;
        }
    }
}
bool LinkExpression::EnterSymbol(LinkExpressionSymbol* Symbol, bool removeOld)
{
    auto it = symbols.find(Symbol);
    if (it == symbols.end())
    {
        symbols.insert(Symbol);
        return true;
    }
    else if (removeOld)
    {
        delete (*it);
        symbols.erase(it);
        symbols.insert(Symbol);
        return true;
    }
    return false;
}
LinkExpressionSymbol* LinkExpression::FindSymbol(const std::string& name)
{
    LinkExpressionSymbol sym(name, 0);
    auto it = symbols.find(&sym);
    if (it != symbols.end())
    {
        return *it;
    }
    return nullptr;
}
LinkExpression* LinkExpression::LookupSymbol(ObjString& symbolName)
{
    LinkExpressionSymbol sym(symbolName, nullptr);
    LinkExpressionSymbol* exp = nullptr;
    auto it = symbols.find(&sym);
    if (it != symbols.end())
        exp = *it;
    if (!exp)
    {
        std::bad_exception v;
        throw v;
    }
    return exp->GetValue();
}
