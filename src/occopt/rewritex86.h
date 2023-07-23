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
extern int uses_substack;
void rewrite_x86_init();
IMODE* rwSetSymbol(const char* name, bool unused);
void insert_parm(QUAD* head, QUAD* q);
void insert_nullparmadj(QUAD* head, int v);
void x86FastcallColor(QUAD* head);
void x86PreColor(QUAD* head);
void ProcessOneInd(SimpleExpression* match, SimpleExpression** ofs1, SimpleExpression** ofs2, SimpleExpression** ofs3, int* scale);
void ProcessInd(SimpleExpression** ofs1, SimpleExpression** ofs2, SimpleExpression** ofs3, int* scale);
int x86PreRegAlloc(QUAD* ins, BRIGGS_SET* globalVars, BRIGGS_SET* eobGlobals, int pass);
int x86_examine_icode(QUAD* head);
void x86InternalConflict(QUAD* head);
}  // namespace Optimizer