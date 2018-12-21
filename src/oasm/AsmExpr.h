/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
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

#ifndef AsmExpr_h
#define AsmExpr_h

#include <string>

#include "Token.h"
class ppDefine;
class Section;

typedef long long PPINT;

class AsmExprNode
{
  public:
    enum Type
    {
        IVAL,
        FVAL,
        LABEL,
        ADD,
        SUB,
        NEG,
        NOT,
        CMPL,
        MUL,
        DIV,
        SDIV,
        MOD,
        SMOD,
        LSHIFT,
        RSHIFT,
        GT,
        LT,
        GE,
        LE,
        EQ,
        NE,
        OR,
        XOR,
        AND,
        LAND,
        LOR,
        PC,
        SECTBASE,
        BASED,
        REG
    };
    AsmExprNode(Type xType, AsmExprNode* Left = nullptr, AsmExprNode* Right = nullptr) :
        type(xType),
        ival(0),
        left(Left),
        right(Right),
        sect(nullptr)
    {
    }
    AsmExprNode(PPINT Ival, bool reg = false) : type(reg? REG : IVAL), ival(Ival), left(nullptr), right(nullptr), sect(nullptr) {}
    AsmExprNode(const FPF& Fval) : type(FVAL), ival(0), fval(Fval), left(nullptr), right(nullptr), sect(nullptr) {}
    AsmExprNode(std::string lbl) : type(LABEL), ival(0), left(nullptr), right(nullptr), label(lbl), sect(nullptr) {}
    AsmExprNode(Section* Sect, int offs) : type(BASED), ival(offs), left(nullptr), right(nullptr), sect(Sect) {}
    AsmExprNode(const AsmExprNode& old)
    {
        fval = old.fval;
        ival = old.ival;
        label = old.label;
        type = old.type;
        left = old.left;
        right = old.right;
        sect = old.sect;
    }
    ~AsmExprNode()
    {
        if (left)
            delete left;
        if (right)
            delete right;
    }
    FPF fval;
    PPINT ival;
    std::string label;
    AsmExprNode* GetLeft() { return left; }
    void SetLeft(AsmExprNode* n) { left = n; }
    AsmExprNode* GetRight() { return right; }
    void SetRight(AsmExprNode* n) { right = n; }
    Section* GetSection() { return sect; }
    Type GetType() { return type; }
    void SetType(Type tType) { type = tType; }
    bool IsAbsolute();

  protected:
    bool IsAbsoluteInternal(int& level);

  private:
    Type type;
    AsmExprNode* left;
    AsmExprNode* right;
    Section* sect;
};
class AsmExpr
{
  public:
    AsmExpr() : define(nullptr) { }
    AsmExpr(ppDefine* Define) : define(Define) { InitHash(); }
    ~AsmExpr() {}
    AsmExprNode* Build(std::string& line);
    static void ReInit();
    static AsmExprNode* Eval(AsmExprNode* n, int PC);
    std::string GetString() { return tokenizer->GetString(); }
    static void SetCurrentLabel(std::string lbl) { currentLabel = lbl; }
    static void SetSection(Section* Sect) { section = Sect; }
    static Section* GetSection() { return section; }
    static void SetEqu(std::string name, AsmExprNode* n) { equs[name] = n; }
    static AsmExprNode* ConvertToBased(AsmExprNode* n, int pc);
    static AsmExprNode* GetEqu(std::string name)
    {
        auto it = equs.find(name);
        if (it != equs.end())
        {
            return it->second;
        }
        else
        {
            return nullptr;
        }
    }

  protected:
    AsmExprNode* primary();
    AsmExprNode* unary();
    AsmExprNode* multiply();
    AsmExprNode* add();
    AsmExprNode* shift();
    AsmExprNode* relation();
    AsmExprNode* equal();
    AsmExprNode* and_();
    AsmExprNode* xor_();
    AsmExprNode* or_();
    AsmExprNode* logicaland();
    AsmExprNode* logicalor();

  private:
    static void InitHash();
    ppDefine* define;
    Tokenizer* tokenizer;
    const Token* token;
    static KeywordHash hash;
    static bool initted;
    static std::string currentLabel;
    static Section* section;
    static std::map<std::string, AsmExprNode*> equs;
};
#endif