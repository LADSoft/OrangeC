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
#include "LinkTokenizer.h"
#include "LinkExpression.h"
#include "ObjUtil.h"
#include "UTF8.h"
#include <ctype.h>
LinkTokenizer::eTokenType LinkTokenizer::NextToken()
{
    const char *str = data.c_str();
    int start = 0, end;
    int base ;
    while (str[start])
    {
        while (isspace(str[start]))
        {
            if (str[start] == '\n')
                lineNo++;
            start++;
        }
        end = start;
        if (str[start] == '\0')
        {
            token = eEOF;
            return token;
        }
        switch(str[end++])
        {
        
            case '+':
                data.replace(0,end,"");;
                token = eAdd;
                return token;
            case '-':
                data.replace(0,end,"");;
                token = eSub;
                return token;
            case '*':
                if (UTF8::IsAlnum(str + end) || str[end] == '.' || 
                    str[end] == '_' || str[end] == '$')
                {
                    end--;
                    goto alphajoin;
                }
                data.replace(0,end,"");;
                token = eMul;
                return token;
            case '/':
                data.replace(0,end,"");;
                token = eDiv;
                return token;
            case '~':
                data.replace(0,end,"");;
                token = eCmpl;
                return token;
            case ',':
                data.replace(0,end,"");;
                token = eComma;
                return token;
            case '{':
                data.replace(0,end,"");;
                token = eBegin;
                return token;
            case '}':
                data.replace(0,end,"");;
                token = eEnd;
                return token;
            case ';':
                data.replace(0,end,"");;
                token = eSemi;
                return token;
            case '(':
                data.replace(0,end,"");;
                token = eOpen;
                return token;
            case ')':
                data.replace(0,end,"");;
                token = eClose;
                return token;
            case '[':
                data.replace(0,end,"");;
                token = eBracketOpen;
                return token;
            case ']':
                data.replace(0,end,"");;
                token = eBracketClose;
                return token;
            case '=':
                data.replace(0,end,"");;
                token = eAssign;
                return token;
            case '%':
                start++;
                base = 2;
                goto numberjoin;
            case '$':
                if (isxdigit(str[end]))
                {
                    start++;
                    base = 16;
                    goto numberjoin;
                }
                else if (UTF8::IsAlpha(str + end))
                {
                    end--;
                    goto alphajoin;
                }
                else
                {
                    data.replace(0,end,"");;
                    token = ePC;
                }
                return token;
            default:
            {
                end--;
                if (isdigit(str[end]))
                {
                    base = 10;
                    if (str[end] == '0')
                    {
                        end++;
                        start++;
                        base = 8;
                        if (str[end] == 'x' || str[end] == 'X')
                        {
                            base = 16;
                            end++;
                            start++;
                        }
                    }
numberjoin:
                    switch(base)
                    {
                        case 2:
                            while(str[end] == '0' || str[end] == '1') end++;
                            break;
                        case 8:
                            while (isdigit(str[end]) && str[end] < 8) end++;
                            break;
                        case 10:
                            while (isdigit(str[end])) end++;
                            break;
                        case 16:
                            while (isxdigit(str[end])) end++;
                            break;
                    }
                    if (start == end)
                    {
                        token = eUnknown;
                        return token;
                    }
                    std::string sym = data.substr(start, end - start);
                    data.replace(0,end,"");;
                    switch(base)
                    {
                        case 2:
                            value = ObjUtil::FromBinary(sym.c_str());
                            break;
                        case 8:
                            value = ObjUtil::FromOctal(sym.c_str());
                            break;
                        case 10:
                            value = ObjUtil::FromDecimal(sym.c_str());
                            break;
                        case 16:
                            value = ObjUtil::FromHex(sym.c_str());
                            break;
                        default:
                            value = 0;
                    }
                    token = eNumber;
                    return token;
                }
                if (UTF8::IsAlpha(str + end))
                {
                    case '_':
                    case '.':
alphajoin:
                    while (UTF8::IsAlnum(str + end) || str[end] == '*' || str[end] == '?'
                           || str[end] == '.' || str[end] == '_' || str[end] == '$')
                    {
                        int v = UTF8::CharSpan(str + end);
                        for (int i=0; i < v && end < data.size(); i++)
                            end++;
                    }
                    std::string sym = data.substr(start,end - start);
                    data.replace(0,end,"");;
                    if (sym == "region" || sym == "REGION")
                    {
                        token = eRegion;
                    }
                    else if (sym == "overlay" || sym == "OVERLAY")
                    {
                        token = eOverlay;
                    }
                    else if (sym == "partition" || sym == "PARTITION")
                    {
                        token = ePartition;
                    }
                    else if (sym == "addr" || sym == "ADDR")
                    {
                        token = eAddr;
                    }
                    else if (sym == "size" || sym == "SIZE")
                    {
                        token = eSize;
                    }
                    else if (sym == "maxsize" || sym == "MAXSIZE")
                    {
                        token = eMaxSize;
                    }
                    else if (sym == "align" || sym == "ALIGN")
                    {
                        token = eAlign;
                    }
                    else if (sym == "roundsize" || sym == "ROUNDSIZE")
                    {
                        token = eRoundSize;
                    }
                    else if (sym == "fill" || sym == "FILL")
                    {
                        token = eFill;
                    }
                    else if (sym == "virtual" || sym == "VIRTUAL")
                    {
                        token = eVirtual;
                    }
                    else
                    {
                        symbol = sym;
                        token = eSymbol;
                    }
                    return token;
                }
                token = eUnknown;
                return token;
            }
        }
        str = data.c_str();
    }
    token = eUnknown;
    return token;
}
bool LinkTokenizer::GetPrimary(LinkExpression **exp, bool canUsePC)
{
    switch(token)
    {
        case eOpen:
            NextToken();
            if (!GetMul(exp, canUsePC))
                return false;
            if (!MustMatch(eClose))
                return false;
            break;
        case eNumber:
            *exp = new LinkExpression(value);
            NextToken();
            break;
        case eSymbol:
            *exp = new LinkExpression(symbol);
            NextToken();
            break;
        case ePC:
            if (canUsePC)
            {
                *exp = new LinkExpression(LinkExpression::ePC);
                NextToken();
                break;
            }
            return false;
        default:
            return false;
    }
    return true;
}
bool LinkTokenizer::GetUnary(LinkExpression **exp, bool canUsePC)
{
    switch(token)
    {
        case eAdd:
            NextToken();
            // fallthrough
        default:
            return GetPrimary(exp, canUsePC);
        case eSub:
            NextToken();
            if (!GetPrimary(exp, canUsePC))
                return false;
            *exp = new LinkExpression(LinkExpression::eNeg, *exp);
            break;
        case eCmpl:
            NextToken();
            if (!GetPrimary(exp, canUsePC))
                return false;
            *exp = new LinkExpression(LinkExpression::eCpl, *exp);
            break;
    }
    return true;
}
bool LinkTokenizer::GetAdd(LinkExpression **exp, bool canUsePC)
{
    if (!GetUnary(exp, canUsePC))
        return false;
    switch(token)
    {
        case eAdd:
        case eSub:
        {
            eTokenType cur = token;
            NextToken();
            LinkExpression *right = NULL;
            if (!GetAdd(&right, canUsePC))
            {
                if (right)
                    delete right;
                return false;
            }
            *exp = new LinkExpression(cur == eAdd ? LinkExpression::eAdd : LinkExpression::eSub,
                                        *exp, right);
            break;
        }
        default:
            break;
    }
    return true;
}
bool LinkTokenizer::GetMul(LinkExpression **exp, bool canUsePC)
{
    if (!GetAdd(exp, canUsePC))
        return false;
    switch(token)
    {
        case eMul:
        case eDiv:
        {
            eTokenType cur = token;
            NextToken();
            LinkExpression *right = NULL;
            if (!GetAdd(&right, canUsePC))
            {
                if (right)
                    delete right;
                return false;
            }
            *exp = new LinkExpression(cur == eMul ? LinkExpression::eMul : LinkExpression::eDiv,
                                        *exp, right);
            break;
        }
        default:
            break;
    }
    return true;
}
bool LinkTokenizer::GetExpression(LinkExpression **exp, bool canUsePC)
{
    *exp = NULL;
    if (!GetMul(exp, canUsePC))
    {
        if (*exp)
            delete *exp;
        *exp = NULL;
        return false;
    }
    return true;
}
