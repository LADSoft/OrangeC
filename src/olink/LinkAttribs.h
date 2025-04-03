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

#ifndef LINKATTRIBS_H
#define LINKATTRIBS_H

#include "ObjTypes.h"
#include "LinkExpression.h"
#include <memory>

class LinkAttribs
{
  public:
    LinkAttribs() :
        address(nullptr), align(nullptr), maxSize(nullptr), roundSize(nullptr), size(nullptr), virtualOffset(nullptr), fill(nullptr)
    {
    }
    ~LinkAttribs();
    LinkAttribs& operator=(const LinkAttribs& Attribs);
    ObjInt GetAddress()
    {
        if (address)
            return address->Eval(0);
        return 0;
    }
    void SetAddress(LinkExpression* Address);
    ObjInt GetAlign()
    {
        if (align)
            return align->Eval(0);
        return 1;
    }
    void SetAlign(LinkExpression* Align);
    ObjInt GetMaxSize()
    {
        if (maxSize)
            return maxSize->Eval(0);
        return 0;
    }
    void SetMaxSize(LinkExpression* Size);
    ObjInt GetSize()
    {
        if (size)
            return size->Eval(0);
        return 0;
    }
    void SetSize(LinkExpression* Size);
    ObjInt GetRoundSize()
    {
        if (roundSize)
            return roundSize->Eval(0);
        return 0;
    }
    void SetRoundSize(LinkExpression* RoundSize);
    ObjInt GetVirtualOffset()
    {
        if (virtualOffset)
            return virtualOffset->Eval(0);
        return 0;
    }
    void SetVirtualOffset(LinkExpression* VirtualOffset);
    ObjInt GetFill()
    {
        if (fill)
            return fill->Eval(0);
        return 0;
    }
    void SetFill(LinkExpression* Fill);
    bool GetAddressSpecified() { return address != nullptr; }
    bool GetVirtualOffsetSpecified() { return virtualOffset != nullptr; }
    bool GetHasFill() { return fill != nullptr; }

  private:
    std::unique_ptr<LinkExpression> address;
    std::unique_ptr<LinkExpression> align;
    std::unique_ptr<LinkExpression> maxSize;
    std::unique_ptr<LinkExpression> roundSize;
    std::unique_ptr<LinkExpression> virtualOffset;
    std::unique_ptr<LinkExpression> size;
    std::unique_ptr<LinkExpression> fill;
};

#endif
