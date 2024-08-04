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

void regInit(void);
namespace Optimizer
{
extern int maxAddr;
extern int lc_maxauto;

typedef struct _spill_
{
    struct _spill_* next;
    IMODE* imode;
    LIST* uses;
} SPILL;

void LoadSaturationBounds(void);
void alloc_init(void);
void AllocateStackSpace(int begin);
void FillInPrologue(QUAD* head, SimpleSymbol* funcsp);
int SqueezeChange(int temp, int t, int delta);
void SqueezeInit(void);
void Precolor(bool optimized);
void Fastcallcolor(void);
void retemp(void);
void AllocateRegisters(QUAD* head);
}  // namespace Optimizer