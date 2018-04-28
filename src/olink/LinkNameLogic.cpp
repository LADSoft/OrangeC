/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
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
        std::cout << "Warning: region specifier '" << spec.c_str() << "' is invalid" << std::endl;
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
            return nullptr;
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
