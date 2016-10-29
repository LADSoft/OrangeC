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
#ifndef LEObject_H
#define LEObject_H

#include <string>
#include <set>
#include "ObjTypes.h"
class ObjSection;
class ObjFile;
class ObjExpression;
inline ObjInt ObjectAlign(ObjInt alignment, ObjInt value)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

class LEObject
{
public:
    enum { HeaderSize = 6 * 4 };
    
    LEObject(ObjSection *Sect) : data(NULL), sect(Sect) { InitFlags(); }
    virtual ~LEObject() { if (data) delete data; }
    void Setup(unsigned &offs);
    void InitFlags();
    unsigned GetSize() { return size; }
    unsigned GetInitSize() { return initSize; }
    void SetSize(unsigned Size) { initSize = size = Size; }
    void SetInitSize(unsigned Size) { initSize = size; }
    unsigned GetAddr() { return base_addr; }
    void SetAddr(unsigned addr) { base_addr = addr; }
    unsigned GetFlags() { return flags; }
    void SetFlags(unsigned Flags) { flags = Flags; }
    unsigned GetPageOffs() { return pageOffs; }
    void WriteHeader(std::fstream &stream);
    void Write(std::fstream &stream);
    static void SetFile(ObjFile *File);
protected:
    bool IsRel(ObjExpression *e);
    unsigned pageOffs;
    unsigned size;
    unsigned initSize;
    unsigned base_addr;
    unsigned flags;
    unsigned char *data;
    std::string name;
    static ObjFile *file;
private:
    ObjSection *sect;
};
#endif