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
#include "ObjFile.h"
#include "ObjSection.h"
#include "ObjExpression.h"
#include "OutputFormats.h"
#include "Utils.h"
bool Tiny::ReadSections(ObjFile *file, ObjExpression *start)
{
    startOffs = start->Eval(0);
    if (startOffs != 0x100)
        Utils::fatal("Start address for tiny program must be 0100h");
    int count = 0;
    ObjSection *sect;
    for (ObjFile::SectionIterator it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        sect = *it;
        size = (*it)->GetOffset()->Eval(0) + (*it)->GetSize()->Eval(0);
        count ++;
    }
    if (count != 1 || size < 0x100)
        return false;
    data = new unsigned char[size];
    ObjMemoryManager &m = sect->GetMemoryManager();
    int ofs = 0;
    for (ObjMemoryManager::MemoryIterator it = m.MemoryBegin(); it != m.MemoryEnd(); ++it)
    {
        int msize = (*it)->GetSize();
        ObjByte *mdata = (*it)->GetData();
        if (msize)
        {
            ObjExpression *fixup = (*it)->GetFixup();
            if (fixup)
            {
                if (fixup->GetOperator() == ObjExpression::eDiv)
                    Utils::fatal("Tiny program cannot have fixups");
                int sbase = sect->GetOffset()->Eval(0);
                int n = fixup->Eval(sbase + ofs);
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
    return true;
}
bool Tiny::Write(std::fstream &stream)
{
    stream.write((char *)data + 0x100, size - 0x100);
    return true;
}
