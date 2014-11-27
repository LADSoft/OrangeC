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
#ifndef OUTPUTOBJECTS_H
#define OUTPUTOBJECTS_H

#include <fstream>
#include <string>
#include "ObjTypes.h"
class OutputObject
{
public:
    OutputObject() { }
    virtual ~OutputObject() { }
    
    virtual int Write(std::fstream &stream, char *data, int line, int firstAddress) = 0;
    virtual int Pad(std::fstream &stream, ObjInt addr, ObjInt size, int padChar);	
    virtual int WriteHeader(std::fstream &stream) = 0;
    virtual int WriteTrailer(std::fstream &stream) = 0;
    virtual std::ios::openmode GetOpenFlags() = 0;
};

class IntelOutputObject : public OutputObject
{
public:
    IntelOutputObject(int Stype) : stype(Stype) { }
    virtual ~IntelOutputObject() { }
    
    virtual int Write(std::fstream &stream, char *data, int line, int firstAddress);
    virtual int WriteHeader(std::fstream &stream);
    virtual int WriteTrailer(std::fstream &stream);
    virtual std::ios::openmode GetOpenFlags() { return std::ios::out; }
protected:
    void putrecord(std::fstream &stream, unsigned char *data, int datalen, int offset);
    void putulba(std::fstream &stream, int address);
private:
    int stype;
};

class MotorolaOutputObject: public OutputObject
{
public:
    MotorolaOutputObject(const std::string &Name, int Type) : type(Type), name(Name){ }
    virtual ~MotorolaOutputObject() { }
    
    virtual int Write(std::fstream &stream, char *data, int line, int firstAddress);
    virtual int WriteHeader(std::fstream &stream);
    virtual int WriteTrailer(std::fstream &stream);
    virtual std::ios::openmode GetOpenFlags() { return std::ios::out; }
protected:
    void putdatarec(std::fstream &stream, unsigned char *data, int datalen, long offset);
    void putendrec(std::fstream &stream);
    void putheaderrec(std::fstream &stream);
private:
    int type;
    const std::string &name;
};
class BinaryOutputObject : public OutputObject
{
public:
    BinaryOutputObject() { }
    virtual ~BinaryOutputObject() { }
    
    virtual int Write(std::fstream &stream, char *data, int line, int firstAddress);
    virtual int WriteHeader(std::fstream &stream) { return 0; }
    virtual int WriteTrailer(std::fstream &stream) { return 0; }
    virtual std::ios::openmode GetOpenFlags() { return std::ios::out | std::ios::binary; }
    
};

#endif