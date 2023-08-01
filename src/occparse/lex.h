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

#define MAX_LOOKBACK 1024
namespace Parser
{
extern Optimizer::LINEDATA nullLineData;
extern int eofLine;
extern const char* eofFile;
extern bool parsingPreprocessorConstant;
extern LEXCONTEXT* context;
extern int charIndex;
extern SymbolTable<KEYWORD>* kwSymbols;
extern LEXLIST* currentLex;

void lexini(void);
KEYWORD* searchkw(const unsigned char** p);
LEXLIST* SkipToNextLine(void);
LEXLIST* getGTSym(LEXLIST* in);
void SkipToEol();
bool AtEol();
void CompilePragma(const unsigned char** linePointer);
LEXLIST* getsym(void);
LEXLIST* prevsym(LEXLIST* lex);
LEXLIST* backupsym(void);
LEXLIST* SetAlternateLex(LEXLIST* lexList);
bool CompareLex(LEXLIST* left, LEXLIST* right);
void SetAlternateParse(bool set, const std::string& val);
long long ParseExpression(std::string& line);
}  // namespace Parser