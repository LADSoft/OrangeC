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
#include "PEFile.h"
namespace DotNetPELib
{

bool Namespace::ILSrcDump(PELib& peLib) const
{

    peLib.Out() << ".namespace '" << name_ << "' {" << std::endl;
    DataContainer::ILSrcDump(peLib);
    peLib.Out() << "}" << std::endl;
    return true;
}
void Namespace::ObjOut(PELib& peLib, int pass) const
{
    peLib.Out() << std::endl << "$nb" << peLib.FormatName(name_);
    DataContainer::ObjOut(peLib, pass);
    peLib.Out() << std::endl << "$ne";
}
Namespace* Namespace::ObjIn(PELib& peLib, bool definition)
{
    // always a definition (never used as an operand)
    std::string name = peLib.UnformatName();
    Namespace *temp, *rv = nullptr;
    temp = (Namespace*)peLib.GetContainer()->FindContainer(name);
    if (temp && typeid(*temp) != typeid(Namespace))
        peLib.ObjError(oe_nonamespace);
    if (!temp)
        rv = temp = peLib.AllocateNamespace(name);
    ((DataContainer*)temp)->ObjIn(peLib);
    if (peLib.ObjEnd() != 'n')
        peLib.ObjError(oe_syntax);
    return rv;
}
std::string Namespace::ReverseName(DataContainer* child)
{
    std::string rv;
    if (child->Parent())
    {
        if (child->Parent()->Parent())
            rv = ReverseName(child->Parent()) + ".";
        rv += child->Name();
    }
    return rv;
}
bool Namespace::PEDump(PELib& peLib)
{
    if (!InAssemblyRef() || !PEIndex())
    {
        std::string fullName = ReverseName(this);
        peIndex_ = peLib.PEOut().HashString(fullName);
    }
    if (!InAssemblyRef())
        DataContainer::PEDump(peLib);
    return true;
}
}  // namespace DotNetPELib