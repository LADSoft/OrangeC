#pragma once
/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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
extern int inGetUserConversion;
extern SYMBOL* argFriend;
extern HASHTABLE* CreateHashTable(int size);
#define F_WITHCONS 1
#define F_INTEGER 2
#define F_ARITHMETIC 4
#define F_STRUCTURE 8
#define F_POINTER 16
#define F_CONVERSION 32

#define F_GOFERR 1
#define F_GOFDELETEDERR 2

Optimizer::LIST* tablesearchone(const char* name, NAMESPACEVALUELIST* ns, bool tagsOnly);
Optimizer::LIST* tablesearchinline(const char* name, NAMESPACEVALUELIST* ns, bool tagsOnly, bool allowUsing = false);
SYMBOL* namespacesearch(const char* name, NAMESPACEVALUELIST* ns, bool qualified, bool tagsOnly);
LEXLIST* nestedPath(LEXLIST* lex, SYMBOL** sym, NAMESPACEVALUELIST** ns, bool* throughClass, bool tagsOnly, enum e_sc storage_class,
                    bool isType);
SYMBOL* classdata(const char* name, SYMBOL* cls, SYMBOL* last, bool isvirtual, bool tagsOnly);
SYMBOL* templatesearch(const char* name, TEMPLATEPARAMLIST* arg);
TEMPLATEPARAMLIST* getTemplateStruct(char* name);
SYMBOL* classsearch(const char* name, bool tagsOnly, bool toErr);
SYMBOL* finishSearch(const char* name, SYMBOL* encloser, NAMESPACEVALUELIST* ns, bool tagsOnly, bool throughClass,
                     bool namespaceOnly);
LEXLIST* nestedSearch(LEXLIST* lex, SYMBOL** sym, SYMBOL** strSym, NAMESPACEVALUELIST** nsv, bool* destructor, bool* isTemplate,
                      bool tagsOnly, enum e_sc storage_class, bool errIfNotFound, bool isType);
LEXLIST* getIdName(LEXLIST* lex, SYMBOL* funcsp, char* buf, int* ov, TYPE** castType);
LEXLIST* id_expression(LEXLIST* lex, SYMBOL* funcsp, SYMBOL** sym, SYMBOL** strSym, NAMESPACEVALUELIST** nsv, bool* isTemplate,
                       bool tagsOnly, bool membersOnly, char* idname);
SYMBOL* LookupSym(char* name);
bool isAccessible(SYMBOL* derived, SYMBOL* currentBase, SYMBOL* member, SYMBOL* funcsp, enum e_ac minAccess, bool asAddress);
bool isExpressionAccessible(SYMBOL* derived, SYMBOL* sym, SYMBOL* funcsp, EXPRESSION* exp, bool asAddress);
bool checkDeclarationAccessible(SYMBOL* sp, SYMBOL* derived, SYMBOL* funcsp);
SYMBOL* lookupGenericConversion(SYMBOL* sym, TYPE* tp);
SYMBOL* lookupSpecificCast(SYMBOL* sym, TYPE* tp);
SYMBOL* lookupNonspecificCast(SYMBOL* sym, TYPE* tp);
SYMBOL* lookupIntCast(SYMBOL* sym, TYPE* tp, bool implicit);
SYMBOL* lookupArithmeticCast(SYMBOL* sym, TYPE* tp, bool implicit);
SYMBOL* lookupPointerCast(SYMBOL* sym, TYPE* tp);
void GetRefs(TYPE* tpa, EXPRESSION* expa, bool& lref, bool& rref);
void getSingleConversion(TYPE* tpp, TYPE* tpa, EXPRESSION* expa, int* n, enum e_cvsrn* seq, SYMBOL* candidate, SYMBOL** userFunc,
                         bool allowUser);
bool sameTemplatePointedTo(TYPE* tnew, TYPE* told, bool quals = false);
bool sameTemplate(TYPE* P, TYPE* A, bool quals = false);
void getSingleConversion(TYPE* tpp, TYPE* tpa, EXPRESSION* expa, int* n, enum e_cvsrn* seq, SYMBOL* candidate, SYMBOL** userFunc,
                         bool allowUser);
SYMBOL* detemplate(SYMBOL* sym, FUNCTIONCALL* args, TYPE* atp);
SYMBOL* GetOverloadedTemplate(SYMBOL* sp, FUNCTIONCALL* args);
void weedgathering(Optimizer::LIST** gather);
SYMBOL* GetOverloadedFunction(TYPE** tp, EXPRESSION** exp, SYMBOL* sp, FUNCTIONCALL* args, TYPE* atp, int toErr,
                              bool maybeConversion, bool toInstantiate, int flags);
SYMBOL* MatchOverloadedFunction(TYPE* tp, TYPE** mtp, SYMBOL* sym, EXPRESSION** exp, int flags);

}  // namespace Parser