/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#include "ObjSymbol.h"
#include "ObjFunction.h"
#include "ObjSection.h"
#include "ObjExpression.h"
#include <cstdio>

ObjSection* ObjExpression::GetSection()
{
    if (section->GetAliasFor())
        return section->GetAliasFor();
    return section;
}
void ObjExpression::Simplify() {}
ObjInt ObjExpression::Eval(ObjInt pc)
{
    switch (op)
    {
        default:
        case eNop:
        case eValue:
            return value;
        case ePC:
            op = eValue;
            value = pc;
            return pc;
        case eSymbol:
            op = eValue;
            value = symbol->GetOffset()->Eval(pc); /* shouldn't have symbols point to symbols */
            return value;
        case eSection:
            op = eValue;
            if (section->GetAliasFor())
                value = section->GetAliasFor()->GetOffset()->Eval(pc);
            else
                value = section->GetOffset()->Eval(pc);
            return value;
        case eNonExpression:
            left->Eval(pc);
            right->Eval(pc);
            return 0;
        case eExpression:
            op = eValue;
            value = left->Eval(pc);
            return value;
        case eAdd:
            op = eValue;
            value = left->Eval(pc) + right->Eval(pc);
            return value;
        case eSub:
            op = eValue;
            value = left->Eval(pc) - right->Eval(pc);
            return value;
        case eMul:
            op = eValue;
            value = left->Eval(pc) * right->Eval(pc);
            return value;
        case eDiv:
            op = eValue;
            if (right->Eval(pc))
                value = left->Eval(pc) / right->Eval(pc);
            else
                value = 0;
            return value;
        case eNeg:
            op = eValue;
            value = -left->Eval(pc);
            return value;
        case eCmpl:
            op = eValue;
            value = ~left->Eval(pc);
            return value;
    }
}
ObjInt ObjExpression::EvalNoModify(ObjInt pc)
{
    switch (op)
    {
        default:
        case eNop:
        case eValue:
            return value;
        case ePC:
            return pc;
        case eSymbol:
            return symbol->GetOffset()->EvalNoModify(pc); /* shouldn't have symbols point to symbols */
        case eSection:
            return 0;
            /*
            if (section->GetAliasFor())
                return section->GetAliasFor()->GetOffset()->EvalNoModify(pc);
            else
                return section->GetOffset()->EvalNoModify(pc);
                */
        case eNonExpression:
            return 0;
        case eExpression:
            return left->EvalNoModify(pc);
        case eAdd:
            return left->EvalNoModify(pc) + right->EvalNoModify(pc);
        case eSub:
            return left->EvalNoModify(pc) - right->EvalNoModify(pc);
        case eMul:
            return left->EvalNoModify(pc) * right->EvalNoModify(pc);
        case eDiv:
            if (right->EvalNoModify(pc))
                return left->EvalNoModify(pc) / right->EvalNoModify(pc);
            return 0;
        case eNeg:
            return -left->EvalNoModify(pc);
        case eCmpl:
            return ~left->EvalNoModify(pc);
    }
}
