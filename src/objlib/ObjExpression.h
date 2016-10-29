/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
#ifndef OBJEXPRESSION_H
#define OBJEXPRESSION_H

#include "ObjTypes.h"
class ObjSymbol;
class ObjFunction;
class ObjSection;

class ObjExpression : public ObjWrapper
{
public:
    enum eOperator { eNop, eValue, eExpression, eSymbol, eSection, eNonExpression,
                     eAdd, eSub, eMul, eDiv, eNeg, eCmpl, ePC };
    ObjExpression(ObjInt Left) : op(eValue), value(Left), left(NULL), right(NULL) {}
    ObjExpression(ObjExpression *Left) : op(eExpression), left(Left), right(NULL) {}
    ObjExpression(ObjSymbol *Left) : op(eSymbol), symbol(Left), left(NULL), right(NULL) {}
    ObjExpression(ObjSection *Left) : op(eSection), section(Left), left(NULL), right(NULL) {}
    ObjExpression(eOperator Type) : op(Type), value(0), left(NULL), right(NULL) {}
    ObjExpression(eOperator Type, ObjExpression *Left) 
                        : op(Type), left(Left), right(NULL) {}
    ObjExpression(eOperator Type, ObjExpression *Left, ObjExpression *Right) 
                        : op(Type), left(Left), right(Right) {}
    virtual ~ObjExpression() { }
    
    eOperator GetOperator() { return op; }
    ObjExpression *GetLeft() { return left; }
    void SetLeft(ObjExpression *Left) { left = Left; }
    ObjExpression *GetRight() { return right; }
    void SetRight(ObjExpression *Right) { right = Right; }
    ObjSymbol *GetSymbol() { return symbol; }
    ObjSection *GetSection();
    ObjInt GetValue() { return value; }
    eOperator GetOp() { return op; }
    void Simplify() ;
    ObjInt Eval(ObjInt pc) ;
    ObjInt EvalNoModify(ObjInt pc);
private:
    union {
        ObjSymbol *symbol;
        ObjSection *section;
        ObjInt value;
    };
    ObjExpression *left;
    ObjExpression *right;
    eOperator op;
};
#endif //OBJEXPRESSION_H
