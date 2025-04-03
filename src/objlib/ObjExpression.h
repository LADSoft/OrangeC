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

#ifndef OBJEXPRESSION_H
#define OBJEXPRESSION_H

#include "ObjTypes.h"
class ObjSymbol;
class ObjFunction;
class ObjSection;

class ObjExpression : public ObjWrapper
{
  public:
    // clang-format off
    enum eOperator
    {
        eNop,
        eValue,
        eExpression,
        eSymbol,
        eSection,
        eNonExpression,
        eAdd,
        eSub,
        eMul,
        eDiv,
        eNeg,
        eCmpl,
        ePC
    };
    // clang-format on
    ObjExpression(ObjInt Left) : op(eValue), value(Left), left(nullptr), right(nullptr) {}
    ObjExpression(ObjExpression* Left) : op(eExpression), left(Left), right(nullptr), value(0) {}
    ObjExpression(ObjSymbol* Left) : op(eSymbol), symbol(Left), left(nullptr), right(nullptr) {}
    ObjExpression(ObjSection* Left) : op(eSection), section(Left), left(nullptr), right(nullptr) {}
    ObjExpression(eOperator Type) : op(Type), value(0), left(nullptr), right(nullptr) {}
    ObjExpression(eOperator Type, ObjExpression* Left) : op(Type), left(Left), right(nullptr), value(0) {}
    ObjExpression(eOperator Type, ObjExpression* Left, ObjExpression* Right) : op(Type), left(Left), right(Right), value(0) {}
    virtual ~ObjExpression() {}

    eOperator GetOperator() { return op; }
    ObjExpression* GetLeft() { return left; }
    void SetLeft(ObjExpression* Left) { left = Left; }
    ObjExpression* GetRight() { return right; }
    void SetRight(ObjExpression* Right) { right = Right; }
    ObjSymbol* GetSymbol() { return symbol; }
    ObjSection* GetSection();
    ObjInt GetValue() { return value; }
    eOperator GetOp() { return op; }
    void Simplify();
    ObjInt Eval(ObjInt pc);
    ObjInt EvalNoModify(ObjInt pc);

  private:
    union
    {
        ObjSymbol* symbol;
        ObjSection* section;
        ObjInt value;
    };
    ObjExpression* left;
    ObjExpression* right;
    eOperator op;
};
#endif  // OBJEXPRESSION_H
