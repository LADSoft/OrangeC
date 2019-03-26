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

#ifndef ppExpr_h
#define ppExpr_h

#include <string>

#include "Token.h"
class ppDefine;

typedef long long PPINT;

enum kw
{
    openpa,
    closepa,
    plus,
    minus,
    lnot,
    bcompl,
    star,
    divide,
    mod,
    leftshift,
    rightshift,
    gt,
    lt,
    geq,
    leq,
    eq,
    ne,
    bor,
    bxor,
    band,
    land,
    lor,
    hook,
    colon,
    comma
};
class ppExpr
{
  public:
    ppExpr(bool isunsignedchar) : define(nullptr), unsignedchar(isunsignedchar) { }
    ~ppExpr() {}

    void SetParams(ppDefine* Define) { define = Define; }
    PPINT Eval(std::string& line);
    std::string GetString() { return tokenizer->GetString(); }
    void SetDefine(ppDefine* Define) { define = Define; }
    static KeywordHash* GetHash() { return &hash; }

  protected:
    PPINT primary(std::string& line);
    PPINT unary(std::string& line);
    PPINT multiply(std::string& line);
    PPINT add(std::string& line);
    PPINT shift(std::string& line);
    PPINT relation(std::string& line);
    PPINT equal(std::string& line);
    PPINT and_(std::string& line);
    PPINT xor_(std::string& line);
    PPINT or_(std::string& line);
    PPINT logicaland(std::string& line);
    PPINT logicalor(std::string& line);
    PPINT conditional(std::string& line);
    PPINT comma_(std::string& line);

  private:
    bool unsignedchar;
    ppDefine* define;
    std::unique_ptr<Tokenizer> tokenizer;
    const Token* token;
    static KeywordHash hash;
};
#endif