#pragma once
/* Software License Agreement
 *
 *     Copyright(C); 1994-2023 David Lindauer, (LADSoft);
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

namespace Parser
{
bool isstructuredmath(Type* tp1, Type* tp2 = nullptr);
bool smallint(Type* tp);
bool largenum(Type* tp);
bool isTemplatedPointer(Type* tp);
EXPRESSION* nodeSizeof(Type* tp, EXPRESSION* exp, int flags = 0);
void eval_unary_left_fold(LEXLIST* lex, SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, LEXLIST *start, Type* lefttp,
                     EXPRESSION* leftexp, bool ismutable, int flags);
void eval_unary_right_fold(LEXLIST* lex, SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, LEXLIST *start, Type* lefttp,
                     EXPRESSION* leftexp, bool ismutable, int flags);
void eval_binary_fold(LEXLIST* lex, SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, LEXLIST *leftstart, Type* lefttp,
                    EXPRESSION* leftexp, LEXLIST *rightstart, Type* righttp, EXPRESSION* rightexp, bool ismutable, int flags);
void eval_unary_plus(LEXLIST* lex, SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, Type* lefttp,
                     EXPRESSION* leftexp, bool ismutable, int flags);
void eval_unary_minus(LEXLIST *lex, SYMBOL *funcsp, Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, bool ismutable, int flags);
void eval_unary_not(LEXLIST *lex, SYMBOL *funcsp, Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, bool ismutable, int flags);
void eval_unary_complement(LEXLIST *lex, SYMBOL *funcsp, Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, bool ismutable, int flags);
void eval_unary_autoincdec(LEXLIST *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, bool ismutable, int flags);
bool eval_binary_pm(LEXLIST *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags); 
bool eval_binary_times(LEXLIST *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags);
bool eval_binary_add(LEXLIST *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags);
bool eval_binary_shift(LEXLIST *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags);
bool eval_binary_inequality(LEXLIST *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags);
bool eval_binary_equality(LEXLIST *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags);
bool eval_binary_logical(LEXLIST *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags);
bool eval_binary_assign(LEXLIST *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags);
bool eval_binary_comma(LEXLIST *lex, SYMBOL *funcsp, Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags);
}  // namespace Parser