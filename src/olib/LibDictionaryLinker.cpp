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
#include "LibDictionary.h"
#include "ObjFile.h"
#include <ctype.h>
#include <iostream>

bool DictCompare::caseSensitive;

ObjInt DictCompare::casecmp(const char *str1, const char *str2, int n) const
{
    while (*str1 && *str2 && n)
    {
        int u1 = *str1;
        int u2 = *str2;
        if (!caseSensitive)
        {
            u1 = toupper(u1);
            u2 = toupper(u2);
        }
        if (u1 != u2)
            break;
        str1++, str2++, n--;
    }
    if (n == 0)
    {
        if (!*str2)
            return 0;
        return -1;
    }
    else if (!*str2)
        return 1;
    else
        return *str1 < *str2 ? -1 : 1;
}
ObjInt LibDictionary::Lookup(FILE *stream, ObjInt dictionaryOffset, ObjInt dictionarySize, const ObjString &name)
{
    if (!dictionary.size())
    {
        fseek(stream, 0, SEEK_END);
        int end = ftell(stream);
        int size = end - dictionaryOffset;
        ObjByte *buf = new ObjByte[size];
        fseek(stream, dictionaryOffset, SEEK_SET);
        fread(buf, size,1, stream);
        ObjByte *q = buf;
        char sig[4] = { '1','0',0,0 }, sig1[4];
        if (!memcmp(sig, buf, 4))
        {
            int len;
            q += 4;
            len = *(short *)q;
            while (len)
            {
                q += 2;
                int fileNum = *(int *)(q + len);
                q[len] = 0;
                dictionary[(char *)q] = fileNum;
                q += len + 4;
                len = *(short *)q;
            }
        }
        else
        {
            std::cout << "Old format library detected, please rebuild libraries" << std::endl;
        }
        delete [] buf;
    }
    std::map<ObjString, ObjInt, DictCompare>::iterator it= dictionary.find(name);
    if (it != dictionary.end())
        return it->second;
    return -1;
}
