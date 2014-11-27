/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#include "Cursor.h"
#include "RCFile.h"
#include "ResFile.h"
#include "ResourceData.h"
#include <stdexcept>

int Cursor::nextCursorIndex;

void Cursor::WriteRes(ResFile &resFile)
{
    Resource::WriteRes(resFile); 
    hotspot.WriteRes(resFile); 
    if (data) 
        data->WriteRes(resFile); 
    resFile.Release();
}
bool Cursor::ReadBin(ResourceData *rd)
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
        pt.y = (bytes - 0x30) / ((pt.x / 8) *2);
    SetSize(pt);
    
    data = new ResourceData(rd->GetData() + offset, bytes);
    
    if (rd->PastEnd() || offset + bytes > rd->GetLen())
        throw new std::runtime_error("Cursor file too short");
    return true;
}
void GroupCursor::WriteRes(ResFile &resFile)
{
    Resource::WriteRes(resFile); 
    resFile.WriteWord(0);
    resFile.WriteWord(2);
    resFile.WriteWord(cursors.size());
    for (iterator it = begin(); it != end(); ++it)
    {
        resFile.WriteWord((*it)->GetSize().x);
        resFile.WriteWord((*it)->GetSize().x * 2);
        resFile.WriteWord(1);
        resFile.WriteWord(1);
        resFile.WriteDWord((*it)->GetBytes() + 4);
        resFile.WriteWord((*it)->GetIndex());
     }
    resFile.Release();
}
void GroupCursor::ReadRC(RCFile &rcFile)
{
    resInfo.ReadRC(rcFile, false);
    ResourceData *rd = new ResourceData;
    rd->ReadRC(rcFile);
    rcFile.NeedEol();
    rd->GetWord();
    if (rd->GetWord() != 2)
    {
        throw new std::runtime_error("file does not contain cursor data");
    }
    int count = rd->GetWord();
    for (int i=0; i < count; i++)
    {
        Cursor *c = new Cursor(resInfo);
        rcFile.GetResFile().Add(c);
        cursors.push_back(c);
        c->ReadBin(rd);
    }
    resInfo.SetFlags(resInfo.GetFlags() | ResourceInfo::Pure);
    delete rd;
}
