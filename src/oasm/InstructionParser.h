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

#ifndef InstructionParser_h
#define InstructionParser_h

#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <cstdlib>
#include <climits>
#include "AsmExpr.h"
#include <memory>
#include <set>
#include "AdlStructures.h"

class Instruction;
class Section;
class AsmFile;

namespace Optimizer
{
class SimpleExpression;
}
struct ocode;  // compiler support
struct expr;   // compiler support
struct amode;  // compiler support

class InstructionParser
{
  public:
    InstructionParser() : asmexpr(nullptr), eol(false), id(0), val(nullptr), tokenPos(0), numeric(nullptr), attSyntax(false) {}

    static InstructionParser* GetInstance();

    void SetATT(bool att);

    bool MatchesOpcode(std::string opcode);
    std::unordered_map<std::string, int>::iterator GetOpcode(const std::string& opcode, int& size1, int& size2);

    Instruction* Parse(const std::string& args, int PC);
    virtual void Setup(Section* sect) = 0;
    virtual void Init() = 0;
    virtual bool ParseSection(AsmFile* fil, Section* sect) = 0;
    virtual bool ParseDirective(AsmFile* fil, Section* sect) = 0;
    virtual std::string ParsePreInstruction(const std::string& op, bool doParse);
    virtual bool IsBigEndian() = 0;
    bool SetNumber(int tokenPos, int oldVal, int newVal);
    static bool SetProcessorMode(int mode)
    {
        if (mode == 16 || mode == 32 || mode == 64)
        {
            processorMode = mode;
            return true;
        }
        return false;
    }
    static const std::set<std::string>& GetAttExterns() { return attPotentialExterns; }

  protected:
    bool ParseNumber(int relOfs, int sign, int bits, int needConstant, int tokenPos);
    void Split(const std::string& line, std::vector<std::string>& splt);
    void PreprendSize(std::string& val, int sz);
    std::string RewriteATTArg(const std::string& line);
    std::string RewriteATT(int& op, const std::string& line, int& size1, int& size2);
    bool Tokenize(int& op, int PC, int& size1, int& size2);
    enum
    {
        TOKEN_BASE = 0,
        REGISTER_BASE = 1000
    };
    virtual asmError DispatchOpcode(int opcode) = 0;
    void InsertTokens(std::string&& line, int PC, bool hasBrackets = true);
    enum
    {
        TK_ID = INT_MIN,
        TK_REG = INT_MIN + 1,
        TK_NUMERIC = INT_MIN + 2,
    };
    std::string token;
    std::string line;
    bool eol;
    int id;
    AsmExprNode* val;
    int tokenPos;
    Numeric* numeric;
    std::unordered_map<std::string, int> tokenTable;
    std::unordered_map<std::string, int> opcodeTable;
    std::unordered_map<std::string, int> prefixTable;
    std::list<Numeric*> operands;
    std::list<Coding*> CleanupValues;
    std::list<int> prefixes;
    std::vector<InputToken*> inputTokens;
    BitStream bits;
    AsmExpr asmexpr;
    bool attSyntax;
    static int processorMode;
    static std::set<std::string> attPotentialExterns;

    // c compiler support

  public:
    std::string FormatInstruction(ocode* ins);
    asmError GetInstruction(ocode* ins, Instruction*& newIns, std::list<Numeric*>& operands);

  protected:
    void SetRegToken(int reg, int sz);
    void SetNumberToken(int val);
    bool SetNumberToken(struct Optimizer::SimpleExpression* offset, int& n);
    void SetExpressionToken(struct Optimizer::SimpleExpression* offset);
    void SetSize(int sz);
    void SetBracketSequence(bool open, int sz, int seg);
    void SetOperandTokens(amode* operand);
    void SetTokens(ocode* ins);
};
#endif
