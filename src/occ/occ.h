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

void diag(const char*, ...);
extern int usingEsp;
extern Optimizer::SimpleSymbol* currentFunction;
void regInit(void);

namespace Parser
{
extern char outFile[260];
extern InstructionParser* instructionParser;
}  // namespace Parser
namespace occx86
{
extern char infile[260];

void outputfile(char* buf, int len, const char* name, const char* ext, bool obj);
void global(Optimizer::SimpleSymbol* sym, int flags);
}  // namespace occx86