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
#ifndef ResourceData_h
#define ResourceData_h

#include <stdlib.h>
#include <string>

class RCFile;
class ResFile;

class ResourceData
{
public:
    ResourceData() : len(0), data(NULL), pos(0) { }
    ResourceData(const unsigned char *rdata, size_t len): len(0), data(NULL), pos(0) 
    {
        SetData(rdata, len);
    }
    ~ResourceData() { delete data; }
    
    size_t GetLen() const { return len; }
    const unsigned char *GetData() const { return data; }
    void SetData(const unsigned char *rdata, size_t rlen)
    {
        delete data;
        len = rlen;
        data = new unsigned char[len];
        memcpy(data, rdata, len);
    }
    unsigned GetByte() { return data[pos++]; }
    unsigned GetWord() { int n = data[pos] + (data[pos +1] << 8); pos+=2; return n; }
    unsigned GetDWord() { int n = data[pos] + (data[pos +1] << 8) + (data[pos +2] << 24) + (data[pos +3] << 24); pos+=4; return n; }
    bool PastEnd() { return pos >= len; }
    void WriteRes(ResFile &resFile);
    void ReadRC(RCFile &rcFile);
    int operator [] (int val) { return data[val]; }		
private:
    int len;
    unsigned char *data;
    int pos;
} ;

#endif
