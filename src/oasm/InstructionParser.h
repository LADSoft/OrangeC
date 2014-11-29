/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#ifndef InstructionParser_h
#define InstructionParser_h

#include <string>
#include <map>
#include <deque>
#include <vector>
#include <stdlib.h>
#include "AsmExpr.h"

class Instruction;
class Section;
class AsmFile;

class Coding
{
public:
    enum Type { eot, bitSpecified=1, valSpecified=2, fieldSpecified = 4, indirect=8, 
            reg= 16, stateFunc = 32, stateVar = 64, number = 128, native = 256, illegal= 512 } type;
    int val;
    int bits;
    int field;
    int math;
    int mathval;
};
class CodingHelper
{
public:
    CodingHelper() : bits(8), math(0), mathval(0), field(-1) { }
    CodingHelper(const CodingHelper &old)
    {
        bits = old.bits;
        math = old.math;
        mathval = old.mathval;
        field = old.field;
    }
    CodingHelper &operator = (const CodingHelper &old)
    {
        bits = old.bits;
        math = old.math;
        mathval = old.mathval;
        field = old.field;
        return *this;
    }
    int DoMath(int val);
    int bits;
    int math;
    int mathval;
    int field;
};
class Numeric
{
public:
    Numeric(): node(NULL) { }
    AsmExprNode *node;
    int pos;
    int relOfs;
    int size;
    int used;
};
class BitStream
{
public:
    BitStream() { Reset(); }
    
    void Reset() { bits = 0; memset(bytes, 0, sizeof(bytes)); }
    void Add(int val, int bits);
    int GetBits() { return bits; }
    void GetBytes(unsigned char *dest, int size) { memcpy(dest, bytes, size < (bits + 7)/8 ? size : (bits + 7)/8); }
private:
    int bits;
    unsigned char bytes[32];
};
class InputToken
{ 
public:
    enum { TOKEN, REGISTER, NUMBER, LABEL } type;
    AsmExprNode *val;
};
class InstructionParser
{
public:
    InstructionParser() :expr(NULL) { }
    
    static InstructionParser *GetInstance();
    
    bool MatchesOpcode(std::string opcode);
    
    Instruction *Parse(const std::string args, int PC);
    virtual void Setup(Section *sect) = 0;
    virtual void Init() = 0;	
    virtual bool ParseSection(AsmFile *fil, Section *sect) = 0;
    virtual bool IsBigEndian() = 0;
    bool SetNumber(int tokenPos, int oldVal, int newVal);
protected:
    void RenameRegisters(AsmExprNode *val);
    AsmExprNode *ExtractReg(AsmExprNode **val);
    bool MatchesTimes(AsmExprNode *val);
    AsmExprNode *ExtractTimes(AsmExprNode **val);
    bool CheckRegs(AsmExprNode *val);
    void ParseNumeric(int PC);
    bool ParseNumber(int relOfs, int sign, int bits, int needConstant, int tokenPos);
    bool Tokenize(int PC);
    bool IsNumber() ;
    enum
    {
        TOKEN_BASE = 0,
        REGISTER_BASE = 1000
    };
    virtual bool DispatchOpcode(int opcode) = 0;
    void NextToken(int PC);
    enum {
        TK_ID = 0x80000000,
        TK_REG = 0x80000001,
        TK_NUMERIC= 0x80000002,
    };
    std::string token;
    std::string line;
    bool eol;
    int id;
    AsmExprNode *val;
    int tokenPos;
    Numeric *numeric;
    struct lt {
        bool operator () (const std::string &left, const std::string &right) const
        {
            return stricmp(left.c_str(), right.c_str()) < 0;
        }
    } ;
    std::map<std::string, int,lt> tokenTable;
    std::map<std::string, int,lt> opcodeTable;
    std::map<std::string, int,lt> prefixTable;
    std::deque<Numeric *> operands;
    std::deque<Coding *>CleanupValues;
    std::deque<int>prefixes;
    std::vector<InputToken *>inputTokens;
    BitStream bits;
    AsmExpr expr;
};
#endif
