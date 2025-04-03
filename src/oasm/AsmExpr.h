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

#ifndef AsmExpr_h
#define AsmExpr_h

#include <string>
#include <memory>
#include "AdlStructures.h"

#include "Token.h"
class ppDefine;
class Section;

typedef long long PPINT;

class AsmExprNode : public AdlExprNode
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
    AsmExprNode(Type xType, std::shared_ptr<AsmExprNode> Left = nullptr, std::shared_ptr<AsmExprNode> Right = nullptr) :
        AdlExprNode(0), type(xType), left(Left), right(Right), sect(nullptr)
    {
    }
    AsmExprNode(PPINT Ival, bool reg = false) :
        AdlExprNode(Ival), type(reg ? REG : IVAL), sect(nullptr), left(nullptr), right(nullptr)
    {
    }
    AsmExprNode(const FPF& Fval) : AdlExprNode(0), type(FVAL), fval(Fval), sect(nullptr), left(nullptr), right(nullptr) {}
    AsmExprNode(std::string lbl) : AdlExprNode(0), type(LABEL), label(lbl), sect(nullptr), left(nullptr), right(nullptr) {}
    AsmExprNode(std::shared_ptr<Section>& Sect, int offs) :
        AdlExprNode(offs), type(BASED), sect(Sect), left(nullptr), right(nullptr)
    {
    }
    AsmExprNode(const AsmExprNode& old) : AdlExprNode(old)
    {
        fval = old.fval;
        label = old.label;
        type = old.type;
        left = old.left;
        right = old.right;
        sect = old.sect;
    }
    ~AsmExprNode() {}
    FPF fval;
    std::string label;
    std::shared_ptr<AsmExprNode> GetLeft() { return left; }
    void SetLeft(std::shared_ptr<AsmExprNode> n) { left = n; }
    std::shared_ptr<AsmExprNode> GetRight() { return right; }
    void SetRight(std::shared_ptr<AsmExprNode> n) { right = n; };
    std::shared_ptr<Section> GetSection() { return sect; }
    Type GetType() { return type; }
    void SetType(Type tType) { type = tType; }
    bool IsAbsolute();

  protected:
    bool IsAbsoluteInternal(int& level);

  private:
    Type type;
    std::shared_ptr<AsmExprNode> left;
    std::shared_ptr<AsmExprNode> right;
    std::shared_ptr<Section> sect;
};
class AsmExpr
{
  public:
    AsmExpr() : define(nullptr), token(nullptr) {}
    AsmExpr(ppDefine* Define) : define(Define), token(nullptr) {}
    ~AsmExpr() {}
    std::shared_ptr<AsmExprNode> Build(std::string& line);
    static void ReInit();
    static std::shared_ptr<AsmExprNode> Eval(std::shared_ptr<AsmExprNode> n, int PC);
    std::string GetString() { return tokenizer->GetString(); }
    static void SetCurrentLabel(std::string lbl) { currentLabel = lbl; }
    static void SetSection(std::shared_ptr<Section>& Sect) { section = Sect; }
    static std::shared_ptr<Section> GetSection() { return section; }
    static void SetEqu(std::string name, std::shared_ptr<AsmExprNode>& n) { equs[name] = n; }
    static std::shared_ptr<AsmExprNode> ConvertToBased(std::shared_ptr<AsmExprNode>& n, int pc);

    static std::shared_ptr<AsmExprNode> GetEqu(std::string& name)
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
    std::shared_ptr<AsmExprNode> primary();
    std::shared_ptr<AsmExprNode> unary();
    std::shared_ptr<AsmExprNode> multiply();
    std::shared_ptr<AsmExprNode> add();
    std::shared_ptr<AsmExprNode> shift();
    std::shared_ptr<AsmExprNode> relation();
    std::shared_ptr<AsmExprNode> equal();
    std::shared_ptr<AsmExprNode> and_();
    std::shared_ptr<AsmExprNode> xor_();
    std::shared_ptr<AsmExprNode> or_();
    std::shared_ptr<AsmExprNode> logicaland();
    std::shared_ptr<AsmExprNode> logicalor();

  private:
    ppDefine* define;
    std::unique_ptr<Tokenizer> tokenizer;
    const Token* token;
    static KeywordHash hash;
    static std::string currentLabel;
    static std::shared_ptr<Section> section;
    static std::unordered_map<std::string, std::shared_ptr<AsmExprNode>> equs;
};
#endif