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

#include "CmdSwitch.h"

extern Optimizer::SimpleSymbol* currentFunction;

namespace Parser
{
extern int anonymousNotAlloc;
}
namespace Optimizer
{

extern CmdSwitchParser SwitchParser;
extern CmdSwitchBool single;
extern CmdSwitchBool WriteIcdFile;
extern CmdSwitchCombineString output;

extern const char* usageText;
extern const char* helpText;

extern int usingEsp;

void InternalConflict(QUAD* head);
void PreColor(QUAD* head);
void FastcallColor(QUAD* head);
void examine_icode(QUAD* head);
int PreRegAlloc(QUAD* tail, BriggsSet* globalVars, BriggsSet* eobGlobals, int pass);
void Optimize(SimpleSymbol* funcsp);
}  // namespace Optimizer