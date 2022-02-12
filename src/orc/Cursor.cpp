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

#include "Cursor.h"
#include "RCFile.h"
#include "ResFile.h"
#include "ResourceData.h"
#include <stdexcept>

int Cursor::nextCursorIndex;

void Cursor::WriteRes(ResFile& resFile)
{
    Resource::WriteRes(resFile);
    hotspot.WriteRes(resFile);
    if (data)
        data->WriteRes(resFile);
    resFile.Release();
}
bool Cursor::ReadBin(ResourceData* rd)
{
    Point pt;
    pt.x = rd->GetByte();
    pt.y = rd->GetByte();
    SetColors(rd->GetByte());
    rd->GetByte();
    Point pt1;
    pt1.x = rd->GetWord();
    pt1.y = rd->GetWord();
    SetHotspot(pt1);
    int bytes = rd->GetDWord();
    int offset = rd->GetDWord();
    if (!pt.x)
        pt.x = 32;
    if (!pt.y)
        pt.y = (bytes - 0x30) / ((pt.x / 8) * 2);
    SetSize(pt);

    data = std::make_unique<ResourceData>(rd->GetData() + offset, bytes);

    if (rd->PastEnd() || offset + bytes > rd->GetLen())
        throw std::runtime_error("Cursor file too short");
    return true;
}
void GroupCursor::WriteRes(ResFile& resFile)
{
    Resource::WriteRes(resFile);
    resFile.WriteWord(0);
    resFile.WriteWord(2);
    resFile.WriteWord(cursors.size());
    for (auto res : *this)
    {
        resFile.WriteWord(res->GetSize().x);
        resFile.WriteWord(res->GetSize().x * 2);
        resFile.WriteWord(1);
        resFile.WriteWord(1);
        resFile.WriteDWord(res->GetBytes() + 4);
        resFile.WriteWord(res->GetIndex());
    }
    resFile.Release();
}
void GroupCursor::ReadRC(RCFile& rcFile)
{
    resInfo.ReadRC(rcFile, false);
    std::unique_ptr<ResourceData> rd = std::make_unique<ResourceData>();
    rd->ReadRC(rcFile);
    rcFile.NeedEol();
    rd->GetWord();
    if (rd->GetWord() != 2)
    {
        throw std::runtime_error("file does not contain cursor data");
    }
    int count = rd->GetWord();
    for (int i = 0; i < count; i++)
    {
        Cursor* c = new Cursor(resInfo);
        rcFile.GetResFile().Add(c);
        cursors.push_back(c);
        c->ReadBin(rd.get());
    }
    resInfo.SetFlags(resInfo.GetFlags() | ResourceInfo::Pure);
}
