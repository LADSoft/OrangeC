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

#include "Icon.h"
#include "RCFile.h"
#include "ResFile.h"
#include "ResourceData.h"
#include <stdexcept>

int Icon::nextIconIndex;
void Icon::ReadBin(ResourceData* rd)
{
    Point pt;
    pt.x = rd->GetByte();
    pt.y = rd->GetByte();
    SetSize(pt);
    int n;

    SetColors(n = rd->GetByte());
    rd->GetByte();
    SetPlanes(rd->GetWord());
    SetBits(rd->GetWord());
    int bytes = rd->GetDWord();
    int offset = rd->GetDWord();
    //    if (!pt.x)
    //        pt.x = 32;
    //    if (!pt.y)
    //        pt.y = (bytes - 0x30) / ((pt.x / 8) *2);

    data = std::make_unique<ResourceData>(rd->GetData() + offset, bytes);
    if (rd->PastEnd() || offset + bytes > rd->GetLen())
        throw std::runtime_error("Icon file too short");
    // borland resets the size in the bitmapinfo header, but apparently sets it wrong...
}
void Icon::WriteRes(ResFile& resFile)
{
    Resource::WriteRes(resFile);
    if (data)
        data->WriteRes(resFile);
    resFile.Release();
}
void GroupIcon::WriteRes(ResFile& resFile)
{
    Resource::WriteRes(resFile);
    resFile.WriteWord(0);
    resFile.WriteWord(1);
    resFile.WriteWord(icons.size());
    for (auto res : *this)
    {
        resFile.WriteByte(res->GetSize().x);
        resFile.WriteByte(res->GetSize().y);
        resFile.WriteByte(res->GetColors());
        resFile.WriteByte(0);
        // sometimes plans or bits are zero so we calculate them
        // note that borland sets the planes to one and bits to 24.  I think this is wrong...
        int bits = res->GetBits();
        int planes = res->GetPlanes();
        int colors = res->GetColors();
        if (!bits || !planes)
        {
            planes = 1;
            // from rcl
            while ((1 << bits) < colors)
                bits++;
        }
        resFile.WriteWord(planes);
        resFile.WriteWord(bits);
        resFile.WriteDWord(res->GetBytes());
        resFile.WriteWord(res->GetIndex());
    }
    resFile.Release();
}
void GroupIcon::ReadRC(RCFile& rcFile)
{
    resInfo.ReadRC(rcFile, false);
    std::unique_ptr<ResourceData> rd = std::make_unique<ResourceData>();
    rd->ReadRC(rcFile);
    rcFile.NeedEol();
    rd->GetWord();
    if (rd->GetWord() != 1)
    {
        throw std::runtime_error("file does not contain icon data");
    }
    int count = rd->GetWord();
    for (int i = 0; i < count; i++)
    {
        Icon* ico = new Icon(resInfo);
        rcFile.GetResFile().Add(ico);
        icons.push_back(ico);
        ico->ReadBin(rd.get());
    }
    resInfo.SetFlags(resInfo.GetFlags() | ResourceInfo::Pure);
}
