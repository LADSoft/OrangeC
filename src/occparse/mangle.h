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

#define MANGLE_DEFERRED_TYPE_CHAR '{'
#define IT_THRESHOLD 2
#define IT_OV_THRESHOLD 2
#define IT_SIZE (sizeof(cpp_funcname_tab) / sizeof(char*))

#define MAX_MANGLE_NAME_COUNT 36


namespace Parser
{
extern char hashdNames[MAX_MANGLE_NAME_COUNT][256];

extern const char* overloadNameTab[];
extern const char* msiloverloadNameTab[];

extern const char* overloadXlateTab[];
extern char mangledNames[MAX_MANGLE_NAME_COUNT][256];
extern int mangledNamesCount;

void mangleInit(void);
char* mangleNameSpaces(char* in, SYMBOL* sym);

char* mangleType(char* in, int len, Type* tp, bool first);
void GetClassKey(char* buf, SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* params);
void SetLinkerNames(SYMBOL* sym, Linkage linkage, bool isTemplateDefinition = false);
}  // namespace Parser