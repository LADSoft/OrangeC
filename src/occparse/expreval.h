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
void eval_unary_left_fold( SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, LexemeStreamPosition& start,
                          Type* lefttp, EXPRESSION* leftexp, bool ismutable, int flags);
void eval_unary_right_fold( SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, LexemeStreamPosition& start,
                           Type* lefttp, EXPRESSION* leftexp, bool ismutable, int flags);
void eval_binary_fold( SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, LexemeStreamPosition& leftstart,
                      Type* lefttp, EXPRESSION* leftexp, LexemeStreamPosition& rightstart, Type* righttp, EXPRESSION* rightexp, bool ismutable,
                      int flags);
void eval_unary_plus( SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, Type* lefttp,
                     EXPRESSION* leftexp, bool ismutable, int flags);
void eval_unary_minus( SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, Type* lefttp,
                      EXPRESSION* leftexp, bool ismutable, int flags);
void eval_unary_not( SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, Type* lefttp,
                    EXPRESSION* leftexp, bool ismutable, int flags);
void eval_unary_complement( SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, Type* lefttp,
                           EXPRESSION* leftexp, bool ismutable, int flags);
void eval_unary_autoincdec( SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, Type* lefttp,
                           EXPRESSION* leftexp, bool ismutable, int flags);
bool eval_binary_pm( SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, Type* lefttp,
                    EXPRESSION* leftexp, Type* righttp, EXPRESSION* rightexp, bool ismutable, int flags);
bool eval_binary_times( SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, Type* lefttp,
                       EXPRESSION* leftexp, Type* righttp, EXPRESSION* rightexp, bool ismutable, int flags);
bool eval_binary_add( SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, Type* lefttp,
                     EXPRESSION* leftexp, Type* righttp, EXPRESSION* rightexp, bool ismutable, int flags);
bool eval_binary_shift( SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, Type* lefttp,
                       EXPRESSION* leftexp, Type* righttp, EXPRESSION* rightexp, bool ismutable, int flags);
bool eval_binary_inequality( SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, Type* lefttp,
                            EXPRESSION* leftexp, Type* righttp, EXPRESSION* rightexp, bool ismutable, int flags);
bool eval_binary_equality( SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, Type* lefttp,
                          EXPRESSION* leftexp, Type* righttp, EXPRESSION* rightexp, bool ismutable, int flags);
bool eval_binary_logical( SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, Type* lefttp,
                         EXPRESSION* leftexp, Type* righttp, EXPRESSION* rightexp, bool ismutable, int flags);
bool eval_binary_assign( SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, Type* lefttp,
                        EXPRESSION* leftexp, Type* righttp, EXPRESSION* rightexp, bool ismutable, int flags);
bool eval_binary_comma( SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, Type* lefttp,
                       EXPRESSION* leftexp, Type* righttp, EXPRESSION* rightexp, bool ismutable, int flags);
}  // namespace Parser