/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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
 */

#include "DotNetPELib.h"
namespace DotNetPELib
{
const char* Qualifiers::qualifierNames_[] = {"public",
                                             "private",
                                             "static",
                                             "instance",
                                             "explicit",
                                             "ansi",
                                             "sealed",
                                             "enum",
                                             "value",
                                             "sequential",
                                             "auto",
                                             "literal",
                                             "hidebysig",
                                             "preservesig",
                                             "specialname",
                                             "rtspecialname",
                                             "cil",
                                             "managed",
                                             "runtime",
                                             "",
                                             "virtual",
                                             "newslot",
                                             "",
                                             "",
                                             "",
                                             "",
                                             "",
                                             "",
                                             "",
                                             "",
                                             "",
                                             ""};
int Qualifiers::afterFlags_ = Qualifiers::PreserveSig | Qualifiers::CIL | Qualifiers::Managed | Qualifiers::Runtime;
void Qualifiers::ILSrcDumpBeforeFlags(PELib& peLib) const
{
    int n = ~afterFlags_ & flags_;
    for (int i = 0; i < 32; i++)
        if (n & (1 << i))
            peLib.Out() << " " << qualifierNames_[i];
}
void Qualifiers::ILSrcDumpAfterFlags(PELib& peLib) const
{
    int n = afterFlags_ & flags_;
    for (int i = 0; i < 32; i++)
        if (n & (1 << i))
            peLib.Out() << " " << qualifierNames_[i];
}
void Qualifiers::ObjOut(PELib& peLib, int pass) const { peLib.Out() << flags_; }
void Qualifiers::ObjIn(PELib& peLib, bool definition) { flags_ = peLib.ObjInt(); }
void Qualifiers::ReverseNamePrefix(std::string& rv, const DataContainer* parent, int& pos, bool type)
{
    if (parent)
    {
        ReverseNamePrefix(rv, parent->Parent(), pos, type);
        if (pos != 0)
        {
            rv += parent->Name();
            if (typeid(*parent) == typeid(Class) && (!parent->Parent() || typeid(*(parent->Parent())) != typeid(Class)))
                rv += type ? "\xf8" : "/";
            else
                rv += '.';
        }
        else if (typeid(*parent) == typeid(AssemblyDef))
        {
            if (parent->InAssemblyRef())
            {
                rv += std::string("[") + parent->Name() + "]";
            }
        }
        pos++;
    }
}
std::string Qualifiers::GetNamePrefix(const DataContainer* parent, bool type)
{
    std::string rv;
    if (parent)
    {
        int pos = 0;
        ReverseNamePrefix(rv, parent, pos, type);
    }
    return rv;
}
std::string Qualifiers::GetName(const std::string& root, const DataContainer* parent, bool type)
{
    std::string rv = GetNamePrefix(parent, type);
    if (rv.size())
    {
        rv = rv.substr(0, rv.size() - 1);
    }
    if (root.size())
    {
        if (rv.size())
            rv += "::";
        rv += "'" + root + "'";
    }
    return rv;
}

std::string Qualifiers::GetObjName(const std::string& stem, const DataContainer* parent)
{
    int pos = 0;
    std::string rv;
    ReverseNamePrefix(rv, parent, pos, false);
    int npos = rv.find('/');
    if (npos != std::string::npos)
        rv[npos] = '.';
    if (rv.size())
    {
        if (rv[rv.size() - 1] == '.')
        {
            if (rv.size() == 1)
                rv = "";
            else
                rv = rv.substr(0, rv.size() - 1);
        }
    }
    if (stem.size())
    {
        if (rv.size())
        {
            rv = rv + "::" + stem;
        }
        else
        {
            rv = stem;
        }
    }
    return rv;
}
}  // namespace DotNetPELib