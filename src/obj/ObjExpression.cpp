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
#include "ObjSymbol.h"
#include "ObjFunction.h"
#include "ObjSection.h"
#include "ObjExpression.h"

void ObjExpression::Simplify()
{
}
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
            return section->GetOffset()->EvalNoModify(pc);
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
            else
                return 0;
        case eNeg:
            return -left->EvalNoModify(pc);
        case eCmpl:
            return ~left->EvalNoModify(pc);
    }
}
