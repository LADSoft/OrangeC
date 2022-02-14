/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

#include "Accelerators.h"
#include "RCFile.h"
#include "ResFile.h"

#include <stdexcept>
void Accelerators::Key::WriteRes(ResFile& resFile, bool last)
{
    resFile.WriteWord(flags | (last ? Last : 0));
    resFile.WriteWord(key);
    resFile.WriteWord(id);
    resFile.WriteWord(0);
}
void Accelerators::Key::ReadRC(RCFile& rcFile)
{
    if (rcFile.IsString())
    {
        std::wstring str = rcFile.GetString();
        if (str[0] == '^')
        {
            if (str.size() != 2)
                throw std::runtime_error("Invalid key sequence");
            if (str[1] == '^')
                key = '^';
            else
                key = str[1] & 0x1f;
        }
        else
        {
            if (str.size() != 1)
                throw std::runtime_error("Invalid key sequence");
            key = str[0];
        }
    }
    else
    {
        key = rcFile.GetNumber();
    }
    rcFile.SkipComma();
    id = rcFile.GetNumber();
    rcFile.SkipComma();
    bool done = false;
    while (rcFile.IsKeyword() && !done)
    {
        switch (rcFile.GetToken()->GetKeyword())
        {
            case kw::ASCII:
                flags &= ~Virtkey;
                break;
            case kw::VIRTKEY:
                flags |= Virtkey;
                break;
            case kw::NOINVERT:
                flags |= NoInvert;
                break;
            case kw::SHIFT:
                flags |= Shift;
                break;
            case kw::CONTROL:
                flags |= Control;
                break;
            case kw::ALT:
                flags |= Alt;
                break;
            default:
                done = true;
                break;
        }
        if (!done)
        {
            rcFile.NextToken();
            rcFile.SkipComma();
        }
    }
    rcFile.NeedEol();
}
void Accelerators::WriteRes(ResFile& resFile)
{
    Resource::WriteRes(resFile);
    int count = keys.size();
    for (auto res : *this)
    {
        res.WriteRes(resFile, --count == 0);
    }
    resFile.Release();
}
void Accelerators::ReadRC(RCFile& rcFile)
{
    resInfo.SetFlags((resInfo.GetFlags() & ~ResourceInfo::Discardable) | ResourceInfo::Pure);
    resInfo.ReadRC(rcFile, true);
    rcFile.NeedBegin();
    while (rcFile.IsString() || rcFile.IsNumber())
    {
        // int done;
        Key key;
        key.ReadRC(rcFile);
        keys.push_back(key);
    }
    rcFile.NeedEnd();
}
