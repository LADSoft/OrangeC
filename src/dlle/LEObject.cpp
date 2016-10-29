/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
#include <fstream>
#include "LEObject.h"
#include "ObjFile.h"
#include "ObjSection.h"
#include "ObjMemory.h"
#include "ObjExpression.h"
#include "LEHeader.h"
#include <string.h>
ObjFile *LEObject::file;

bool LEObject::IsRel(ObjExpression *e)
{
    if (!e)
        return false;
    if (e->GetOperator() == ObjExpression::ePC)
        return true;
    return IsRel(e->GetLeft()) || IsRel(e->GetRight());
}
void LEObject::Setup(unsigned &offs)
{
    pageOffs = offs + 1;
    offs += ObjectAlign(4096, initSize)/4096;
    data = new unsigned char[initSize];
    ObjMemoryManager &m = sect->GetMemoryManager();
    int ofs = 0;
    for (ObjMemoryManager::MemoryIterator it = m.MemoryBegin(); it != m.MemoryEnd() && ofs < initSize; ++it)
    {
        int msize = (*it)->GetSize();
        ObjByte *mdata = (*it)->GetData();
        if (msize)
        {
            ObjExpression *fixup = (*it)->GetFixup();
            if (fixup)
            {
                int sbase = sect->GetOffset()->Eval(0);
                int n = fixup->Eval(sbase + ofs); // need this to apply relative offsets in code seg
                int bigEndian = file->GetBigEndian();
                if (msize == 1)
                {
                    data[ofs] = n & 0xff;
                }
                else if (msize == 2)
                {
                    if (bigEndian)
                    {
                        data[ofs] = n >> 8;
                        data[ofs + 1] = n & 0xff;
                    }
                    else
                    {
                        data[ofs] = n & 0xff;
                        data[ofs+1] = n >> 8;
                    }
                }
                else // msize == 4
                {
                    if (bigEndian)
                    {
                        data[ofs + 0] = n >> 24;
                        data[ofs + 1] = n >> 16;
                        data[ofs + 2] = n >>  8;
                        data[ofs + 3] = n & 0xff;
                    }
                    else
                    {
                        data[ofs] = n & 0xff;
                        data[ofs+1] = n >> 8;
                        data[ofs+2] = n >> 16;
                        data[ofs+3] = n >> 24;
                    }
                }
            }
            else
            {
                if ((*it)->IsEnumerated())
                    memset(data + ofs, (*it)->GetFill(), msize);
                else
                    memcpy(data + ofs, mdata, msize);
            }
            ofs += msize;
        }
    }
}
void LEObject::InitFlags()
{
    name = sect->GetName();
    initSize = size = sect->GetSize()->Eval(0);
    base_addr = sect->GetOffset()->Eval(0);
    if (name == ".text")
    {
        SetFlags(LX_OF_READABLE | LX_OF_EXECUTABLE | LX_OF_BIGDEFAULT /*| LX_OF_PRELOAD*/);
    }
    else if (name == ".data")
    {
        SetFlags(LX_OF_READABLE | LX_OF_WRITEABLE | LX_OF_BIGDEFAULT);
    }
    else if (name == ".bss")
    {
        SetFlags(LX_OF_READABLE | LX_OF_WRITEABLE | LX_OF_BIGDEFAULT | LX_OF_ZEROFILL);
        initSize = 0;
    }
    else if (name == ".stack")
    {
        SetFlags(LX_OF_READABLE | LX_OF_WRITEABLE | LX_OF_BIGDEFAULT | LX_OF_ZEROFILL);
        initSize = 0;
    }
    else
    {
        SetFlags(LX_OF_READABLE | LX_OF_WRITEABLE | LX_OF_BIGDEFAULT);
    }
}
void LEObject::WriteHeader(std::fstream &stream)
{
    unsigned temp = size; //ObjectAlign(4096, size);
    stream.write((char *)&temp, sizeof(temp));
    stream.write((char *)&base_addr, sizeof(base_addr));
    stream.write((char *)&flags, sizeof(flags));
//	if (initSize == 0)
//		pageOffs = 1;
    stream.write((char *)&pageOffs, sizeof(pageOffs));
    temp = ObjectAlign(4096, initSize)/4096;
    stream.write((char *)&temp, sizeof(temp));
    temp = 0 ;
    stream.write((char *)&temp, sizeof(temp));
}
void LEObject::Write(std::fstream &stream)
{
    stream.write((char *)data, initSize);
    int n = ObjectAlign(4096, initSize) - initSize;
    char buf[512];
    memset(buf, 0, sizeof(buf));
    while (n > 0)
    {
        int s = sizeof(buf);
        if (n < sizeof(buf))
            s = n;
        stream.write(buf, s);
        n -= s;
    }
}
void LEObject::SetFile(ObjFile *File)
{
    file = File;
}
