#pragma once
/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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
namespace Parser
{
extern int anonymousNotAlloc;
void helpinit(void);
void deprecateMessage(SYMBOL* sym);
bool ismember(SYMBOL* sym);
bool istype(SYMBOL* sym);
bool ismemberdata(SYMBOL* sym);
bool startOfType(LEXEME* lex, bool assumeType);
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
bool isconstraw(const TYPE* tp, bool useTemplate);
bool isconst(const TYPE* tp);
bool isvolatile(const TYPE* tp);
bool islrqual(TYPE* tp);
bool isrrqual(TYPE* tp);
bool isrestrict(TYPE* tp);
bool isatomic(TYPE* tp);
bool isvoid(TYPE* tp);
bool isvoidptr(TYPE* tp);
bool isarray(TYPE* tp);
bool isunion(TYPE* tp);
void DeduceAuto(TYPE** pat, TYPE* nt);
SYMBOL* getFunctionSP(TYPE** tp);
LEXEME* concatStringsInternal(LEXEME* lex, STRING** str, int* elems);
LEXEME* concatStrings(LEXEME* lex, EXPRESSION** expr, enum e_lexType* tp, int* elems);
bool isintconst(EXPRESSION* exp);
bool isfloatconst(EXPRESSION* exp);
bool isimaginaryconst(EXPRESSION* exp);
bool iscomplexconst(EXPRESSION* exp);
EXPRESSION* anonymousVar(enum e_sc storage_class, TYPE* tp);
void deref(TYPE* tp, EXPRESSION** exp);
int sizeFromType(TYPE* tp);
void cast(TYPE* tp, EXPRESSION** exp);
bool castvalue(EXPRESSION* exp);
bool xvalue(EXPRESSION* exp);
bool lvalue(EXPRESSION* exp);
EXPRESSION* convertInitToExpression(TYPE* tp, SYMBOL* sym, EXPRESSION* expsym, SYMBOL* funcsp, INITIALIZER* init,
                                    EXPRESSION* thisptr, bool isdest);
bool assignDiscardsConst(TYPE* dest, TYPE* source);
bool isconstzero(TYPE* tp, EXPRESSION* exp);
bool fittedConst(TYPE* tp, EXPRESSION* exp);
bool isarithmeticconst(EXPRESSION* exp);
bool isconstaddress(EXPRESSION* exp);
SYMBOL*(clonesym)(SYMBOL* sym_in, bool full);
e_bt btmax(e_bt left, e_bt right);
TYPE* destSize(TYPE* tp1, TYPE* tp2, EXPRESSION** exp1, EXPRESSION** exp2, bool minimizeInt, TYPE* atp);
EXPRESSION* RemoveAutoIncDec(EXPRESSION* exp);
}  // namespace Parser