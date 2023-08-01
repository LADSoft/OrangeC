#pragma once
/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include <stack>
#include <unordered_map>
namespace Parser
{

template <class T>
class NestedStack;
template <class T>
class StackList
{
  private:
    template <class>
    friend class NestedStack;
    std::stack<T>& Get()
    {
        if (pos == stacks.size())
            stacks[pos] = std::stack<T>();
        pos++;
        return stacks[pos - 1];
    }
    void Release()
    {
        if (pos)
        {
            std::stack<T>& stk = stacks[pos - 1];
            while (!stk.empty())
                stk.pop();
            pos--;
        }
    }

    std::unordered_map<int, std::stack<T>> stacks;
    int pos = 0;
};

template <class T>
class NestedStack
{
  public:
    NestedStack(StackList<T>& stacklist) : list(stacklist), thisStack(stacklist.Get()) {}
    ~NestedStack() { list.Release(); }
    T& top() { return thisStack.top(); }
    void push(const T& val) { thisStack.push(val); }
    void pop() { thisStack.pop(); }
    bool empty() { return thisStack.empty(); }

  private:
    std::stack<T>& thisStack;
    StackList<T>& list;
};

extern int anonymousNotAlloc;
void helpinit(void);
void deprecateMessage(SYMBOL* sym);
bool ismember(SYMBOL* sym);
bool istype(SYMBOL* sym);
bool ismemberdata(SYMBOL* sym);
bool startOfType(LEXLIST* lex, bool* structured, bool assumeType);
void UpdateRootTypes(TYPE* tp);
bool isDerivedFromTemplate(TYPE* tp);
bool isautotype(TYPE* tp);
bool isunsigned(TYPE* tp);
bool isint(TYPE* tp);
bool isfloat(TYPE* tp);
bool iscomplex(TYPE* tp);
bool isimaginary(TYPE* tp);
bool isarithmetic(TYPE* tp);
bool ismsil(TYPE* tp);
bool isconstraw(const TYPE* tp);
bool isconst(const TYPE* tp);
bool isconstexpr(const EXPRESSION* expa);
bool isvolatile(const TYPE* tp);
bool islrqual(TYPE* tp);
bool isrrqual(TYPE* tp);
bool isrestrict(TYPE* tp);
bool isatomic(TYPE* tp);
bool isvoid(TYPE* tp);
bool isvoidptr(TYPE* tp);
bool isarray(TYPE* tp);
bool isunion(TYPE* tp);
void DeduceAuto(TYPE** pat, TYPE* nt, EXPRESSION* exp);
SYMBOL* getFunctionSP(TYPE** tp);
LEXLIST* concatStringsInternal(LEXLIST* lex, STRING** str, int* elems);
LEXLIST* concatStrings(LEXLIST* lex, EXPRESSION** expr, e_lexType* tp, int* elems);
bool isintconst(EXPRESSION* exp);
bool isfloatconst(EXPRESSION* exp);
bool isimaginaryconst(EXPRESSION* exp);
bool iscomplexconst(EXPRESSION* exp);
EXPRESSION* anonymousVar(StorageClass storage_class, TYPE* tp);
void deref(TYPE* tp, EXPRESSION** exp);
int sizeFromType(TYPE* tp);
void cast(TYPE* tp, EXPRESSION** exp);
bool castvalue(EXPRESSION* exp);
bool xvalue(EXPRESSION* exp);
bool lvalue(EXPRESSION* exp);
EXPRESSION* convertInitToExpression(TYPE* tp, SYMBOL* sym, EXPRESSION* expsym, SYMBOL* funcsp, std::list<INITIALIZER*>* init,
                                    EXPRESSION* thisptr, bool isdest);
bool assignDiscardsConst(TYPE* dest, TYPE* source);
bool isconstzero(TYPE* tp, EXPRESSION* exp);
bool fittedConst(TYPE* tp, EXPRESSION* exp);
bool isarithmeticconst(EXPRESSION* exp);
bool isconstaddress(EXPRESSION* exp);
SYMBOL*(CopySymbol)(SYMBOL* sym_in, bool full);
BasicType btmax(BasicType left, BasicType right);
TYPE* destSize(TYPE* tp1, TYPE* tp2, EXPRESSION** exp1, EXPRESSION** exp2, bool minimizeInt, TYPE* atp);
EXPRESSION* RemoveAutoIncDec(EXPRESSION* exp);
EXPRESSION* EvaluateDest(EXPRESSION*exp, TYPE* tp);
}  // namespace Parser