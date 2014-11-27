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
#ifndef AsmExpr_h
#define AsmExpr_h

#include <string>

#include "Token.h"
class ppDefine;
class Section;

typedef L_INT PPINT;

class AsmExprNode
{
public:
    enum Type { IVAL, FVAL, LABEL, ADD, SUB, NEG, NOT, CMPL, MUL, DIV, SDIV, MOD, SMOD, LSHIFT,
                RSHIFT, GT, LT, GE, LE, EQ, NE, OR, XOR, AND, LAND, LOR, PC, SECTBASE, BASED, REG };
    AsmExprNode(Type xType, AsmExprNode *Left = NULL, AsmExprNode *Right = NULL) :
                type(xType), ival(0), left(Left), right(Right), sect(NULL) { }
    AsmExprNode(PPINT Ival) : type(IVAL), ival(Ival), left(NULL), right(NULL), sect(NULL) { }
    AsmExprNode(const FPF &Fval) : type(FVAL), ival(0), fval(Fval), left(NULL), right(NULL), sect(NULL) { }
    AsmExprNode(std::string lbl) : type(LABEL), ival(0), left(NULL), right(NULL), label(lbl), sect(NULL){ }
    AsmExprNode(Section *Sect, int offs) : type(BASED), ival(offs), left(NULL), right(NULL), sect(Sect){ }
    AsmExprNode(const AsmExprNode &old)
    {
        fval = old.fval;
        ival = old.ival;
        label = old.label;
        type = old.type;
        left = old.left;
        right = old.right;
        sect = old.sect;
    }
    ~AsmExprNode() { if (left) delete left; if (right) delete right; }
    FPF fval;
    PPINT ival;
    std::string label;
    AsmExprNode *GetLeft() { return left; }
    void SetLeft(AsmExprNode *n) { left = n; }
    AsmExprNode *GetRight() { return right; }
    void SetRight(AsmExprNode *n) { right = n; }
    Section *GetSection() { return sect; }
    Type GetType() { return type; }
    void SetType(Type tType) { type = tType; }
    bool IsAbsolute();
protected:
    bool IsAbsoluteInternal(int &level);
private:
    Type type;
    AsmExprNode *left;
    AsmExprNode *right;
    Section *sect;
} ;
class AsmExpr
{
public:
    AsmExpr(ppDefine *Define) : define(Define)   { InitHash(); }
    ~AsmExpr() { }
    AsmExprNode *Build(std::string &line);
    static void ReInit() ;
    static AsmExprNode *Eval(AsmExprNode *n, int PC);
    std::string GetString() { return tokenizer->GetString(); }
    static void SetCurrentLabel(std::string lbl) { currentLabel = lbl; }
    static void SetSection(Section *Sect) { section=Sect; }
    static Section *GetSection() { return section; }
    static void SetEqu(std::string name, AsmExprNode *n) { equs[name] = n; }
    static AsmExprNode *ConvertToBased(AsmExprNode *n, int pc);
    static AsmExprNode *GetEqu(std::string name)
    {
        std::map<std::string, AsmExprNode *>::iterator it = equs.find(name);
        if (it != equs.end())
        {
            return it->second;
        }
        else
        {
            return NULL;
        }
            
    }
protected:
    AsmExprNode *primary();
    AsmExprNode *unary();
    AsmExprNode *multiply();
    AsmExprNode *add();
    AsmExprNode *shift();
    AsmExprNode *relation();	
    AsmExprNode *equal();
    AsmExprNode *and_();
    AsmExprNode *xor_();
    AsmExprNode *or_();
    AsmExprNode *logicaland();
    AsmExprNode *logicalor();

private:
    static void InitHash();
    ppDefine *define;
    Tokenizer *tokenizer;
    const Token *token;
    static KeywordHash hash;
    static bool initted;
    static std::string currentLabel;
    static Section *section;
    static std::map<std::string, AsmExprNode *> equs;
} ;
#endif