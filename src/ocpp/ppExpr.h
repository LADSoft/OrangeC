/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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
class ppInclude;

typedef long long PPINT;
typedef unsigned long long PPUINT;

class ppExpr
{
  public:
    typedef long long CompilerExpression(std::string& line);

    ppExpr(bool isunsignedchar) : define(nullptr), unsignedchar(isunsignedchar), token(nullptr), floatWarned(false) {}
    ~ppExpr() {}

    void SetParams(ppDefine* Define) { define = Define; }
    PPINT Eval(std::string& line, bool fromConditional = false);
    std::string GetString() { return tokenizer->GetString(); }
    void SetDefine(ppDefine* Define) { define = Define; }
    static KeywordHash* GetHash() { return &hash; }

    static void SetInclude(ppInclude* inc) { include = inc; }
    static void SetExpressionHandler(CompilerExpression* handler) { expressionHandler = handler; }

  protected:
    PPINT primary(std::string& line, bool& isunsigned);
    PPINT unary(std::string& line, bool& isunsigned);
    PPINT multiply(std::string& line, bool& isunsigned);
    PPINT add(std::string& line, bool& isunsigned);
    PPINT shift(std::string& line, bool& isunsigned);
    PPINT relation(std::string& line, bool& isunsigned);
    PPINT equal(std::string& line, bool& isunsigned);
    PPINT and_(std::string& line, bool& isunsigned);
    PPINT xor_(std::string& line, bool& isunsigned);
    PPINT or_(std::string& line, bool& isunsigned);
    PPINT logicaland(std::string& line, bool& isunsigned);
    PPINT logicalor(std::string& line, bool& isunsigned);
    PPINT conditional(std::string& line, bool& isunsigned);
    PPINT comma_(std::string& line, bool& isunsigned);

  private:
    bool floatWarned;
    bool unsignedchar;
    ppDefine* define;
    std::unique_ptr<Tokenizer> tokenizer;
    const Token* token;
    static KeywordHash hash;
    static ppInclude* include;
    static CompilerExpression* expressionHandler;
};
#endif