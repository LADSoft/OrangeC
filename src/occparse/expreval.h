/* Software License Agreement
 *
 *     Copyright(C); 1994-2024 David Lindauer, (LADSoft);
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, ore
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
#pragma once

namespace Parser
{
// localexpr
EXPRESSION* nodeSizeof(Type* tp, EXPRESSION* exp, int flags = 0);
void eval_unary_left_fold(LexList* lex, SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, LexList *start, Type* lefttp,
                     EXPRESSION* leftexp, bool ismutable, int flags);
void eval_unary_right_fold(LexList* lex, SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, LexList *start, Type* lefttp,
                     EXPRESSION* leftexp, bool ismutable, int flags);
void eval_binary_fold(LexList* lex, SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, LexList *leftstart, Type* lefttp,
                    EXPRESSION* leftexp, LexList *rightstart, Type* righttp, EXPRESSION* rightexp, bool ismutable, int flags);
void eval_unary_plus(LexList* lex, SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, Type* lefttp,
                     EXPRESSION* leftexp, bool ismutable, int flags);
void eval_unary_minus(LexList *lex, SYMBOL *funcsp, Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, bool ismutable, int flags);
void eval_unary_not(LexList *lex, SYMBOL *funcsp, Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, bool ismutable, int flags);
void eval_unary_complement(LexList *lex, SYMBOL *funcsp, Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, bool ismutable, int flags);
void eval_unary_autoincdec(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, bool ismutable, int flags);
bool eval_binary_pm(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags); 
bool eval_binary_times(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags);
bool eval_binary_add(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags);
bool eval_binary_shift(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags);
bool eval_binary_inequality(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags);
bool eval_binary_equality(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags);
bool eval_binary_logical(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags);
bool eval_binary_assign(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags);
bool eval_binary_comma(LexList *lex, SYMBOL *funcsp, Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags);
}  // namespace Parser