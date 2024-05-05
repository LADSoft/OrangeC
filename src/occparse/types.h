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

namespace Parser
{
bool isautotype(TYPE* tp);
bool isunsigned(TYPE* tp);
bool isbitint(TYPE* tp);
bool isint(TYPE* tp);
bool isfloat(TYPE* tp);
bool iscomplex(TYPE* tp);
bool isimaginary(TYPE* tp);
bool isarithmetic(TYPE* tp);
bool ismsil(TYPE* tp);
bool isconstraw(const TYPE* tp);
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
TYPE* MakeType(TYPE& tp, BasicType type, TYPE* base = nullptr);
TYPE* MakeType(BasicType type, TYPE* base = nullptr);
TYPE* CopyType(TYPE* tp, bool deep = false, std::function<void(TYPE*&, TYPE*&)> callback = nullptr);
LEXLIST* getPointerQualifiers(LEXLIST* lex, TYPE** tp, bool allowstatic);
LEXLIST* getFunctionParams(LEXLIST* lex, SYMBOL* funcsp, SYMBOL** spin, TYPE** tp, bool inTemplate, StorageClass storage_class,
                           bool funcptr);
LEXLIST* getBeforeType(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, SYMBOL** spi, SYMBOL** strSym, std::list<NAMESPACEVALUEDATA*>** nsv,
    bool inTemplate, StorageClass storage_class, Linkage* linkage, Linkage* linkage2, Linkage* linkage3,
    bool* notype, bool asFriend, int consdest, bool beforeOnly, bool funcptr);
LEXLIST* getBasicType(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, SYMBOL** strSym_out, bool inTemplate, StorageClass storage_class,
    Linkage* linkage_in, Linkage* linkage2_in, Linkage* linkage3_in, AccessLevel access, bool* notype,
    bool* defd, int* consdest, bool* templateArg, bool* deduceTemplate, bool isTypedef, bool templateErr, bool inUsing, bool asfriend,
    bool constexpression);
LEXLIST* get_type_id(LEXLIST* lex, TYPE** tp, SYMBOL* funcsp, StorageClass storage_class, bool beforeOnly, bool toErr, bool inUsing);
bool istype(SYMBOL* sym);
bool startOfType(LEXLIST* lex, bool* structured, bool assumeType);
void UpdateRootTypes(TYPE* tp);
bool comparetypes(TYPE* typ1, TYPE* typ2, int exact);
bool compareXC(TYPE* typ1, TYPE* typ2);
bool matchingCharTypes(TYPE* typ1, TYPE* typ2);
void typenumptr(char* buf, TYPE* tp);
void RenderExpr(char* buf, EXPRESSION* exp);
TYPE* typenum(char* buf, TYPE* tp);
void typeToString(char* buf, TYPE* typ);
}  // namespace Parser