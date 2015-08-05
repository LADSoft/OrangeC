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
#include "ResFile.h"
#include "Resource.h"
#include "Font.h"
#include "Icon.h"
#include "Cursor.h"
#include "StringTable.h"

#include <fstream>

ResFile::~ResFile()
{
    for (std::deque<Resource *>::iterator it = resources.begin(); it != resources.end(); ++it)
    {
        Resource *res = *it;
        delete res;
    }
    for (std::set<Resource *, lt>::iterator it = strings.begin(); it != strings.end(); ++it)
    {
        Resource *res = *it;
        delete res;
    }
}
void ResFile::Mark()
{
    Align();
    base = stream->tellp();
}
void ResFile::MarkHeader()
{
    hdrSize = (size_t)stream->tellp() - base;
}
void ResFile::Release()
{
    size_t pos = stream->tellp();
    stream->seekp(base);
    WriteDWord(pos - base - hdrSize);
    WriteDWord(hdrSize);
    stream->seekp(pos);
}
void ResFile::Align()
{
    size_t pos = stream->tellp();
    if (pos %4)
    {
        int size = 4 - pos % 4;
        int n = 0;
        stream->write((char *)&n, size);
    }
}
void ResFile::WriteByte(int byte)
{
    char c = (char) byte;
    stream->write(&c, 1);
}

void ResFile::WriteWord(int word)
{
    WriteByte(word & 0xff);
    WriteByte(word >> 8);
}

void ResFile::WriteDWord(int dword)
{
    WriteByte(dword & 0xff);
    WriteByte(dword >> 8);
    WriteByte(dword >> 16);
    WriteByte(dword >> 24);
}
void ResFile::WriteData(const unsigned char *data, int len)
{
    stream->write((char *)data, len);
}
void ResFile::WriteString(const std::wstring &str)
{
    int n = str.size();
    // writing word by word for portability
    for (int i=0; i < n; i++)
        WriteWord(str[i]);
    WriteWord(0);
}

bool ResFile::Write(const std::string &name)
{
    stream = new std::fstream(name.c_str(), std::ios::out | std::ios::binary);
    //if (!stream->fail())
    {
        for (std::deque<Resource *>::iterator it = resources.begin(); it != resources.end(); ++it)
        {
            (*it)->WriteRes(*this);
        }
        for (std::set<Resource *, lt>::iterator it = strings.begin(); it != strings.end(); ++it)
        {
            (*it)->WriteRes(*this);
        }
        // the alignment rules for end of file seem strange
        // we are just going to align on a dword boundary like gnu rc does
        Align();
        stream->close();
        bool rv = !stream->fail();
        delete stream;
        stream = NULL;
        return rv;
    }
    return false;
}

void ResFile::Reset()
{
    Icon::Reset();
    Cursor::Reset();
    StringTable::Reset();
}
void ResFile::Add(Resource *th)
{
    if (th->GetType() == Resource::eString)
        strings.insert(th);
    else
        resources.push_back(th);
}
