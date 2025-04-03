/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#include <typeinfo>
#include <string.h>
#include <stdio.h>
#include "_rtti.h"

extern void _RTL_FUNC __undecorate(char* buf, char* name);
namespace std
{

type_info::type_info(const type_info& in) { this->tpp = in.tpp; }
type_info& type_info::operator=(const type_info& in) { return *this; }

type_info::~type_info() {}

bool type_info::operator==(const type_info& in) const
{
    if (this->tpp == in.tpp)
        return true;
    RTTI* s1 = (RTTI*)this->tpp;
    RTTI* s2 = (RTTI*)in.tpp;
    if (s1->flags & s2->flags & XD_POINTER)
        if (s1->destructor == s2->destructor)
            return true;
    return false;
}
bool type_info::operator!=(const type_info& in) const { return !(*this == in); }

bool type_info::before(const type_info& in) const
{
    RTTI* left = (RTTI*)this->tpp;
    RTTI* right = (RTTI*)in.tpp;
    return (strcmp(left->name, right->name) > 0);
}

const char* type_info::name() const
{
    RTTI* ths = (RTTI*)this->tpp;
    return ths->name;
}

};  // namespace std
