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
#ifndef LINKTOKENIZER_H
#define LINKTOKENIZER_H

#include "ObjTypes.h"

class LinkExpression;

class LinkTokenizer
{
    public:
        enum eTokenType { eUnknown, eSymbol, eNumber, ePartition, eOverlay, eRegion, 
                eComma, eBegin, eEnd, eSemi, eOpen, eClose, eBracketOpen, eBracketClose, eAssign,
                ePC,
                eAddr, eSize, eMaxSize, eRoundSize, eAlign, eVirtual, eFill,
                eAdd, eSub, eMul, eDiv, eNeg, eCmpl, eEOF } ;
        LinkTokenizer(ObjString spec) : data(spec), token(eUnknown), 
                            value(0), lineNo(1) { NextToken(); }
        ~LinkTokenizer() { }
        
        eTokenType NextToken();
        ObjInt GetValue() { return value; }
        ObjInt GetLineNo() { return lineNo; }
        ObjString GetSymbol() { return symbol; }
        eTokenType GetTokenType() { return token; }
        const char *GetData() { return data.c_str(); }
        void Skip(int n) { data.replace(0, n, ""); NextToken(); }
        bool Matches(eTokenType Token) { return token == Token; }
        bool MustMatch(eTokenType Token)
        { 
            if (token == Token)
            {
                NextToken();
                return true;
            }
            return false;
        }
        bool GetExpression(LinkExpression **exp, bool canUsePC);
        ObjString &GetError() { return errorString; }
    private:
        bool GetPrimary(LinkExpression **exp, bool canUsePC);
        bool GetUnary(LinkExpression **exp, bool canUsePC);
        bool GetAdd(LinkExpression **exp, bool canUsePC);
        bool GetMul(LinkExpression **exp, bool canUsePC);
        eTokenType token;
        ObjString data;
        ObjString symbol;
        ObjInt value;
        ObjInt lineNo;
        ObjString errorString;
};
#endif
