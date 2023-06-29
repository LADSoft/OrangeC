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

#define CE_ERROR 1
#define CE_WARNING 2
#define CE_TRIVIALWARNING 4
#define CE_ANSIERROR 8
#define CE_ANSIWARNING 16
#define CE_CPLUSPLUSERROR 32
#define CE_NOTE 64

enum ERROR_LIST
{
#define ERRLIST(x, y, z, a) x = y,
#define ERRSCHEMA(x, y, z, a) x = y,
#define ERRWITHWARNFLAG(x, y, ...) x = y,
#define ERRWITHHELP(x, y, ...) x = y,
#define ERRWITHWARNFLAGHELP(x, y, ...) x = y,
#include "errorlist.h"
};

void diag(const char* fmt, ...);

namespace Parser
{
struct ErrorNamesAndLevels
{
    const char* name;
    int level;
};

enum _vlaTypes
{
    v_label,
    v_goto,
    v_return,
    v_branch,
    v_vla,
    v_declare,
    v_blockstart,
    v_blockend
};
typedef struct vlaShim
{
    struct vlaShim* next;
    std::list<struct vlaShim*> lower;
    struct vlaShim* fwd;
    struct vlaShim* parent;
    std::list<struct vlaShim*> backs;
    enum _vlaTypes type;
    STATEMENT* stmt;
    int level;
    int blocknum;
    int blockindex;
    int label;
    int line;
    int checkme : 1;
    int mark : 1;
    const char* file;
} VLASHIM;

extern ErrorNamesAndLevels errors[];
extern int diagcount;
extern int currentErrorLine;

extern SYMBOL* theCurrentFunc;
extern enum e_kw skim_end[];
extern enum e_kw skim_closepa[];
extern enum e_kw skim_semi[];
extern enum e_kw skim_semi_declare[];
extern enum e_kw skim_closebr[];
extern enum e_kw skim_comma[];
extern enum e_kw skim_colon[];
extern enum e_kw skim_templateend[];
extern std::deque<std::tuple<const char*, int, SYMBOL*>> instantiationList;

void DisableWarning(int num);
void EnableWarning(int num);
void WarningOnlyOnce(int num);
void WarningAsError(int num);
void WarningAsError(const char* str);
void AllWarningsAsError(void);
void AllWarningsDisable(void);
void PushWarnings(void);
void PopWarnings(void);
void DisableTrivialWarnings(void);
int TotalErrors(void);
void errorinit(void);
bool printerrinternal(int err, const char* file, int line, va_list args);
int printerr(int err, const char* file, int line, ...);
void pperror(int err, int data);
void pperrorstr(int err, const char* str);
void preverror(int err, const char* name, const char* origFile, int origLine);
void preverrorsym(int err, SYMBOL* sp, const char* origFile, int origLine);
void errorat(int err, const char* name, const char* file, int line);
void errorcurrent(int err);
void getns(char* buf, SYMBOL* nssym);
void getcls(char* buf, SYMBOL* clssym);
void errorqualified(int err, SYMBOL* strSym, NAMESPACEVALUEDATA* nsv, const char* name);
void errorNotMember(SYMBOL* strSym, NAMESPACEVALUEDATA* nsv, const char* name);
void error(int err);
void errorint(int err, int val);
void errorstr(int err, const char* val);
void errorstr2(int err, const char* val, const char* two);
void errorsym(int err, SYMBOL* sym);
void errorsym(int err, SYMBOL* sym, int line, const char* file);
void errorsym2(int err, SYMBOL* sym1, SYMBOL* sym2);
void errorstrsym(int err, const char* name, SYMBOL* sym2);
void errorstringtype(int err, char* str, TYPE* tp1);
void errortype(int err, TYPE* tp1, TYPE* tp2);
void errorabstract(int error, SYMBOL* sp);
void errorarg(int err, int argnum, SYMBOL* declsp, SYMBOL* funcsp);
void errskim(LEXLIST** lex, enum e_kw* skimlist, bool assumeTemplate = false);
void skip(LEXLIST** lex, enum e_kw kw);
bool needkw(LEXLIST** lex, enum e_kw kw);
void specerror(int err, const char* name, const char* file, int line);
void unmarkGotos(VLASHIM* shim);
void checkGotoPastVLA(std::list<STATEMENT*>* stmt, bool first);
void checkUnlabeledReferences(std::list<BLOCKDATA*>& block);
void checkUnused(SymbolTable<SYMBOL>* syms);
void findUnusedStatics(std::list<NAMESPACEVALUEDATA*>* nameSpace);
void assignmentUsages(EXPRESSION* node, bool first);
void checkDefaultArguments(SYMBOL* spi);
void EnterInstantiation(LEXLIST* lex, SYMBOL* sp);
void LeaveInstantiation();
}  // namespace Parser