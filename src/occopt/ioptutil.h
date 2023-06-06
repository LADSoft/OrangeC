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

namespace Optimizer
{
extern BITINT bittab[BITINTBITS];

void BitInit(void);
BRIGGS_SET* briggsAlloc(int size);
BRIGGS_SET* briggsAlloct(int size);
BRIGGS_SET* briggsAllocc(int size);
BRIGGS_SET* briggsAllocs(int size);
BRIGGS_SET* briggsReAlloc(BRIGGS_SET* set, int size);
int briggsSet(BRIGGS_SET* p, int index);
int briggsReset(BRIGGS_SET* p, int index);
int briggsTest(BRIGGS_SET* p, int index);
int briggsUnion(BRIGGS_SET* s1, BRIGGS_SET* s2);
int briggsIntersect(BRIGGS_SET* s1, BRIGGS_SET* s2);
}  // namespace Optimizer