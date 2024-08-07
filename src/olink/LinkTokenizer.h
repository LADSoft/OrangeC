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

#ifndef LINKTOKENIZER_H
#define LINKTOKENIZER_H

#include "ObjTypes.h"

class LinkExpression;

class LinkTokenizer
{
  public:
    // clang-format off
        enum eTokenType { eUnknown, eSymbol, eNumber, ePartition, eOverlay, eRegion, 
                eComma, eBegin, eEnd, eSemi, eOpen, eClose, eBracketOpen, eBracketClose, eAssign,
                ePC,
                eAddr, eSize, eMaxSize, eRoundSize, eAlign, eVirtual, eFill,
                eAdd, eSub, eMul, eDiv, eNeg, eCmpl, eEOF } ;
    // clang-format on
    LinkTokenizer(ObjString spec) : data(spec), token(eUnknown), value(0), lineNo(1) { NextToken(); }
    ~LinkTokenizer() {}

    eTokenType NextToken();
    ObjInt GetValue() { return value; }
    ObjInt GetLineNo() { return lineNo; }
    ObjString GetSymbol() { return symbol; }
    eTokenType GetTokenType() { return token; }
    const char* GetData() { return data.c_str(); }
    void Skip(int n)
    {
        data.replace(0, n, "");
        NextToken();
    }
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
    bool GetExpression(LinkExpression** exp, bool canUsePC);
    ObjString& GetError() { return errorString; }

  private:
    bool GetPrimary(LinkExpression** exp, bool canUsePC);
    bool GetUnary(LinkExpression** exp, bool canUsePC);
    bool GetAdd(LinkExpression** exp, bool canUsePC);
    bool GetMul(LinkExpression** exp, bool canUsePC);
    eTokenType token;
    ObjString data;
    ObjString symbol;
    ObjInt value;
    ObjInt lineNo;
    ObjString errorString;
};
#endif
