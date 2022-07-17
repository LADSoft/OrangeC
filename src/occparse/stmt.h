#pragma once
/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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
extern bool isCallNoreturnFunction;

extern int inLoopOrConditional;
extern int funcNesting;
extern int funcLevel;
extern int tryLevel;

extern bool hasFuncCall;
extern bool hasXCInfo;
extern int startlab, retlab;
extern int codeLabel;
extern bool declareAndInitialize;
extern bool functionCanThrow;
extern int bodyIsDestructor;

extern Optimizer::LINEDATA *linesHead, *linesTail;

void statement_ini(bool global);
bool msilManaged(SYMBOL* s);
void InsertLineData(int lineno, int fileindex, const char* fname, char* line);
void FlushLineData(const char* file, int lineno);
STATEMENT* currentLineData(BLOCKDATA* parent, LEXLIST* lex, int offset);
STATEMENT* stmtNode(LEXLIST* lex, BLOCKDATA* parent, enum e_stmt stype);
void makeXCTab(SYMBOL* funcsp);
LEXLIST* statement_catch(LEXLIST* lex, SYMBOL* funcsp, BLOCKDATA* parent, int label, int startlab, int endlab);
LEXLIST* statement_try(LEXLIST* lex, SYMBOL* funcsp, BLOCKDATA* parent);
bool hasInlineAsm(void);
LEXLIST* statement_asm(LEXLIST* lex, SYMBOL* funcsp, BLOCKDATA* parent);
bool resolveToDeclaration(LEXLIST* lex, bool structured);
LEXLIST* statement(LEXLIST* lex, SYMBOL* funcsp, BLOCKDATA* parent, bool viacontrol);
LEXLIST* compound(LEXLIST* lex, SYMBOL* funcsp, BLOCKDATA* parent, bool first);
void assignParam(SYMBOL* funcsp, int* base, SYMBOL* param);
void parseNoexcept(SYMBOL* funcsp);
LEXLIST* body(LEXLIST* lex, SYMBOL* funcsp);
}  // namespace Parser