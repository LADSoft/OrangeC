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
#include "LibManager.h"
#include "Objieee.h"
#include "Objfactory.h"
#include <string.h>

bool LibManager::SaveLibrary()
{
    InitHeader();
    ObjIeeeIndexManager im1;
    ObjFactory fact1(&im1);
    if (!files.ReadFiles(stream, &fact1))
        return false;
    dictionary.CreateDictionary(files);
    // can't do more reading
    Close();
    header.filesInModule = files.size();
    FILE * ostr = fopen(name.c_str(), "wb");
    fwrite(&header, sizeof(header), 1, ostr);
    Align(ostr,16);
    header.namesOffset = ftell(ostr);
    files.WriteNames(ostr);
    Align(ostr);
    header.filesOffset = ftell(ostr);
    files.WriteFiles(ostr, ALIGN);
    Align(ostr);
    header.offsetsOffset = ftell(ostr);
    files.WriteOffsets(ostr);
    Align(ostr);
    header.dictionaryOffset = ftell(ostr);
    header.dictionaryBlocks = dictionary.GetBlockCount();
    dictionary.Write(ostr);
    fseek(ostr, 0, SEEK_SET);
    fwrite(&header, sizeof(header), 1, ostr);
    return true;
//    return !ostr.fail();
}
void LibManager::Align(FILE *ostr, ObjInt align)
{
    char buf[ALIGN];
    memset(buf, 0, align);
    int n = ftell(ostr);
    if ( n % align)
        fwrite(buf, align - (n % align), 1, ostr);
}
