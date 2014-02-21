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
#include "Icon.h"
#include "RCFile.h"
#include "ResFile.h"
#include "ResourceData.h"
#include <exception>

int Icon::nextIconIndex;
bool Icon::ReadBin(ResourceData *rd)
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
    
    data = new ResourceData(rd->GetData() + offset, bytes);
    if (rd->PastEnd() || offset + bytes > rd->GetLen())
        throw new std::runtime_error("Icon file too short");
    // borland resets the size in the bitmapinfo header, but apparently sets it wrong...
	return true; // FIXME
}
void Icon::WriteRes(ResFile &resFile) 
{ 
    Resource::WriteRes(resFile); 
    if (data) 
        data->WriteRes(resFile); 
    resFile.Release(); 
}
void GroupIcon::WriteRes(ResFile &resFile)
{
    Resource::WriteRes(resFile);
    resFile.WriteWord(0);
    resFile.WriteWord(1);
    resFile.WriteWord(icons.size());	
    for (iterator it = begin(); it != end(); ++it)
    {
        resFile.WriteByte((*it)->GetSize().x);
        resFile.WriteByte((*it)->GetSize().y);
        resFile.WriteByte((*it)->GetColors());
        resFile.WriteByte(0);
        // sometimes plans or bits are zero so we calculate them
        // note that borland sets the planes to one and bits to 24.  I think this is wrong...
        int bits = (*it)->GetBits();
        int planes = (*it)->GetPlanes();
        int colors = (*it)->GetColors();
        if (!bits || !planes)
        {
            planes = 1;
            // from rcl
            while ((1 << bits) < colors)
                bits++;
        }
        resFile.WriteWord(planes);
        resFile.WriteWord(bits);
        resFile.WriteDWord((*it)->GetBytes());
        resFile.WriteWord((*it)->GetIndex());
    }
    resFile.Release();
}
bool GroupIcon::ReadRC(RCFile &rcFile)
{
    resInfo.ReadRC(rcFile, false);
    ResourceData *rd = new ResourceData;
    rd->ReadRC(rcFile);
    rcFile.NeedEol();
    rd->GetWord();
    if (rd->GetWord() != 1)
    {
        throw new std::runtime_error("file does not contain icon data");
    }
    int count = rd->GetWord();
    for (int i=0; i < count; i++)
    {
        Icon *ico = new Icon(resInfo);
        rcFile.GetResFile().Add(ico);
        icons.push_back(ico);
        ico->ReadBin(rd);
    }
    resInfo.SetFlags(resInfo.GetFlags() | ResourceInfo::Pure);
    delete rd;
	return true; // FIXME
}
    
