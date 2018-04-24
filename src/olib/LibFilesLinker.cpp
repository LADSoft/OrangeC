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
#include "LibManager.h"
#include "LibFiles.h"
#include "ObjFile.h"
#include "ObjIeee.h"
#include "ObjFactory.h"
#include <assert.h>

ObjFile *LibFiles::ReadData(FILE *stream, const ObjString &name, ObjFactory *factory)
{
    ObjIeeeIndexManager im1;
    ObjIeee ieee(name.c_str(), caseSensitive);
    return ieee.Read(stream, ObjIeee::eAll, factory);
}
bool LibFiles::ReadNames(FILE *stream, int count)
{
    for (int i=0; i < count; i++)
    {
        char c = ' ';
        char buf[260], *p = buf;
        do
        {
            c = fgetc(stream);
//            if (stream.fail())
//                return false;
            *p++ = c;
        } while (c != 0);
        ObjString name = buf;
        FileDescriptor *d = new FileDescriptor(name);
        files.push_back(d);
    }
    return true;
}
void LibFiles::ReadOffsets(FILE *stream, int count)
{
    assert(count == files.size());
    for (FileIterator it = FileBegin(); it != FileEnd(); ++it)
    {
        unsigned ofs;
        fread(&ofs, 4, 1, stream);
        (*it)->offset = ofs;
    }
}
ObjFile *LibFiles::LoadModule(FILE *stream, ObjInt FileIndex, ObjFactory *factory)
{
    if (FileIndex >= files.size())
        return nullptr;
    const FileDescriptor *a = files[FileIndex];
    if (!a->offset)
        return nullptr;
    fseek(stream, a->offset, SEEK_SET);
    return ReadData(stream, a->name, factory);
}
