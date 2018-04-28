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

#include "LinkAttribs.h"
#include "LinkExpression.h"
LinkAttribs &LinkAttribs::operator =(const LinkAttribs &Attribs)
{
    if (Attribs.address)
        address = new LinkExpression(*Attribs.address);
    else
        address = nullptr;
    if (Attribs.align)
        align = new LinkExpression(*Attribs.align);
    else
        align = nullptr;
    if (Attribs.maxSize)
        maxSize = new LinkExpression(*Attribs.maxSize);
    else
        maxSize = nullptr;
    if (Attribs.roundSize)
        roundSize = new LinkExpression(*Attribs.roundSize);
    else
        roundSize = nullptr;
    if (Attribs.virtualOffset)
        virtualOffset = new LinkExpression(*Attribs.virtualOffset);
    else
        virtualOffset = nullptr;
    if (Attribs.fill)
        fill = new LinkExpression(*Attribs.fill);
    else
        fill = nullptr;
    return *this;
}
LinkAttribs::~LinkAttribs()
{
        delete address;
        delete align;
        delete maxSize;
        delete roundSize;
        delete virtualOffset;
        delete fill;
}
