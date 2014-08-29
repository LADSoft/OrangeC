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
#include "LinkNameLogic.h"
#include "LinkRegionFileSpec.h"
#include "UTF8.h"
#include <fstream>
#include <ctype.h>
#include <iostream>
bool LinkNameLogic::ParseItem::Matches(const std::string &name)
{
    if (mode == eName)
    {
        LinkRegionFileSpecContainer fs(token);
        return fs.Matches(name);
    }
    else
    {
        bool l = true, r = true;
        if (left)
            l = left->Matches(name);
        if (right)
            r = right->Matches(name);
        switch (mode)
        {
            case eAnd:
                return l && r;
            case eOr:
                return l || r;
            case eNot:
                return !l;
            default:
                return false;
        }
    }
}
void LinkNameLogic::ParseItem::Unlink()
{
    if (left)
        left->Unlink();
    if (right)
        right->Unlink();
    delete this;
}	
LinkNameLogic::~LinkNameLogic()
{
    if (top)
        top->Unlink();
}
bool LinkNameLogic::Matches(const std::string &name)
{
    if (top)
        return top->Matches(name);
    return false;
}

void LinkNameLogic::ParseOut(std::string spec)
{
    top = ParseOutOr(spec);
    if (!top)
        std::cout << "Warning: region specifier '" << spec << "' is invalid" << std::endl;
}
LinkNameLogic::ParseItem *LinkNameLogic::ParseOutOr(std::string &spec)
{
    ParseItem *rv = ParseOutAnd(spec);
    int n = spec.find_first_not_of("\r\n\t\v \f");
    if (n != std::string::npos)
    {
        spec.erase(0, n);
    }
    while (rv && spec.size() && spec[0] == '|')
    {
        spec.erase(0,1);
        ParseItem *right = ParseOutAnd(spec);
        ParseItem *v = new ParseItem;
        v->mode = ParseItem::eOr;
        v->left = rv;
        v->right = right;
        rv = v;
        n = spec.find_first_not_of("\r\n\t\v \f");
        if (n != std::string::npos)
        {
            spec.erase(0, n);
        }
    }
    return rv;
}
LinkNameLogic::ParseItem *LinkNameLogic::ParseOutAnd(std::string &spec)
{
    ParseItem *rv = ParseOutNot(spec);
    int n = spec.find_first_not_of("\r\n\t\v \f");
    if (n != std::string::npos)
    {
        spec.erase(0, n);
    }
    while (rv && spec.size() && spec[0] == '&')
    {
        spec.erase(0,1);
        ParseItem *right = ParseOutAnd(spec);
        ParseItem *v = new ParseItem;
        v->mode = ParseItem::eAnd;
        v->left = rv;
        v->right = right;
        rv = v;
        int n = spec.find_first_not_of("\r\n\t\v \f");
        if (n != std::string::npos)
        {
            spec.erase(0, n);
        }
    }
    return rv;
}

LinkNameLogic::ParseItem *LinkNameLogic::ParseOutNot(std::string &spec)
{
    bool invert = false;
    int n = spec.find_first_not_of("\r\n\t\v \f");
    if (n != std::string::npos)
    {
        spec.erase(0, n);
    }
    while (spec.size() && spec[0] == '!')
    {
        spec.erase(0, 1);
        invert = !invert;
        n = spec.find_first_not_of("\r\n\t\v \f");
        if (n != std::string::npos)
        {
            spec.erase(0, n);
        }
    }
    ParseItem *rv = ParseOutPrimary(spec);
    if (rv && invert)
    {
        ParseItem *v = new ParseItem;
        v->mode = ParseItem::eNot;
        v->left = rv;
        rv = v;
    }
    return rv;
}
LinkNameLogic::ParseItem *LinkNameLogic::ParseOutPrimary(std::string &spec)
{
    int n = spec.find_first_not_of("\r\n\t\v \f");
    if (n != std::string::npos)
    {
        spec.erase(0, n);
    }
    if (spec[0] == '(')
    {
        spec.erase(0, 1);	
        ParseItem *rv = ParseOutOr(spec);
        n = spec.find_first_not_of("\r\n\t\v \f");
        if (n != std::string::npos)
        {
            spec.erase(0, n);
        }
        if (!spec.size() || spec[0] != ')')
            return NULL;
        spec.erase(0, 1);
        return rv;
    }
    char buf[4096], *q = buf;
    n = 0;
    while ((UTF8::IsAlnum(spec.c_str() + n) || spec[n] == '*' || spec[n] == '?') && spec.size() > n)
    {
        int v = UTF8::CharSpan(spec.c_str() + n);
        for (int i=0; i < v && spec.size() >= v + n; i++)
            *q++ = spec[n++];
    }
    *q = 0;
    spec.erase(0, n);
    ParseItem *rv = new ParseItem;
    rv->mode = ParseItem::eName;
    rv->token = buf;
    return rv;
}
