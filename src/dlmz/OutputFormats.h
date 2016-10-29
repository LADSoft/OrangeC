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
#ifndef OutputFormats_h
#define OutputFormats_h

#include <fstream>
#include <deque>
#include <map>
#include "MZHeader.h"
class ObjFile;
class ObjExpression;
class OutFile
{
public:
    OutFile() : data(0), size(0), startSeg(0), startOffs(0), stackSeg(0), stackOffs(0) { }	
    virtual ~OutFile() {  delete data; }
    virtual bool ReadSections(ObjFile *file, ObjExpression *start) = 0;
    virtual bool Write(std::fstream &stream) = 0;
protected:
    unsigned char *data;
    int size;
    int startSeg, startOffs;
    int stackSeg, stackOffs;
};
class Tiny : public OutFile
{
public:
    Tiny() : OutFile() { }
    virtual bool ReadSections(ObjFile *file, ObjExpression *start);
    virtual bool Write(std::fstream &stream);
} ;
class Real : public OutFile
{
public:
    Real() : OutFile() { } 
    virtual bool ReadSections(ObjFile *file, ObjExpression *start);
    virtual bool Write(std::fstream &stream);
protected:
    void GetSectionBalance(ObjExpression *n, int &add, int &sub, bool positive);
    bool Balanced(ObjExpression *n, bool skipping);
    void WriteHeader(std::fstream &stream);
    int GetFixupOffset(ObjExpression *fixup, int sbase, int pc);
    int GetFirstSeg(ObjExpression *exp);
    struct Fixup
    {
        Fixup() :seg(0), off(0) {}  
        Fixup(int Seg, int Off) : seg(Seg), off(Off) { }
        int seg;
        int off;
    } ;
    MZHeader header;
    std::deque<Fixup> fixups;	
};

#endif