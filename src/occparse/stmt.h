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

#include <stack>

namespace Parser
{
struct Statement
{
    std::list<Statement*>* lower;
    std::list<Statement*>* blockTail;
    StatementNode type;
    EXPRESSION* select;
    EXPRESSION* destexp;
    Optimizer::LINEDATA* lineData;
    union
    {
        Type* tp;
        std::list<CASEDATA*>* cases;
        FunctionBlock* parent;
    };
    struct sym* sp;
    int charpos;
    int line;
    const char* file;
    int label;
    int endlabel;
    int breaklabel;  // also the label at the end of the try block
    int altlabel;
    int tryStart;
    int tryEnd;
    // inline assembly used regs
    unsigned char* assemblyRegs;
    int hasvla : 1;
    int hasdeclare : 1;
    int purelabel : 1;
    int explicitGoto : 1;
    int indirectGoto : 1;
    int blockInit : 1;
    static Statement* MakeStatement(std::list<FunctionBlock*>& parent, StatementNode stype, Lexeme *lex = nullptr);
};
struct FunctionBlock
{
    FunctionBlock* orig;
    FunctionBlock* caseDestruct;
    Keyword type;
    std::list<CASEDATA*>* cases;
    std::list<Statement*>* statements;
    std::list<Statement*>* blockTail;
    SymbolTable<struct sym>* table;
    int breaklabel;
    int continuelabel;
    int defaultlabel;
    int needlabel : 1;
    int hasbreak : 1;
    int hassemi : 1;
    int nosemi : 1; /* ok to skip semi */
    void AddThis( std::list<FunctionBlock*>& parent);
};
struct StatementGenerator
{
    StatementGenerator(SYMBOL* funcsp_in) : funcsp(funcsp_in), functionReturnType(nullptr) {}

    bool ParseAsm(std::list<FunctionBlock*>& parent);
    void Compound(std::list<FunctionBlock*>& parent, bool first);
    void FunctionBody();
    bool CompileFunctionFromStream(bool withC = false, bool now = false);
    void GenerateDeferredFunctions();
    void GenerateFunctionIntermediateCode();
    void BodyGen();
    bool CompileFunctionFromStreamInternal();

    static void SetFunctionDefine(std::string name, bool set);
    static int GetLabelValue( std::list<FunctionBlock*>* parent, Statement* st);
    static void AssignParam(SYMBOL* funcsp, int* base, SYMBOL* param);
    static void ParseNoExceptClause(SYMBOL* sp);
    static bool ResolvesToDeclaration( bool structured);
    static bool HasInlineAsm() { return Optimizer::architecture == ARCHITECTURE_X86; }
    static EXPRESSION* DestructorsForExpression(EXPRESSION* exp);
    static void DestructorsForBlock(EXPRESSION** exp, SymbolTable<SYMBOL>* table, bool mainDestruct);
    static void DestructorsForArguments(std::list<Argument*>* il);
    static void AllocateLocalContext(std::list<FunctionBlock*>& block, SYMBOL* sym, int label);
    static void FreeLocalContext(std::list<FunctionBlock*>& block, SYMBOL* sym, int label);

