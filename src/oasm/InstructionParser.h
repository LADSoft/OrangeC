/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
#include <map>
#include <list>
#include <vector>
#include <stdlib.h>
#include <climits>
#include "AsmExpr.h"
#include <memory>
class Instruction;
class Section;
class AsmFile;

struct ocode;  // compiler support
struct expr;   // compiler support
struct amode;  // compiler support

enum asmError
{
    AERR_NONE,
    AERR_SYNTAX,
    AERR_OPERAND,
    AERR_BADCOMBINATIONOFOPERANDS,
    AERR_UNKNOWNOPCODE,
    AERR_INVALIDINSTRUCTIONUSE
};

class Coding
{
  public:
#ifdef DEBUG
    std::string name;
#endif
    enum Type
    {
        eot,
        bitSpecified = 1,
        valSpecified = 2,
        fieldSpecified = 4,
        indirect = 8,
        reg = 16,
        stateFunc = 32,
        stateVar = 64,
        number = 128,
        optional = 256,
        native = 512,
        illegal = 1024
    } type;
    int val;
    unsigned char bits;
    unsigned char field;
    char unary;
    char binary;
};
#ifdef DEBUG
#    define CODING_NAME(x) x,
#else
#    define CODING_NAME(x)
#endif

class Numeric
{
  public:
    Numeric(AsmExprNode* n) : node(n) {}
    Numeric() : node(nullptr) {}
    AsmExprNode* node;
    int pos = 0;
    int relOfs = 0;
    int size = 0;
    int used = 0;
};
class BitStream
{
  public:
    BitStream() { Reset(); }

    void Reset()
    {
        bits = 0;
        memset(bytes, 0, sizeof(bytes));
    }
    void Add(int val, int bits);
    int GetBits() { return bits; }
    void GetBytes(unsigned char* dest, int size) { memcpy(dest, bytes, size < (bits + 7) / 8 ? size : (bits + 7) / 8); }

  private:
    int bits;
    unsigned char bytes[32];
};
class InputToken
{
  public:
    enum
    {
        TOKEN,
        REGISTER,
        NUMBER,
        LABEL
    } type;
    AsmExprNode* val;
};
class InstructionParser
{
  public:
    InstructionParser() : asmexpr(nullptr) {}

    static InstructionParser* GetInstance();

    bool MatchesOpcode(std::string opcode);

    Instruction* Parse(const std::string args, int PC);
    virtual void Setup(Section* sect) = 0;
    virtual void Init() = 0;
    virtual bool ParseSection(AsmFile* fil, Section* sect) = 0;
    virtual bool ParseDirective(AsmFile* fil, Section* sect) = 0;
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
  protected:
    void RenameRegisters(AsmExprNode* val);
    AsmExprNode* ExtractReg(AsmExprNode** val);
    bool MatchesTimes(AsmExprNode* val);
    AsmExprNode* ExtractTimes(AsmExprNode** val);
    bool CheckRegs(AsmExprNode* val);
    void ParseNumeric(int PC);
    bool ParseNumber(int relOfs, int sign, int bits, int needConstant, int tokenPos);
    bool Tokenize(int PC);
    bool IsNumber();
    enum
    {
        TOKEN_BASE = 0,
        REGISTER_BASE = 1000
    };
    virtual asmError DispatchOpcode(int opcode) = 0;
    void NextToken(int PC);
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
    std::map<std::string, int> tokenTable;
    std::map<std::string, int> opcodeTable;
    std::map<std::string, int> prefixTable;
    std::list<Numeric*> operands;
    std::list<Coding*> CleanupValues;
    std::list<int> prefixes;
    std::vector<InputToken*> inputTokens;
    BitStream bits;
    AsmExpr asmexpr;
    static int processorMode;
    // c compiler support

  public:
    std::string FormatInstruction(ocode* ins);
    asmError GetInstruction(ocode* ins, Instruction*& newIns, std::list<Numeric*>& operands);

  protected:
    void SetRegToken(int reg, int sz);
    void SetNumberToken(int val);
    bool SetNumberToken(expr* offset, int& n);
    void SetExpressionToken(expr* offset);
    void SetSize(int sz);
    void SetBracketSequence(bool open, int sz, int seg);
    void SetOperandTokens(amode* operand);
    void SetTokens(ocode* ins);
};
#endif
