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
#ifndef LEfixup_h
#define LEfixup_h

#include "ObjTypes.h"
#include <fstream>
#include <deque>
#include <map>

class ObjFile;
class ObjSection;
class ObjExpression;

class LEObject;

class LEFixup
{
public:
    LEFixup(ObjFile &file, std::deque<LEObject *>&Objects, bool Lx) : objects(Objects), pages(0), fixupSize(0), indexTableSize(0), lx(Lx) { LoadFixups(file); }
    virtual ~LEFixup() { if (fixupTable) delete [] fixupTable; if (indexTable) delete[] indexTable; }
    
    void Setup();
    void Write(std::fstream &stream);
    unsigned CreateSections();
    unsigned GetFixupSize() { return fixupSize; }
    unsigned GetIndexTableSize() { return indexTableSize; }
    int SectionOf(ObjExpression *e);
protected:
    bool IsRel(ObjExpression *e);
    void LoadFixups(ObjFile &file);
    struct Target {
        Target() : target(0), section(-1), origSection(0) { }
        Target(ObjInt targ, int sect, int origSect) : target(targ), section(sect), origSection(origSect) { }
        ObjInt target;
        int section;
        int origSection;
    } ;
    std::map<ObjInt, Target> fixups;
    std::deque<LEObject *> &objects;
    unsigned pages;
    unsigned char *fixupTable;
    unsigned char *indexTable;
    unsigned fixupSize;
    unsigned indexTableSize;
    bool lx;
} ;
#endif