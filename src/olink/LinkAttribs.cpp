/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#include "LinkAttribs.h"
#include "LinkExpression.h"
void LinkAttribs::SetAddress(LinkExpression* Address)
{
    std::unique_ptr<LinkExpression> temp(Address);
    address = std::move(temp);
}
void LinkAttribs::SetAlign(LinkExpression* Align)
{
    std::unique_ptr<LinkExpression> temp(Align);
    align = std::move(temp);
}
void LinkAttribs::SetMaxSize(LinkExpression* Size)
{
    std::unique_ptr<LinkExpression> temp(Size);
    maxSize = std::move(temp);
}
void LinkAttribs::SetSize(LinkExpression* Size)
{
    std::unique_ptr<LinkExpression> temp(Size);
    size = std::move(temp);
}
void LinkAttribs::SetRoundSize(LinkExpression* RoundSize)
{
    std::unique_ptr<LinkExpression> temp(RoundSize);
    roundSize = std::move(temp);
}
void LinkAttribs::SetVirtualOffset(LinkExpression* VirtualOffset)
{
    std::unique_ptr<LinkExpression> temp(VirtualOffset);
    virtualOffset = std::move(temp);
}
void LinkAttribs::SetFill(LinkExpression* Fill)
{
    std::unique_ptr<LinkExpression> temp(Fill);
    fill = std::move(temp);
}

LinkAttribs& LinkAttribs::operator=(const LinkAttribs& Attribs)
{
    if (Attribs.address)
        address = std::make_unique<LinkExpression>(*Attribs.address);
    if (Attribs.align)
        align = std::make_unique<LinkExpression>(*Attribs.align);
    if (Attribs.maxSize)
        maxSize = std::make_unique<LinkExpression>(*Attribs.maxSize);
    if (Attribs.roundSize)
        roundSize = std::make_unique<LinkExpression>(*Attribs.roundSize);
    if (Attribs.virtualOffset)
        virtualOffset = std::make_unique<LinkExpression>(*Attribs.virtualOffset);
    if (Attribs.fill)
        fill = std::make_unique<LinkExpression>(*Attribs.fill);
    return *this;
}
LinkAttribs::~LinkAttribs() {}
