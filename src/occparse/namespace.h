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

#pragma once

namespace Parser
{
extern std::list<NAMESPACEVALUEDATA*>*globalNameSpace, *localNameSpace, *rootNameSpace;
extern SymbolTable<SYMBOL>* labelSyms;
extern std::list<SYMBOL*> nameSpaceList;
extern char anonymousNameSpaceName[512];

void namespaceinit();
void unvisitUsingDirectives(NAMESPACEVALUEDATA* v);
void  tablesearchinline(std::list<SYMBOL*>&gather, const char* name, NAMESPACEVALUEDATA* ns, bool tagsOnly, bool allowUsing = false);
SYMBOL* namespacesearch(const char* name, std::list<NAMESPACEVALUEDATA*>* ns, bool qualified, bool tagsOnly);
void searchNS(SYMBOL* sym, SYMBOL* nssp, std::list<SYMBOL*>& gather);
SYMBOL* tablesearchone(const char* name, NAMESPACEVALUEDATA* ns, bool tagsOnly);
SYMBOL* gsearch(const char* name);
SYMBOL* tsearch(const char* name);
void insertNamespace( Linkage linkage, StorageClass storage_class, bool* linked);
}  // namespace Parser