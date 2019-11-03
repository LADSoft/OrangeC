/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
 */
#include "compiler.h"
#include <unordered_map>

bool equalnode(SimpleExpression* left, SimpleExpression *right);
inline bool isarithmeticconst(SimpleExpression* e)
{
    switch (e->type)
        case se_i:
        case se_ui:
        case se_f:
        case se_fc:
        case se_fi:
            return true;
    return false;
}
inline bool isintconst(SimpleExpression* e)
{
    switch (e->type)
        case se_i:
        case se_ui:
            return true;
    return false;
}
inline bool isfloatconst(SimpleExpression* e)
{
    switch (e->type)
        case se_f:
            return true;
    return false;
}
inline bool iscomplexconst(SimpleExpression* e)
{
    switch (e->type)
        case se_fc:
            return true;
    return false;
}
inline bool isimaginaryconst(SimpleExpression* e)
{
    switch (e->type)
        case se_fi:
            return true;
    return false;
}
inline bool isconstaddress(SimpleExpression* exp)
{
    switch (exp->type)
    {
    case se_add:
        return (isconstaddress(exp->left) || isintconst(exp->left)) && (isconstaddress(exp->right) || isintconst(exp->right));
    case se_global:
    case se_pc:
    case se_labcon:
        return true;
    case se_func:
        return !exp->ascall;
    case se_threadlocal:
    default:
        return false;
    }
}
inline void GENREF(SimpleSymbol* sym)
{
    sym->genreffed = true;
}

struct ArgList
{
    ArgList* next;
    SimpleType* tp;
    SimpleExpression* exp;
};
struct SymbolManager
{
    static SimpleSymbol* Get(struct sym *sym);
    static SimpleExpression* Get(struct expr* e);
    static SimpleType* Get(struct typ *tp);
    static void clear() { symbols.clear(); }
    static SimpleSymbol* Get(const char* name);
    static void Add(SimpleSymbol* sym);
private:
    static SimpleSymbol* Make(struct sym *sym);
    static std::unordered_map<struct sym*, SimpleSymbol*> symbols;
    static std::unordered_map<std::string, SimpleSymbol*> globalSymbols;
};
