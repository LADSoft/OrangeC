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
#ifndef Instruction_h
#define Instruction_h

#include "Label.h"
#include "AsmExpr.h"

class Fixup;
class AsmFile;

#include <vector>
#include <string>

typedef std::vector<Fixup *> FixupContainer;
class Instruction
{
public:
    enum iType { LABEL, DATA, CODE, ALIGN, RESERVE };
    Instruction(Label *lbl ) : data(NULL), label(lbl), type(LABEL), pos(0), fpos(0),
            size(0), offs(0), repeat(1) { }
    Instruction(unsigned char *Data, int Size, bool isData=false ) : type(isData ? DATA : CODE), 
            label(NULL), pos(0), fpos(0), repeat(1),
            size(Size), offs(0) { data = new unsigned char[size]; memcpy(data, Data, size); }
    Instruction(int aln) : data(NULL), type(ALIGN), label(NULL), pos(0), fpos(0), size(aln), offs(0), repeat(1) { }
    Instruction(int Repeat, int Size) : type(RESERVE), label(NULL), pos(0), fpos(0), size(Size), repeat(Repeat), offs(0) { data = new unsigned char[size]; memset(data, 0, size); }
    virtual ~Instruction();

    Label *GetLabel() { return label; }
    bool IsLabel() { return type == LABEL; }
    int GetType() { return type; }
    void Optimize(int pc, bool doErrors);
    void SetOffset(int Offs) { offs = Offs; }
    int GetOffset() { return offs; }
    int GetSize() { if (type == ALIGN) { int n = size - offs % size; if (n == size) return 0; else return n; }
                    else if (type == RESERVE) return size * repeat; else return size; }
    int GetRepeat() { return repeat; }
    int GetNext(Fixup &fixup, unsigned char *buf);
    void Rewind() { pos = fpos = 0; }
    static bool ParseSectionAttrib(AsmFile *file);
    void Add(Fixup *fixup) { fixups.push_back(fixup); }
    unsigned char *GetBytes() { return data; }
    FixupContainer *GetFixups() { return &fixups; }
    static void SetBigEndian(bool be) { bigEndian = be; }
private:
    enum iType type;
    unsigned char *data;
    Label *label;
    int size;
    int offs;
    int pos;
    int fpos;
    int repeat, xrepeat;

    FixupContainer fixups;
    static bool bigEndian;
} ;
#endif