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
extern NAMESPACEVALUELIST *globalNameSpace, *localNameSpace;
extern HASHTABLE* labelSyms;

extern HASHTABLE* CreateHashTable(int size);
extern int matchOverloadLevel;

HASHTABLE* CreateHashTable(int size);
void syminit(void);
HASHTABLE* CreateHashTable(int size);
void AllocateLocalContext(BLOCKDATA* block, SYMBOL* sym, int label);
void TagSyms(HASHTABLE* syms);
void FreeLocalContext(BLOCKDATA* block, SYMBOL* sym, int label);
SYMLIST** GetHashLink(HASHTABLE* t, const char* string);
SYMLIST* AddName(SYMBOL* item, HASHTABLE* table);
SYMLIST* AddOverloadName(SYMBOL* item, HASHTABLE* table);
SYMLIST** LookupName(const char* name, HASHTABLE* table);
SYMBOL* search(const char* name, HASHTABLE* table);
bool matchOverload(TYPE* tnew, TYPE* told, bool argsOnly);
SYMBOL* searchOverloads(SYMBOL* sym, HASHTABLE* table);
SYMBOL* gsearch(const char* name);
SYMBOL* tsearch(const char* name);
void baseinsert(SYMBOL* in, HASHTABLE* table);
void insert(SYMBOL* in, HASHTABLE* table);
void insertOverload(SYMBOL* in, HASHTABLE* table);
}  // namespace Parser