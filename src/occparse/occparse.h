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

#ifndef ORANGE_NO_INASM
#    include "InstructionParser.h"
#endif
#include "PreProcessor.h"

namespace Parser
{
extern char outFile[260];
extern unsigned identityValue;
extern int maxBlocks, maxTemps;
extern char cppfile[256];
extern FILE* cppFile;
extern char infile[256];
extern int fileIndex;

extern FILE* errFile;
extern PreProcessor* preProcessor;

extern char realOutFile[260];

#ifndef ORANGE_NO_INASM
extern InstructionParser* instructionParser;
#endif
extern int usingEsp;

extern Optimizer::COMPILER_PARAMS cparams_default;

long long ParseExpression(std::string& line);
int natural_size(EXPRESSION* exp);
void MakeStubs(void);
void compile(bool global);
void enter_filename(const char* name);
}  // namespace Parser
