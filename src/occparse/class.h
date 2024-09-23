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
 */
#pragma once

namespace Parser
{

LexList* nestedPath(LexList* lex, SYMBOL** sym, std::list<NAMESPACEVALUEDATA*>** ns, bool* throughClass, bool tagsOnly, StorageClass storage_class,
                    bool isType, int flags);
SYMBOL* classdata(const char* name, SYMBOL* cls, SYMBOL* last, bool isvirtual, bool tagsOnly);
SYMBOL* templatesearch(const char* name, std::list<TEMPLATEPARAMPAIR>* arg);
TEMPLATEPARAMPAIR* getTemplateStruct(char* name);
LexList* tagsearch(LexList* lex, char* name, SYMBOL** rsp, SymbolTable<SYMBOL>** table, SYMBOL** strSym_out, std::list<NAMESPACEVALUEDATA*>** nsv_out,
    StorageClass storage_class);
SYMBOL* classsearch(const char* name, bool tagsOnly, bool needTypeOrNamespace, bool toErr);
SYMBOL* finishSearch(const char* name, SYMBOL* encloser, std::list<NAMESPACEVALUEDATA*>* ns, bool tagsOnly, bool throughClass,
                     bool namespaceOnly);
LexList* nestedSearch(LexList* lex, SYMBOL** sym, SYMBOL** strSym, std::list<NAMESPACEVALUEDATA*>** nsv, bool* destructor, bool* isTemplate,
                      bool tagsOnly, StorageClass storage_class, bool errIfNotFound, bool isType);
LexList* getIdName(LexList* lex, SYMBOL* funcsp, char* buf, int* ov, Type** castType);
LexList* id_expression(LexList* lex, SYMBOL* funcsp, SYMBOL** sym, SYMBOL** strSym, std::list<NAMESPACEVALUEDATA*>** nsv, bool* isTemplate,
                       bool tagsOnly, bool membersOnly, char* idname, int flags);
SYMBOL* LookupSym(char* name);
bool isAccessible(SYMBOL* derived, SYMBOL* currentBase, SYMBOL* member, SYMBOL* funcsp, AccessLevel minAccess, bool asAddress);
bool isExpressionAccessible(SYMBOL* derived, SYMBOL* sym, SYMBOL* funcsp, EXPRESSION* exp, bool asAddress);
bool checkDeclarationAccessible(SYMBOL* sp, SYMBOL* derived, SYMBOL* funcsp);

}  // namespace Parser