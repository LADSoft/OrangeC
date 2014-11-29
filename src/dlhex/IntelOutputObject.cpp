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

void IntelOutputObject::putrecord(std::fstream &stream, unsigned char *data, int datalen, int offset)
{
    int cs = 0,i;
    cs += offset &255;
    cs += (offset >>8) & 255;
        
    cs += datalen;
    stream << ":" << ObjUtil::ToHex(datalen, 2).c_str() << ObjUtil::ToHex(offset & 0xffff, 4).c_str() << "00"; // data record
    for (i=0; i < datalen; i++) {
        stream << ObjUtil::ToHex(data[i], 2).c_str();
        cs += data[i];
    }
    cs = (256-(cs & 255)) & 0xff;
    stream << ObjUtil::ToHex(cs,2).c_str() << std::endl;
}
void IntelOutputObject::putulba(std::fstream &stream, int address)
{
    int cs = 0,i;
    int len;
    int offset ;
    int type;
    if (stype == 4)
    {
        // linear
        type = 4;
        offset = (address >> 16) & 0xffff;
        len = 2;
    }
    else
    {
        // segmented
        type = 2;
        offset = (address >> 4) & 0xf000;
        len = 2;
    }
    cs += type;
    cs += len;
    cs += offset &255;
    cs += (offset >>8) & 255;
    cs = (256-(cs & 255)) & 0xff;
        
    stream << ":" << ObjUtil::ToHex(len, 2).c_str() << "0000" << ObjUtil::ToHex(type, 2).c_str() << ObjUtil::ToHex(offset, 4).c_str() << ObjUtil::ToHex(cs, 2).c_str() << std::endl;
}
int IntelOutputObject::Write(std::fstream &stream, char *data, int len, int firstAddress)
{
    if ((firstAddress & 0xffff0000) || stype == 2)
        putulba(stream, firstAddress);
    while (len)
    {
        int m = len > 16 ? 16 : len;
        int n = ((firstAddress + 0x10000) & 0xffff0000) - firstAddress;
        m = n < m ? n : m;
        putrecord(stream, (unsigned char *)data, m, firstAddress);
        firstAddress += m;
        data += m;
        len -= m;
        if (len && (firstAddress & 0xffff) == 0)
            putulba(stream, firstAddress);
    }
    return 0;
}
int IntelOutputObject::WriteHeader(std::fstream &stream)
{
    return 0;
}
int IntelOutputObject::WriteTrailer(std::fstream &stream)
{
    stream << ":00000001FF" << std::endl; // end record
    return 0;
}
