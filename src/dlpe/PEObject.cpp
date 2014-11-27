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
#include "PEObject.h"
#include "ObjSection.h"
#include "ObjFile.h"
#include "ObjSymbol.h"
#include "ResourceContainer.h"
#include <string.h>

unsigned PEObject::objectAlign = 0x1000;
unsigned PEObject::fileAlign = 0x200;
unsigned PEObject::imageBase = 0x400000;
unsigned PEObject::dataInitSize;
unsigned PEObject::importThunkVA;

ObjFile *PEObject::file;

void PEObject::WriteHeader(std::fstream &stream)
{
    unsigned zero = 0;
    for (int i =0; i < name.size() && i < 8; i++)
        stream.write(&name[i], 1);
    for (int i = name.size(); i < 8; i++)
        stream.write((char *)&zero, 1);
    unsigned msize = ObjectAlign(objectAlign, size);
    stream.write((char *)&msize, 4);
    stream.write((char *)&virtual_addr, 4);
    msize = ObjectAlign(fileAlign, initSize);
    stream.write((char *)&msize, 4);
    stream.write((char *)&raw_addr, 4);
    stream.write((char *)&zero, 4);
    stream.write((char *)&zero, 4);
    stream.write((char *)&zero, 4);
    int flg = (flags ^ WINF_NEG_FLAGS) & WINF_IMAGE_FLAGS; /* get characteristice for section */
    stream.write((char *)&flg, 4);
}
void PEObject::Write(std::fstream &stream)
{
    stream.write((char *)data, initSize);
    int n = ObjectAlign(fileAlign, initSize) - initSize;
    char buf[512];
    memset(buf,0, sizeof(buf));
    while (n > 0)
    {
        int s = sizeof(buf);
        if (n < s)
            s = n;
        stream.write(buf, s);
        n -= s;
    }
}
void PEObject::SetFile(ObjFile *File)
{
    file = File;
    for (ObjFile::SymbolIterator it = file->DefinitionBegin(); it != file->DefinitionEnd(); it++)
    {
        ObjDefinitionSymbol *p = (ObjDefinitionSymbol *)(*it);
        if (p->GetName() == "FILEALIGN")
        {
            fileAlign = p->GetValue();
        }
        else if (p->GetName() == "IMAGEBASE")
        {
            imageBase = p->GetValue();
        }
        else if (p->GetName() == "IMPORTTHUNKS")
        {
            importThunkVA = p->GetValue();
        }
        else if (p->GetName() == "INITSIZE")
        {
            dataInitSize = p->GetValue();
        }
        else if (p->GetName() == "OBJECTALIGN")
        {
            objectAlign = p->GetValue();
        }
    }
}
