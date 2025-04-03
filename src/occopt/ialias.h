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

namespace Optimizer
{
extern int cachedTempCount;
extern BITINT* uivBytes;
extern BITINT* processBits;
struct UIVHash
{
    struct UIVHash* next;
    ALIASNAME* name;
    int offset;
    ALIASNAME* result;
};

void AliasInit(void);
void AliasRundown(void);
void AliasStruct(BITINT* bits, IMODE* ans, IMODE* left, IMODE* right);
void AliasGosub(QUAD* tail, BITINT* parms, BITINT* bits, int n);
void AliasUses(BITINT* bits, IMODE* im, bool rhs);
void AliasPass1(void);
void AliasPass2(void);
}  // namespace Optimizer