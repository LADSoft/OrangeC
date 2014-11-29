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
#include "OutputObjects.h"
#include "ObjUtil.h"

void MotorolaOutputObject::putdatarec(std::fstream &stream, unsigned char *data, int datalen, long offset)
{
    int cs = 0,i;
    cs += offset &255;
    cs += (offset >>8) & 255;
    cs += datalen;
    cs += 3;
    int len = datalen + 3;
    switch(type) {
        case 1:
            stream << "S1" << ObjUtil::ToHex(len, 2).c_str() << ObjUtil::ToHex(offset & 0xffff, 4).c_str();
            break;
        case 2:
            len += 1;
            cs ++;
            cs += (offset >>16) & 255;
            stream << "S2" << ObjUtil::ToHex(len, 2).c_str() << ObjUtil::ToHex(offset & 0xffffff, 6).c_str();
            break;
        case 3:
            len += 2;
            cs += 2;
            cs += (offset >>16) & 255;
            cs += (offset >>24) & 255;
            stream << "S3" << ObjUtil::ToHex(len, 2).c_str() << ObjUtil::ToHex(offset & 0xffffffff, 8).c_str();
            break;
    }
    for (i=0; i < datalen; i++) 
    {	
        stream << ObjUtil::ToHex(data[i], 2).c_str();
        cs += data[i];
    }
    cs = 255-(cs & 255);
    stream << ObjUtil::ToHex(cs, 2).c_str() << std::endl;
}
void MotorolaOutputObject::putendrec(std::fstream &stream)
{
    int cs = 0;
    int entryPoint = 0;
    cs += entryPoint &255;
    cs += (entryPoint >>8) & 255;
    cs += 3; /* record size */
    switch(type) {
        case 1:
            stream << "S903" << ObjUtil::ToHex(entryPoint & 0xffff, 4).c_str();
            break;
        case 2:
            cs += 1+ (entryPoint >>16) & 255;
            stream << "S805" << ObjUtil::ToHex(entryPoint & 0xffffff, 6).c_str();
            break;
        case 3:
            cs += 1+ (entryPoint >>16) & 255;
            cs += 1+ (entryPoint >>24) & 255;
            stream << "S709" << ObjUtil::ToHex(entryPoint & 0xffffffff, 8).c_str();
            break;
    }
    cs = 255-(cs & 255);
    stream << ObjUtil::ToHex(cs, 2).c_str() << std::endl;
}
void MotorolaOutputObject::putheaderrec(std::fstream &stream)
{
    int cs = 0,i;
    int len = name.size() + 5;
    cs +=len ;
    stream << "S0" << ObjUtil::ToHex(len,2).c_str() << "0000";
    const char *p = name.c_str();
    while (*p)
    {
        stream << ObjUtil::ToHex(*p).c_str();
        cs += *p++;
    }
    cs = 255-(cs & 255);
    stream << ObjUtil::ToHex(cs).c_str() << std::endl;
}
int MotorolaOutputObject::Write(std::fstream &stream, char *data, int len, int firstAddress)
{
    while (len)
    {
        int max = 16;
        if (type == 3)
            max = 15;
        int m = len > max ? max : len;
        putdatarec(stream, (unsigned char *)data, m, firstAddress);
        firstAddress += m;
        data += m;
        len -= m;
    }
    return 0;
}
int MotorolaOutputObject::WriteHeader(std::fstream &stream)
{
    putheaderrec(stream);
    return 0;
}
int MotorolaOutputObject::WriteTrailer(std::fstream &stream)
{
    putendrec(stream);
    return 0;
}