  protected:
    bool IsSelectTrue(EXPRESSION* exp);
    bool IsSelectFalse(EXPRESSION* exp);
    void MarkInitializersAsDeclarations(std::list<Statement*>& lst);
    void SelectionExpression(std::list<FunctionBlock*>& parent, EXPRESSION** exp, Keyword kw, bool* declaration);
    FunctionBlock* GetCommonParent(std::list<FunctionBlock*>& src, std::list<FunctionBlock*>& dest);
    void ThunkCatchCleanup(Statement* st, std::list<FunctionBlock*>& src, std::list<FunctionBlock*>& dest);
    void DestructSymbolsInTable(SymbolTable<SYMBOL>* syms);
    void ThunkReturnDestructors(EXPRESSION** exp, SymbolTable<SYMBOL>* top, SymbolTable<SYMBOL>* syms);
    void ThunkGotoDestructors(EXPRESSION** exp, std::list<FunctionBlock*>& gotoTab, std::list<FunctionBlock*>& labelTab);
    void StartOfCaseGroup(std::list<FunctionBlock*>& parent);
    void EndOfCaseGroup(std::list<FunctionBlock*>& parent);
    void HandleStartOfCase(std::list<FunctionBlock*>& parent);
    void HandleEndOfCase(std::list<FunctionBlock*>& parent);
    void HandleEndOfSwitchBlock(std::list<FunctionBlock*>& parent);
    void ParseBreak(std::list<FunctionBlock*>& parent);
    void ParseCase(std::list<FunctionBlock*>& parent);
    void ParseContinue(std::list<FunctionBlock*>& parent);
    void ParseDefault(std::list<FunctionBlock*>& parent);
    void ParseDo(std::list<FunctionBlock*>& parent);
    void ParseFor(std::list<FunctionBlock*>& parent);
    void ParseIf(std::list<FunctionBlock*>& parent);
    void ParseGoto(std::list<FunctionBlock*>& parent);
    void ParseLabel(std::list<FunctionBlock*>& parent);
    EXPRESSION* ConvertReturnToRef(EXPRESSION* exp, Type* tp, Type* boundTP);
    void MatchReturnTypes(Type* tp1, Type* tp2);
    void AdjustForAutoReturnType(Type* tp1, EXPRESSION* exp1);
    void ParseReturn(std::list<FunctionBlock*>& parent);
    void ParseSwitch(std::list<FunctionBlock*>& parent);
    void ParseWhile(std::list<FunctionBlock*>& parent);
    bool NoSideEffect(EXPRESSION* exp);
    void ParseExpr(std::list<FunctionBlock*>& parent);
    void AsmDeclare();
    void ParseCatch(std::list<FunctionBlock*>& parent, int label, int startlab, int endlab);
    void ParseTry(std::list<FunctionBlock*>& parent);
    void AssignInReverse(std::list<Statement*>* current, EXPRESSION** exp);
    void AutoDeclare(Type** tp, EXPRESSION** exp, std::list<FunctionBlock*>& parent, int asExpression);
    void StatementWithoutNonconst(std::list<FunctionBlock*>& parent, bool viacontrol);
    bool ThunkReturnInMain(std::list<FunctionBlock*>& parent, bool always);
    void ReturnThIsPtr(std::list<Statement*>* st, EXPRESSION* thisptr);
    bool IsReturnTypeVoid(Type* tp);
    void AssignCParams(int* base, SymbolTable<SYMBOL>* params, Type* rv, std::list<FunctionBlock*>& block);
    void AssignPascalParams(int* base, SymbolTable<SYMBOL>* params, Type* rv, std::list<FunctionBlock*>& block);
    void AssignParameterSizes(std::list<FunctionBlock*>& block);
    int CountInlineStatements(std::list<Statement*>* block);
    void HandleInlines();
    void SingleStatement(std::list<FunctionBlock*>& parent, bool viacontrol);
    static void TagSyms(SymbolTable<SYMBOL>* syms);

  private:
    SYMBOL* funcsp;
    Type* functionReturnType;
};

extern bool isCallNoreturnFunction;

extern int processingLoopOrConditional;
extern int funcNesting;
extern int funcNestingLevel;
extern int tryLevel;

extern bool hasFuncCall;
extern bool hasXCInfo;
extern int startlab, retlab;
extern int codeLabel;
extern bool declareAndInitialize;
extern bool functionCanThrow;
extern int bodyIsDestructor;
extern bool inFunctionExpressionParsing;

extern std::list<Optimizer::LINEDATA*>* lines;
extern std::list<FunctionBlock*> emptyBlockdata;

extern std::stack<std::list<FunctionBlock*>*> expressionStatements;
extern std::deque<std::pair<EXPRESSION*, Type*>> expressionReturns;

void statement_ini(bool global);
bool msilManaged(SYMBOL* s);
}  // namespace Parser